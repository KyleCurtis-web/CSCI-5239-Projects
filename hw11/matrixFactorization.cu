#include "matrixFactorization.cuh"
#include "InitGPUcu.h"
#include "CSCIx239.h"


//
//  Initialize matrix with random values
//
void RandomInit(float x[], const unsigned int n)
{
    for (unsigned int i = 0; i < n * n; i++)
        x[i] = rand() / (float)RAND_MAX;
}

//
//Cholesky Stuff
//


// Cholesky factorization
void localCholesky(int n, const float A[], float L[])
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j <= i; j++) 
        {
            double sum = 0;

            // get the sumation, L_jk^2 = L_ik * L_jk when i=j, so its the same sum either way
            for (int k = 0; k < j; k++)//sum to j-1 of L_ik * L_jk 
            {
                sum += L[i * n + k] * L[j * n + k];
            }

            //decide what to do to the summation
            if (i == j) //diagonal element, sqrt(A_ij - sum)
            {
                double val = A[i * n + i] - sum;
                L[i * n + j] = (val > 0) ? sqrt(val) : 0; //must be positive, so gurantee sqrt is positive or 0
            }
            else //non-diagonal element, 1/L_jj (A_ij - sum)
            {
                L[i * n + j] = ( (1.0 / L[j * n + j]) * (A[i * n + j] - sum));
            }
        }
    }
}


//calculate one diagonal in a Cholesky factorization L_jj = sqrt(A_jj - sum from 1-(j-1) of L_jk^2
//does a single BWxBW diagonal block, one thread per row
__global__ void Cholesky_Diagonal(float* A, int n, int k, int Bw) {
    int start = k * Bw;
    for (int i = 0; i < Bw; i++) 
    {
        __syncthreads();
        if (threadIdx.x == 0) 
        { // Thread 0 handles the diagonal element
            float val = A[(start + i) * n + (start + i)]; //A_jj
            for (int m = 0; m < i; m++) 
            {
                val -= A[(start + i) * n + (start + m)] * A[(start + i) * n + (start + m)]; // - summation
            }
            A[(start + i) * n + (start + i)] = sqrtf(val); //take the square root
        }
        __syncthreads();

        // Threads parallelize the panel below the diagonal within this tile
        if (threadIdx.x > i && threadIdx.x < Bw) 
        {
            int row = start + threadIdx.x;
            float val = A[row * n + (start + i)];
            for (int m = 0; m < i; m++) 
            {
                val -= A[row * n + (start + m)] * A[(start + i) * n + (start + m)];
            }
            A[row * n + (start + i)] = val / A[(start + i) * n + (start + i)]; //update the panels that depend on this block
        }
    }
}

//handle the panel below the diagonal, the column of blocks below the factorized diagonal
//
//i > j: L_ij = (1/L_jj) (A_ij - sum from k-(j-1) of L_ik * L_jk
//the non-summation part of the equation, applies the sum, then performs the division
//A_ij - Sum
//also handels the division (1/L_jj) after the summation has finished
__global__ void Cholesky_Panel(float* A, int n, int k_block, int Bw) {
    int i_block = k_block + 1 + blockIdx.x; // Block index in the panel
    int row = i_block * Bw + threadIdx.y;
    int col = k_block * Bw + threadIdx.x;

    for (int k = 0; k < Bw; k++) 
    {
        float sum = 0;
        for (int m = 0; m < k; m++) 
        {
            sum += A[row * n + (k_block * Bw + m)] * A[(k_block * Bw + k) * n + (k_block * Bw + m)]; //calculate the part of the sum that relies on this column
        }
        __syncthreads();
        // Row-wise triangular solve dependency
        if (threadIdx.x == k) 
        {
            A[row * n + col] = (A[row * n + col] - sum) / A[(k_block * Bw + k) * n + (k_block * Bw + k)]; //apply the part of the sum calculated above, then perform the division
        }
        __syncthreads();
    }
}

// A_ij = A_ij - L_ik * (L_jk transpose)
//update remaining lower-triangle blocks
// just the summation part of the non-diagonal solve
//basically pre-computes and applies part of the sum that relies on this column
__global__ void Cholesky_Update(float* A, int n, int k_block, int Bw) {
    int row_block = k_block + 1 + blockIdx.y;
    int col_block = k_block + 1 + blockIdx.x;

    // Only update the lower triangle blocks
    if (row_block >= col_block) 
    {
        int row = row_block * Bw + threadIdx.y;
        int col = col_block * Bw + threadIdx.x;

        if (row < n && col < n && row >= col) 
        {
            float sum = 0;
            for (int k = 0; k < Bw; k++) {
                // Multiplying elements from the current panel columns
                sum += A[row * n + (k_block * Bw + k)] * A[col * n + (k_block * Bw + k)];//calculating the part of the summation that relies on this column L_ik * L_jk
            }
            A[row * n + col] -= sum;//doing the part of the summation that relies on this column (actually subtracting it)
        }
    }
}

//cholenky solved on the GPU
//first calculate diagonal, second calculate non-diagonal
//breaks the matrix into smaller blocks, that get solved
//first we traverse the diagonal, solving the diagonal blocks
//then we traverse the remaining space below the diagonal blocks, and update those
// //for each block along the diagonal
//1: panel factorization, factorize the current diagonal block
//2: triangular solve: update remaining blocks in the column (the panel)
//3: trailing matrix update: update the non-diagonal elements not in a diagonal block
void GPUCholesky(float* Ah, float* Lh, int Bn, int Bw)
{
    // Calculate dimensions
    int n = Bw * Bn;
    int N = n * n * sizeof(float);

    // Allocate device memory
    float* Ad;
    if (cudaMalloc((void**)&Ad, N)) Fatal("Cannot allocate device memory Ad\n");

    // Copy A from host to device
    if (cudaMemcpy(Ad, Ah, N, cudaMemcpyHostToDevice)) Fatal("Cannot copy A from host to device\n");

    // Define execution configurations
    dim3 threads(Bw, Bw);
    // Note: Diagonal kernel usually uses 1D threads (Bw, 1) for the small tile factorization

    // Main Loop: Iterate through blocks (each block dependent on previous block)
    for (int k = 0; k < Bn; k++) 
    {
        // 1. Factorize the diagonal block k
        // Block size 1x1, Threads Bw, since we have to do diagonal elements one at a time, sequential
        Cholesky_Diagonal << <1, Bw >> > (Ad, n, k, Bw);
        if (cudaGetLastError()) Fatal("Diagonal kernel failed\n");

        int remaining = Bn - k - 1;
        if (remaining > 0) //now we have done the diagonal, we can do all the non-diagonals dependent on that diagonal, in the column below the diagonal
        {
            // 2. Update the panel (column below diagonal), performs the division and applies the sum, parallelize each value below the diagonal in the column, each row gets its own thread
            // Grid is 'remaining' blocks long, Threads Bw x Bw
            Cholesky_Panel << <remaining, threads >> > (Ad, n, k, Bw);
            if (cudaGetLastError()) Fatal("Panel kernel failed\n");

            // 3. Update the trailing submatrix (GEMM update), calculates the sum used in the next pass, remove the first column from the remaining submatrix solution, we subtract this part of the summation
            // Grid is 'remaining x remaining', Threads Bw x Bw
            dim3 grid_gemm(remaining, remaining);
            Cholesky_Update << <grid_gemm, threads >> > (Ad, n, k, Bw);
            if (cudaGetLastError()) Fatal("Update kernel failed\n");
        }
    }

    // Copy result back to host (L is stored in the lower triangle of Ad)
    if (cudaMemcpy(Lh, Ad, N, cudaMemcpyDeviceToHost)) Fatal("Cannot copy result from device to host\n");

    // Free device memory
    cudaFree(Ad);
}

//generates random matrix valid for Cholesky
//must be Symmetric, positive-definite
//Symmetric- A=A trasnpose
//Positive-Definite: all eigenvvalues are positive, or (equivalently) all diagonal elements in the decomposition are exclusively positive
void generate_valid_matrix(int n, float A[]) {
    srand(time(NULL));
    float* temp = (float*)malloc(n * n * sizeof(float));

    // 1. Fill temp matrix with random values
    RandomInit(temp, n);

    // 2. A = temp * temp_transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i * n + j] = 0; // Accessing A[i][j]
            for (int k = 0; k < n; k++) {
                // A[i][j] += temp[i][k] * temp[j][k]
                A[i * n + j] += temp[i * n + k] * temp[j * n + k]; //A[i][k] = A[i][k] * T[j][k]
            }
        }
    }

    // 3. Boost diagonal for stability (since we are working with floats, offset floating point error)
    for (int i = 0; i < n; i++) {
        A[i * n + i] += 0.5;
    }

    free(temp);
}


//
// SOMETHING ELSE
//