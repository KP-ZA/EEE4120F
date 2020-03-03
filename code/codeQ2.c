#include "main.h"
//------------------------------------------------------------------------------

// GPU reset?  See <http://stackoverflow.com/questions/12259044/
//                  limitations-of-work-item-load-in-gpu-cuda-opencl>

// <https://social.technet.microsoft.com/Forums/windows/en-US/
// 92a45329-3dd1-4c42-8a53-42dd232edd81/
// how-to-turn-off-timeout-detection-and-recovery-of-gpus>
//------------------------------------------------------------------------------

void Process_Serial(){
 int i, j, k;
 float result;

 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   result = 0;
   for(k = 0; k < N; k++){
    result += A[N*i + k] * B[N*k + j];
   }
   Output_Serial[N*j + i] = result;
  }
 }
}
//------------------------------------------------------------------------------
//******************************************************************************
void Process_OpenCL(){

 printf("\n");
 printf("Matrix is an %dx%d matrix\n", N, N);
 OpenCL_ConstantInt(3, N);
 tic();
 OpenCL_WriteData(A_Buffer, N*N*sizeof(float), A);
 double A_Buff = toc()/1e-3;
 printf("A_Buffer: %lg ms\n", A_Buff);

 tic();
 OpenCL_WriteData(B_Buffer, N*N*sizeof(float), B);
 double B_Buff = toc()/1e-3;
 printf("B_Buffer: %lg ms\n", B_Buff);

 tic();
 OpenCL_Run(N, LocalSize);
 double run_t = toc()/1e-3;
 printf("Run: %lg ms\n", run_t);

 tic();
 OpenCL_ReadData(OutputBuffer, N*N*sizeof(float), Output_OpenCL);
 double read_data = toc()/1e-3;
 printf("Read: %lg ms\n", read_data);
 double total = A_Buff + B_Buff + run_t + read_data;

 double speed_up = total/run_t;
 printf("The speed up time is: %lg", speed_up);
}
//------------------------------------------------------------------------------

void Compare(){
 int i, j;

 printf("A:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", A[N*j + i]);
  }
  printf("\n");
 }

 printf("B:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", B[N*j + i]);
  }
  printf("\n");
 }

 printf("Output_Serial:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", Output_Serial[N*j + i]);
  }
  printf("\n");
 }

 printf("Output_OpenCL:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", Output_OpenCL[N*j + i]);
  }
  printf("\n");
 }
}
//------------------------------------------------------------------------------

void Fill(float* A){
 int i, j;

 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   A[N*j + i] = rand() % 20;
  }
 }
}
//------------------------------------------------------------------------------

int main(){
 // Initialise OpenCL
 if(
  !OpenCL_Init("NVIDIA"                ) && // nVidia
  !OpenCL_Init("Advanced Micro Devices") && // AMD
  !OpenCL_Init(0                       )    // Default
 ){
  printf("Error: Cannot initialise OpenCL.\n");
  return 1;
 }

 // Load a kernel
 if(!OpenCL_LoadKernel("OpenCL/Kernel.cl", "Multiply")) return 1;

 N = 3;//number************************************************************
 size_t BufferSize = N*N*sizeof(float);

 // Allocate CPU RAM
 A = (float*)malloc(BufferSize);
 B = (float*)malloc(BufferSize);

 Output_Serial = (float*)malloc(BufferSize);
 Output_OpenCL = (float*)malloc(BufferSize);

 Fill(A);
 Fill(B);

 // Allocate GPU RAM
 OpenCL_PrepareLocalSize(N, LocalSize);
 A_Buffer     = OpenCL_CreateBuffer(0, CL_MEM_READ_ONLY , BufferSize);
 B_Buffer     = OpenCL_CreateBuffer(1, CL_MEM_READ_ONLY , BufferSize);
 OutputBuffer = OpenCL_CreateBuffer(2, CL_MEM_WRITE_ONLY, BufferSize);

 // Process the matrices
 Process_Serial();

 Process_OpenCL();

 // Compare results
 //Compare();*****************************************************************

 // Clean-up
 free(A);
 free(B);
 free(Output_Serial);
 free(Output_OpenCL);

 OpenCL_FreeBuffer(A_Buffer    );
 OpenCL_FreeBuffer(B_Buffer    );
 OpenCL_FreeBuffer(OutputBuffer);
 OpenCL_Destroy();

 return 0;
}
//------------------------------------------------------------------------------
