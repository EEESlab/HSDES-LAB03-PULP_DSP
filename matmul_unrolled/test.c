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


int MatA[N_MAT*N_MAT] = DATA_A;  
int MatB[N_MAT*N_MAT] = DATA_B; 
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


void matmul_int32(int * in1, int * in2, int* out3,int MN, int NN, int KN){
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

void matmul_int32_unroll1x2(int * in1, int * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    for (int i = 0; i < MN; i++) {
      for (int j = 0; j < NN; j=j+2) {
        int acc0 = 0;
        int acc1 = 0;
        for (int k = 0; k < KN; k++) {
           int shared_op = in1[i*KN+k];
           int idx = k*NN+j;
           acc0   += shared_op * in2[idx];
           acc1   += shared_op * in2[idx+1];
        } //k
        out3[i*NN+j] = acc0;
        out3[i*NN+j+1] = acc1;
      }//j
    }//i
}

void matmul_int32_unroll1x4(int * in1, int * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    for (int i = 0; i < MN; i++) {
      for (int j = 0; j < NN; j=j+4) {
        int acc0 = 0;
        int acc1 = 0;
        int acc2 = 0;
        int acc3 = 0;
        for (int k = 0; k < KN; k++) {
           int shared_op = in1[i*KN+k];
           acc0   += shared_op * in2[k*NN+j];
           acc1   += shared_op * in2[k*NN+j+1];
           acc2   += shared_op * in2[k*NN+j+2];
           acc3   += shared_op * in2[k*NN+j+3];
        } //k
        out3[i*NN+j]   = acc0;
        out3[i*NN+j+1] = acc1;
        out3[i*NN+j+2] = acc2;
        out3[i*NN+j+3] = acc3;

      }//j
    }//i
}


void matmul_int32_unroll1x8(int * in1, int * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    for (int i = 0; i < MN; i++) {
      for (int j = 0; j < NN; j=j+8) {
        int acc0 = 0;
        int acc1 = 0;
        int acc2 = 0;
        int acc3 = 0;
        int acc4 = 0;
        int acc5 = 0;
        int acc6 = 0;
        int acc7 = 0;
        for (int k = 0; k < KN; k++) {
           int shared_op = in1[i*KN+k];
           acc0   += shared_op * in2[k*NN+j];
           acc1   += shared_op * in2[k*NN+j+1];
           acc2   += shared_op * in2[k*NN+j+2];
           acc3   += shared_op * in2[k*NN+j+3];
           acc4   += shared_op * in2[k*NN+j+4];
           acc5   += shared_op * in2[k*NN+j+5];
           acc6   += shared_op * in2[k*NN+j+6];
           acc7   += shared_op * in2[k*NN+j+7];
        } //k
        out3[i*NN+j]   = acc0;
        out3[i*NN+j+1] = acc1;
        out3[i*NN+j+2] = acc2;
        out3[i*NN+j+3] = acc3;
        out3[i*NN+j+4] = acc4;
        out3[i*NN+j+5] = acc5;
        out3[i*NN+j+6] = acc6;
        out3[i*NN+j+7] = acc7;
      }//j
    }//i
}

void matmul_int32_unroll2x4(int * in1, int * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    int acc0, acc1, acc2, acc3,
        acc4, acc5, acc6, acc7;

    for (int i = 0; i < MN; i=i+2) {
      for (int j = 0; j < NN; j=j+4) {
        acc0 = acc1 = acc2 = acc3 = 0;
        acc4 = acc5 = acc6 = acc7 = 0;

        for (int k = 0; k < KN; k++) {
           int shared_op = in1[i*KN+k]; 
           char a = in2[k*NN+j];
           char b = in2[k*NN+j+1];
           char c = in2[k*NN+j+2];
           char d = in2[k*NN+j+3];

           acc0   += shared_op * a;
           acc1   += shared_op * b;
           acc2   += shared_op * c;
           acc3   += shared_op * d;

           shared_op = in1[(i+1)*KN+k];
           acc4   += shared_op * a;
           acc5   += shared_op * b;
           acc6   += shared_op * c;
           acc7   += shared_op * d;
        } //k
        out3[i*NN+j]   = acc0;
        out3[i*NN+j+1] = acc1;
        out3[i*NN+j+2] = acc2;
        out3[i*NN+j+3] = acc3;
        out3[(i+1)*NN+j]   = acc4;
        out3[(i+1)*NN+j+1] = acc5;
        out3[(i+1)*NN+j+2] = acc6;
        out3[(i+1)*NN+j+3] = acc7;

      }//j
    }//i
}

void matmul_int32_unroll4x4(int * in1, int * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    int acc0, acc1, acc2, acc3,
        acc4, acc5, acc6, acc7,
        acc8, acc9, acc10, acc11,
        acc12, acc13, acc14, acc15;

    for (int i = 0; i < MN; i=i+4) {
      for (int j = 0; j < NN; j=j+4) {
        acc0 = acc1 = acc2 = acc3 = 0;
        acc4 = acc5 = acc6 = acc7 = 0;
        acc8 = acc9 = acc10 = acc11 = 0;
        acc12 = acc13 = acc14 = acc15 = 0;

        for (int k = 0; k < KN; k++) {
           int shared_op = in1[i*KN+k]; 
           char a = in2[k*NN+j];
           char b = in2[k*NN+j+1];
           char c = in2[k*NN+j+2];
           char d = in2[k*NN+j+3];

           acc0   += shared_op * a;
           acc1   += shared_op * b;
           acc2   += shared_op * c;
           acc3   += shared_op * d;

           shared_op = in1[(i+1)*KN+k];
           acc4   += shared_op * a;
           acc5   += shared_op * b;
           acc6   += shared_op * c;
           acc7   += shared_op * d;

           shared_op = in1[(i+2)*KN+k];
           acc8   += shared_op * a;
           acc9   += shared_op * b;
           acc10  += shared_op * c;
           acc11  += shared_op * d;

           shared_op = in1[(i+3)*KN+k];
           acc12  += shared_op * a;
           acc13  += shared_op * b;
           acc14  += shared_op * c;
           acc15  += shared_op * d;
        } //k
        out3[i*NN+j]   = acc0;
        out3[i*NN+j+1] = acc1;
        out3[i*NN+j+2] = acc2;
        out3[i*NN+j+3] = acc3;
        out3[(i+1)*NN+j]   = acc4;
        out3[(i+1)*NN+j+1] = acc5;
        out3[(i+1)*NN+j+2] = acc6;
        out3[(i+1)*NN+j+3] = acc7;
        out3[(i+2)*NN+j]   = acc8;
        out3[(i+2)*NN+j+1] = acc9;
        out3[(i+2)*NN+j+2] = acc10;
        out3[(i+2)*NN+j+3] = acc11;
        out3[(i+3)*NN+j]   = acc12;
        out3[(i+3)*NN+j+1] = acc13;
        out3[(i+3)*NN+j+2] = acc14;
        out3[(i+3)*NN+j+3] = acc15;

      }//j
    }//i
}

int main()
{

    printf("*****************************\n");
    printf("  MATMUL UNROLLING EXAMPLES  \n");
    printf("*****************************\n");

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
    matmul_int32(MatA, MatB, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);


/******************************************
         MatMul w/ unrolled
******************************************/
    printf("\n\n*** MatMul With Unrolling 1x2 ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_reset();
    pi_perf_start();

    // task to profile
    matmul_int32_unroll1x2(MatA, MatB, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);

/******************************************
         MatMul w/ unrolled 2
******************************************/
    printf("\n\n*** MatMul With Unrolling 1x4 ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_reset();
    pi_perf_start();

    // task to profile
    matmul_int32_unroll1x4(MatA, MatB, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);

/******************************************
         MatMul w/ unrolled 3
******************************************/
    printf("\n\n*** MatMul With Unrolling 1x8 ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_reset();
    pi_perf_start();

    // task to profile
    matmul_int32_unroll1x8(MatA, MatB, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);

/******************************************
         MatMul w/ unrolled 4
******************************************/
    printf("\n\n*** MatMul With Unrolling 2x4 ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_reset();
    pi_perf_start();

    // task to profile
    matmul_int32_unroll2x4(MatA, MatB, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);

/******************************************
         MatMul w/ unrolled 5
******************************************/
    printf("\n\n*** MatMul With Unrolling 4x4 ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_reset();
    pi_perf_start();

    // task to profile
    matmul_int32_unroll4x4(MatA, MatB, MatC, N_MAT, N_MAT, N_MAT);

    pi_perf_stop(); // stop the performance counters

    instr_cnt = pi_perf_read(PI_PERF_INSTR);
    cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    cpi = (float) cycles_cnt / instr_cnt;
    mac_cyc = (float) mac / cycles_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\n", 
        instr_cnt, cycles_cnt, cpi, mac_cyc);
    //print the checksum
    check_result_matmul(MatC, MatC_golden, N_MAT);
/******************************************
    End of Tasks!
******************************************/

    pmsis_exit(0);
}
