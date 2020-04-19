#include "pulp.h"
#include "conv_kernel.h"
#include "data.h"
#include <stdio.h>

#define NB_ITER 1

void check_function (int *errors, rt_perf_t *perf);
void __attribute__ ((noinline))  InitData         (uint8_t * __restrict__ Img,    int size);
void __attribute__ ((noinline))  InitZero         (uint8_t * __restrict__ Img,    int size);
void __attribute__ ((noinline))  InitKernel       (uint8_t * __restrict__ Kernel, int size);
int  __attribute__ ((noinline))  checkresult      (uint8_t * __restrict__ Out, uint8_t * __restrict__ OutGold, int N);

int main()
{
  int errors = 0;
  rt_perf_t perf;
  for(int i=0; i<NB_ITER; i++) {
    check_function(&errors, &perf);
  }
  synch_barrier();

  if(get_core_id() == 0) {
    printf("errors=%d\n", errors);
    printf("cycles=%d\n", rt_perf_read(RT_PERF_CYCLES));
    return errors;
  }
}

static uint8_t  __attribute__ ((section(".heapsram")))  Out[IMG_DIM];
static uint8_t  __attribute__ ((section(".heapsram")))  In[IMG_DIM];
static uint8_t  __attribute__ ((section(".heapsram")))  Kernel[FILT_DIM];

void check_function(int *errors, rt_perf_t *perf) {

  // start benchmark
#ifdef CLUSTER
  unsigned int core_id = get_core_id();
  unsigned int num_cores = get_core_num();
#else
  unsigned int core_id = 0;
  unsigned int num_cores = 1;
#endif
  unsigned int chunk;
  unsigned int lb, ub;

  // number of rows each core has to convolve
  chunk = IMG_ROW / num_cores;
  // lower bound
  lb = 0;
  // upper bound
  ub = lb + chunk;

  if(core_id == 0)
    lb+=1; //core0 does not compute first row (black board)
  if(core_id == num_cores-1)
    ub-=1; //last core does not compute last row (black board)

  synch_barrier();

  if(core_id == 0){
    printf("2D Convolution WINDOW=%d, DATA FORMAT Q%d.%d\n",FILT_WIN,8-FRACTIONARY_BITS,FRACTIONARY_BITS);
    InitKernel(Kernel,FILT_WIN);
    InitData(In, IMG_DIM);
    InitZero(Out, IMG_DIM);
  }

  synch_barrier();

  rt_perf_reset(perf);
  rt_perf_start(perf);
  ConvKxK_Naive(In, Out, IMG_ROW, lb, ub, IMG_COL, Kernel, 3);
  synch_barrier();
  rt_perf_stop(perf);
  if(core_id == 0){
    *errors = checkresult(Out, Gold_Out_Img, IMG_DIM);
  }
  synch_barrier();
}

// load kernel
void __attribute__ ((noinline)) InitKernel(uint8_t * __restrict__ Kernel, int size)
{
  int i;
  int n = size*size;
  for (i=0; i < n; i++) {
      Kernel[i] = Filter_Kern[i];
  }
}

// load input img
void __attribute__ ((noinline)) InitData(uint8_t * __restrict__ Img, int size)
{
  int i;

  for (i=0; i < size; i++) 
      Img[i] = In_Img[i];

}

// load initialize out to 0
void __attribute__ ((noinline)) InitZero(uint8_t * __restrict__ Img, int size)
{
  int i;

  for (i=0; i < size; i++) 
      Img[i] = 0;

}

int  __attribute__ ((noinline)) checkresult(uint8_t * __restrict__ Out, uint8_t * __restrict__ OutGold, int N)
{
  int i;
  int err = 0;

  for (i = 0; i<N; i++) {
    if (Out[i]!=OutGold[i]) {
#ifdef CONV2D_DEBUG
      printf("At index %d: Actual value: %x: Expected: %x\n", i, Out[i],  OutGold[i]);
#endif
      err++;
    }
  }
  return err;
}

void print_image(uint8_t* image, int N, int M) {
  for (int y = 0; y < M; y++) {
    for (int x = 0; x < N; x++)
      printf("%d,", (uint8_t) image[M * y + x]);

    printf("\n");
  }

  printf("\n");
}
