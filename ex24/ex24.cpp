/*
 *  OpenCL square matrix multiplier
 *
 *  The size of the matrix is width*blocks
 *
 *  Parameters:
 *  -v      Verbose - show hardware detila
 *  width   Block width (width squared <= max threads/block)
 *  blocks  Number of blocks
 */
#include "CSCIx239.h"
#include "InitGPUcl.h"
//  OpenCL globals
cl_device_id     devid;
cl_context       context;
cl_command_queue queue;

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
const char* source =
  "__kernel void AxB(__global float C[],__global const float A[],__global const float B[],const unsigned int n)\n"
  "{\n"
  "   unsigned int j = get_global_id(0);\n"
  "   unsigned int i = get_global_id(1);\n"
  "   float sum =0;\n"
  "   for (int k=0;k<n;k++)\n"
  "      sum += A[i*n+k] * B[k*n+j];\n"
  "   C[i*n+j] = sum;\n"
  "}\n";

//
// C = A * B -- device
//
void AxBd(float Ch[],float Ah[],float Bh[],const unsigned int Bw,const unsigned int Bn)
{
   //  Calculate matrix dimensions
   unsigned int n = Bw*Bn;
   unsigned int N = n*n*sizeof(float);

   // Allocate device memory and copy A&B from host to device
   cl_int  err;
   cl_mem Ad = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,N,Ah,&err);
   if (err) Fatal("Cannot create and copy A from host to device\n");
   cl_mem Bd = clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,N,Bh,&err);
   if (err) Fatal("Cannot create and copy B from host to device\n");

   //  Allocate device memory for C on device
   cl_mem Cd = clCreateBuffer(context,CL_MEM_WRITE_ONLY,N,NULL,&err);
   if (err) Fatal("Cannot create C on device\n");

   //  Compile kernel
   cl_program prog = clCreateProgramWithSource(context,1,&source,0,&err);
   if (err) Fatal("Cannot create program\n");
   if (clBuildProgram(prog,0,NULL,NULL,NULL,NULL))
   {
      char log[1048576];
      if (clGetProgramBuildInfo(prog,devid,CL_PROGRAM_BUILD_LOG,sizeof(log),log,NULL))
         Fatal("Cannot get build log\n");
      else
         Fatal("Cannot build program\n%s\n",log);
   }
   cl_kernel kernel = clCreateKernel(prog,"AxB",&err);
   if (err) Fatal("Cannot create kernel\n");

   //  Set parameters for kernel
   if (clSetKernelArg(kernel,0,sizeof(cl_mem),&Cd)) Fatal("Cannot set kernel parameter Cd\n");
   if (clSetKernelArg(kernel,1,sizeof(cl_mem),&Ad)) Fatal("Cannot set kernel parameter Ad\n");
   if (clSetKernelArg(kernel,2,sizeof(cl_mem),&Bd)) Fatal("Cannot set kernel parameter Bd\n");
   if (clSetKernelArg(kernel,3,sizeof(int),&n)) Fatal("Cannot set kernel parameter n\n");

   //  Run kernel
   size_t Global[2] = {n,n};
   size_t Local[2]  = {Bw,Bw};
   if (clEnqueueNDRangeKernel(queue,kernel,2,NULL,Global,Local,0,NULL,NULL)) Fatal("Cannot run kernel\n");

   //  Release kernel and program
   if (clReleaseKernel(kernel)) Fatal("Cannot release kernel\n");
   if (clReleaseProgram(prog)) Fatal("Cannot release program\n");

   // Copy C from device to host (block until done)
   if (clEnqueueReadBuffer(queue,Cd,CL_TRUE,0,N,Ch,0,NULL,NULL)) Fatal("Cannot copy C from device to host\n");

   //  Free device memory
   clReleaseMemObject(Ad);
   clReleaseMemObject(Bd);
   clReleaseMemObject(Cd);
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
   int Mw = InitGPU(verbose,devid,context,queue);
   if (Mw<Bw*Bw) Fatal("Thread count %d exceeds max work group size of %d\n",Bw*Bw,Mw);

   // Allocate host matrices A/B/C/R
   float* Ah = (float*)malloc(N);
   float* Bh = (float*)malloc(N);
   float* Ch = (float*)malloc(N);
   float* Rh = (float*)malloc(N);
   if (!Ah || !Bh || !Ch || !Rh) Fatal("Cannot allocate host memory\n");

   // Initialize A & B
   srand(9999);
   RandomInit(Ah,n);
   RandomInit(Bh,n);

   //  Compute R = AB on host
   Elapsed();
   AxBh(Rh,Ah,Bh,n);
   float Th = Elapsed();

   //  Compute C = AB on device
   Elapsed();
   AxBd(Ch,Ah,Bh,Bw,Bn);
   float Td = Elapsed();

   //  Compute difference between R and C
   double r2=0;
   for (int i=0;i<n*n;i++)
      r2 += fabs(Ch[i]-Rh[i]);
   r2 /= n*n;

   //  Free host memory
   free(Ah);
   free(Bh);
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
