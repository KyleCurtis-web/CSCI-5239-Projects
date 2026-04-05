#include "CSCIx239.h"
#include "InitGPUcu.h"

//
//  Initialize fastest GPU device
//
int InitGPU(int verbose)
{
   //  Get number of CUDA devices
   int num;
   if (cudaGetDeviceCount(&num)) Fatal("Cannot get number of CUDA devices\n");
   if (num<1) Fatal("No CUDA devices found\n");

   //  Get fastest device
   cudaDeviceProp prop;
   int   MaxDevice = -1;
   int   MaxGflops = -1;
   for (int dev=0;dev<num;dev++)
   {
      if (cudaGetDeviceProperties(&prop,dev)) Fatal("Error getting device %d properties\n",dev);
      int Gflops = prop.multiProcessorCount * prop.clockRate;
      if (verbose) printf("CUDA Device %d: %s Gflops %f Processors %d Threads/Block %d\n",dev,prop.name,1e-6*Gflops,prop.multiProcessorCount,prop.maxThreadsPerBlock);
      if(Gflops > MaxGflops)
      {
         MaxGflops = Gflops;
         MaxDevice = dev;
      }
   }

   //  Print and set device
   if (cudaGetDeviceProperties(&prop,MaxDevice)) Fatal("Error getting device %d properties\n",MaxDevice);
   printf("Fastest CUDA Device %d: %s\n",MaxDevice,prop.name);
   cudaSetDevice(MaxDevice);

   //  Return max thread count
   return prop.maxThreadsPerBlock;
}
