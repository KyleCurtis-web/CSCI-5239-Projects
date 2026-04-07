/*
 *  CUDA matrix factorization
 *
 *  The size of the matrix is width*blocks
 *
 *  Parameters:
 *  -v      Verbose - show hardware detila
 *  width   Block width (width squared <= max threads/block)
 *  blocks  Number of blocks
 */
#include "CSCIx239.h"
#include "InitGPUcu.h"

//
//  Initialize matrix with random values
//
void RandomInit(float x[],const unsigned int n)
{
   for (unsigned int i=0;i<n*n;i++)
      x[i] = rand() / (float)RAND_MAX;
}

//
// C = A * B -- host
//
void AxBh(float C[], const float A[], const float B[], unsigned int n)
{
   for (unsigned int i=0;i<n;i++)
      for (unsigned int j=0;j<n;j++)
      {
         double sum=0;
         for (unsigned int k=0;k<n;k++)
            sum += A[i*n+k] * B[k*n+j];
         C[i*n+j] = (float)sum;
      }
}

//
// Compute one element of A * B
//
__global__ void AxB(float C[],const float A[],const float B[],const unsigned int n)
{
   unsigned int j = blockIdx.x*blockDim.x+threadIdx.x;
   unsigned int i = blockIdx.y*blockDim.y+threadIdx.y;
   float sum =0;
   for (int k=0;k<n;k++)
      sum += A[i*n+k] * B[k*n+j];
   C[i*n+j] = sum;
}

//
// C = A * B -- device
//
void AxBd(float Ch[],const float Ah[],const float Bh[],const unsigned int Bw,const unsigned int Bn)
{
   //  Calculate matrix dimensions
   int n = Bw*Bn;
   int N = n*n*sizeof(float);

   // Allocate device memory
   float* Ad;
   float* Bd;
   float* Cd;
   if (cudaMalloc((void**)&Ad,N)) Fatal("Cannot allocate device memory Ad\n");
   if (cudaMalloc((void**)&Bd,N)) Fatal("Cannot allocate device memory Bd\n");
   if (cudaMalloc((void**)&Cd,N)) Fatal("Cannot allocate device memory Cd\n");

   // Copy A and B from host to device
   if (cudaMemcpy(Ad,Ah,N,cudaMemcpyHostToDevice)) Fatal("Cannot copy A from host to device\n");
   if (cudaMemcpy(Bd,Bh,N,cudaMemcpyHostToDevice)) Fatal("Cannot copy B from host to device\n");

   // Set size of block to Bw x Bw, and Bn x Bn blocks
   dim3 threads(Bw,Bw);
   dim3 grid(Bn,Bn);
   // Execute the kernel
   AxB<<<grid,threads>>>(Cd,Ad,Bd,n);
   if (cudaGetLastError()) Fatal("AxB failed\n");

   // Copy C from device to host
   if (cudaMemcpy(Ch,Cd,N,cudaMemcpyDeviceToHost)) Fatal("Cannot copy C from device to host\n");

   //  Free device memory
   cudaFree(Ad);
   cudaFree(Bd);
   cudaFree(Cd);
}

// cholensky factorization
void localCholesky(int n, const float A[], float L[])
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0;

            for (int k = 0; k < j; k++) {
                // sum += L[i][k] * L[j][k]
                sum += L[i * n + k] * L[j * n + k];
            }

            if (i == j) {
                // L[i][i] = sqrt(A[i][i] - sum)
                double val = A[i * n + i] - sum;
                L[i * n + j] = (val > 0) ? sqrt(val) : 0;
            }
            else {
                // L[i][j] = (1.0 / L[j][j] * (A[i][j] - sum))
                L[i * n + j] = (1.0 / L[j * n + j] * (A[i * n + j] - sum));
            }
        }
    }
}
//calculate one diagonal in a cholensky factorization L_jj = sqrt(A_jj - sum from 1-(j-1) of L_jk^2
//does a single BWxBW diagonal block, one thread per row
__global__ void Cholesky_Diagonal(float* A, int n, int k, int Bw) {
    int start = k * Bw;
    for (int i = 0; i < Bw; i++) {
        __syncthreads();
        if (threadIdx.x == 0) { // Thread 0 handles the diagonal element
            float val = A[(start + i) * n + (start + i)];
            for (int m = 0; m < i; m++) {
                val -= A[(start + i) * n + (start + m)] * A[(start + i) * n + (start + m)];
            }
            A[(start + i) * n + (start + i)] = sqrtf(val);
        }
        __syncthreads();

        // Threads parallelize the panel below the diagonal within this tile
        if (threadIdx.x > i && threadIdx.x < Bw) {
            int row = start + threadIdx.x;
            float val = A[row * n + (start + i)];
            for (int m = 0; m < i; m++) {
                val -= A[row * n + (start + m)] * A[(start + i) * n + (start + m)];
            }
            A[row * n + (start + i)] = val / A[(start + i) * n + (start + i)];
        }
    }
}

//handle the panel below the diagonal, the column of blocks below the factorized diagonal
//
//i > j: L_ij = (1/L_jj) (A_ij - sum from k-(j-1) of L_ik * L_jk
//the non-summation part of the equation
__global__ void Cholesky_Panel(float* A, int n, int k_block, int Bw) {
    int i_block = k_block + 1 + blockIdx.x; // Block index in the panel
    int row = i_block * Bw + threadIdx.y;
    int col = k_block * Bw + threadIdx.x;

    for (int k = 0; k < Bw; k++) {
        float sum = 0;
        for (int m = 0; m < k; m++) {
            sum += A[row * n + (k_block * Bw + m)] * A[(k_block * Bw + k) * n + (k_block * Bw + m)];
        }
        __syncthreads();
        // Row-wise triangular solve dependency
        if (threadIdx.x == k) {
            A[row * n + col] = (A[row * n + col] - sum) / A[(k_block * Bw + k) * n + (k_block * Bw + k)];
        }
        __syncthreads();
    }
}

// A_ij = A_ij - L_ik * (L_jk transpose)
//update remaining lower-triangle blocks
// just the summation part of the non-diagonal solve (each round is one part of the summation)
__global__ void Cholesky_Update(float* A, int n, int k_block, int Bw) {
    int row_block = k_block + 1 + blockIdx.y;
    int col_block = k_block + 1 + blockIdx.x;

    // Only update the lower triangle blocks
    if (row_block >= col_block) {
        int row = row_block * Bw + threadIdx.y;
        int col = col_block * Bw + threadIdx.x;

        if (row < n && col < n && row >= col) {
            float sum = 0;
            for (int k = 0; k < Bw; k++) {
                // Multiplying elements from the current panel columns
                sum += A[row * n + (k_block * Bw + k)] * A[col * n + (k_block * Bw + k)];
            }
            A[row * n + col] -= sum;
        }
    }
}

//cholenky solved on the GPU
//first calculate diagonal, second calculate non-diagonal
void GPUCholensky(float* Ah, float* Lh, int Bn, int Bw) 
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
    for (int k = 0; k < Bn; k++) {
        // 1. Factorize the diagonal block k
        // Block size 1x1, Threads Bw
        Cholesky_Diagonal << <1, Bw >> > (Ad, n, k, Bw);
        if (cudaGetLastError()) Fatal("Diagonal kernel failed\n");

        int remaining = Bn - k - 1;
        if (remaining > 0) {
            // 2. Update the panel (column below diagonal)
            // Grid is 'remaining' blocks long, Threads Bw x Bw
            Cholesky_Panel << <remaining, threads >> > (Ad, n, k, Bw);
            if (cudaGetLastError()) Fatal("Panel kernel failed\n");

            // 3. Update the trailing submatrix (GEMM update)
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

//generates random matrix valid for cholensky
void generate_valid_matrix(int n, float A[]) {
    srand(time(NULL));
    float* temp = (float*)malloc(n * n * sizeof(float));

    // 1. Fill flat temp matrix with random values
    RandomInit(temp, n);

    // 2. A = temp * temp_transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i * n + j] = 0; // Accessing A[i][j]
            for (int k = 0; k < n; k++) {
                // A[i][j] += temp[i][k] * temp[j][k]
                A[i * n + j] += temp[i * n + k] * temp[j * n + k];
            }
        }
    }

    // 3. Boost diagonal for stability
    for (int i = 0; i < n; i++) {
        A[i * n + i] += 0.5;
    }

    free(temp);
}

//
//  Main program
//
int main(int argc, char* argv[])
{
   //  Process options
   int opt;
   int verbose=0;
   while ((opt=getopt(argc,argv,"v"))!=-1)
   {
      if (opt=='v')
         verbose++;
      else
         Fatal("Usage: [-v] <block width> <number of blocks>\n");
   }
   argc -= optind;
   argv += optind;
 
   //  Get width and number of blocks
   if (argc!=2) Fatal("Usage: [-v] <block width> <number of blocks>\n");
   int Bw = atoi(argv[0]);
   if (Bw<1) Fatal("Block width out of range %d\n",Bw);
   int Bn = atoi(argv[1]);
   if (Bn<1) Fatal("Number of blocks out of range %d\n",Bn);
   //  Total width is block times number of blocks
   int n = Bw*Bn;
   int N = n*n*sizeof(float);
   printf("Bw=%d Bn=%d n=%d\n",Bw,Bn,n);

   //  Initialize GPU
   int Mw = InitGPU(verbose);
   if (Mw<Bw*Bw) Fatal("Thread count %d exceeds threads per block of %d\n",Bw*Bw,Mw);

   // Allocate host matrices A/B/C/R
   float* Ah = (float*)malloc(N);
   //float* Bh = (float*)malloc(N);
   float* Ch = (float*)malloc(N);
   float* Rh = (float*)malloc(N);
  // if (!Ah || !Bh || !Ch || !Rh) Fatal("Cannot allocate host memory\n");
   if (!Ah || !Ch || !Rh) Fatal("Cannot allocate host memory\n");

   // Initialize A & B
   srand(9999);
   //RandomInit(Ah,n);
   //RandomInit(Bh,n);

   //ensure it is good for cholensky
   //multiply by its transpose
   //add small value to diagonal
   generate_valid_matrix(n, Ah);
   //generate_valid_matrix(n, Bh);

   //  Compute R = AB on host
   Elapsed();
   //AxBh(Rh,Ah,Bh,n);
   localCholesky(n, Ah, Rh);
   double Th = Elapsed();

   //  Compute C = AB on device
   Elapsed();
   //AxBd(Ch,Ah,Bh,Bw,Bn);
   GPUCholensky(Ah, Ch, Bn, Bw);
   double Td = Elapsed();

   //  Compute difference between R and C
   double r2=0;
   for (int i=0;i<n*n;i++)
      r2 += fabs(Ch[i]-Rh[i]);
   r2 /= n*n;

   //  Free host memory
   free(Ah);
   //free(Bh);
   free(Ch);
   free(Rh);

   //  Print results
   printf("Host   Time = %6.3f s\n",Th);
   printf("Device Time = %6.3f s\n",Td);
   printf("Speedup = %.1f\n",Th/Td);
   printf("Difference = %.2e\n",r2);

   //  Done
   return 0;
}
