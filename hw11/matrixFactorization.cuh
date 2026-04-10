//a series of functions for calculating special matrix decompositions
#ifndef MATRIX_FACROTIZATION_CUH //Guard start
#define MATRIX_FACROTIZATION_CUH // Guard define

void RandomInit(float x[], const unsigned int n);

// Cholesky factorization
void localCholesky(int n, const float A[], float L[]);
//calculate one diagonal in a Cholesky factorization L_jj = sqrt(A_jj - sum from 1-(j-1) of L_jk^2
//does a single BWxBW diagonal block, one thread per row
__global__ void Cholesky_Diagonal(float* A, int n, int k, int Bw);

//handle the panel below the diagonal, the column of blocks below the factorized diagonal
//
//i > j: L_ij = (1/L_jj) (A_ij - sum from k-(j-1) of L_ik * L_jk
//the non-summation part of the equation, applies the sum, then performs the division
//A_ij - Sum
//also handels the division (1/L_jj) after the summation has finished
__global__ void Cholesky_Panel(float* A, int n, int k_block, int Bw);

// A_ij = A_ij - L_ik * (L_jk transpose)
//update remaining lower-triangle blocks
// just the summation part of the non-diagonal solve (each round is one part of the summation) to be added next time panel is run
__global__ void Cholesky_Update(float* A, int n, int k_block, int Bw);

//cholenky solved on the GPU
//first calculate diagonal, second calculate non-diagonal
void GPUCholesky(float* Ah, float* Lh, int Bn, int Bw);

//generates random matrix valid for Cholesky
//must be Symmetric, positive-definite
//Symmetric: A=A trasnpose
//Positive-Definite: all eigenvvalues are positive, or (equivalently) all diagonal elements in the decomposition are exclusively positive
void generate_valid_matrix(int n, float A[]);

// LU factorization
void localLU(int n, const float A[], float L[], float U[]);

//the kernal for solving a step of LU factorization
__global__ void luStepKernel(int n, int i, double* A, double* L, double* U);

// LU factorization solved with parallelization on the GPU
void GPULU(const float A[], float L[], float U[], int Bn, int Bw);

#endif // MATRIX_FACROTIZATION_CUH Guard end