/* 
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: 
 *  Manuele Rusci, UniBO (manuele.rusci@unibo.it),
 */

#include "pmsis.h"
#include "data.h"


char MatA_INT8[N_MAT*N_MAT] = DATA_A;  
char MatB_INT8[N_MAT*N_MAT] = DATA_B; 
int MatC[N_MAT*N_MAT];  // output array 
int MatC_golden[N_MAT*N_MAT] = DATA_C; // precomputed glden model


void check_result_matmul(int * in1, int * in2, int size){
  int error = 0;
  for (int i = 0; i < (size*size); i++) {
    if (in1[i] != in2[i]) error++;
  } 
  if(error)
    printf("Checksum NOK with a number of errors: %d\n", error);
  else
    printf("Checksum OK!\n");

}

void reset_output_mat(int * in1, int size){
  for (int i = 0; i < (size*size); i++) {
    in1[i] = 0;
  } 
}

void matmul_int8(char * in1, char * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    for (int i = 0; i < MN; i++) {
      for (int j = 0; j < NN; j++) {
        int acc = 0;
        for (int k = 0; k < KN; k++) {
           acc += in1[i*KN+k] * in2[k*NN+j];
        } //k
        out3[i*NN+j] = acc;
      }//j
    }//i
}

int main()
{

    printf("*********************************\n");
    printf("  MATMUL UNROLLING & VECTORIZED  \n");
    printf("*********************************\n");

    // define variables
    uint32_t instr_cnt,cycles_cnt;
    float cpi, mac_cyc;
    int mac = N_MAT * N_MAT * N_MAT;

    pi_perf_conf(
        1 << PI_PERF_CYCLES | 
        1 << PI_PERF_INSTR   
    );

/******************************************
         MatMul no unrolled
******************************************/
    printf("\n\n*** MatMul Baseline ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_stop(); // stop the performance counters
    pi_perf_reset();
    pi_perf_start();

    // task to profile
    matmul_int8(MatA_INT8, MatB_INT8, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);



    pmsis_exit(0);
}
