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
char MatBt_INT8[N_MAT*N_MAT]; 

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

void mat_transpose_int8(char * mat_in, char * mat_out, int sizeM, int sizeN){
  // task to profile
  for (int i = 0; i < sizeM; i++) {
    for (int j = 0; j < sizeN; j++) {
      mat_out[j*sizeM+i] = mat_in[i*sizeN+j];
    }//j
  }//i
}


void matmul_int8_vect(char * in1, char * in2, int* out3,int MN, int NN, int KN){
    // task to profile
    int vect_loop = KN >>2;
    int remainder_loop = KN & 0x3;
    for (int i = 0; i < MN; i++) {
      for (int j = 0; j < NN; j++) {
        int acc = 0;
        v4s * in1_vect8 = (v4s *) &in1[i*NN];
        v4s * in2_vect8 = (v4s *) &in2[j*MN];
        for (int k = 0; k < vect_loop; k++) {
          acc = __builtin_pulp_sdotsp4(in1_vect8[k], in2_vect8[k], acc);
        }
        // leftover management
        for (int k = KN-remainder_loop; k < KN; k++) {
           acc += in1[i*KN+k] * in2[j*KN+k];
        } //k
        out3[i*NN+j] = acc;
      }//j
    }//i
}


void matmul_int8_vect_unroll1x4(char * in1, char * in2, int* out3, int MN, int NN, int KN){
    // task to profile
    int vect_inloop = KN >>2;
    int remainder_inloop = KN & 0x3;
    int remainder_outloop = NN & 0x3;
    for (int i = 0; i < MN; i++) {
      v4s * in1_vect8   = (v4s *) &in1[i*NN];
      for (int j = 0; j < NN; j=j+4) {
        int acc0 = 0;
        int acc1 = 0;
        int acc2 = 0;
        int acc3 = 0;
        
        v4s * in2_vect8_0 = (v4s *) &in2[j*MN];
        v4s * in2_vect8_1 = (v4s *) &in2[(j+1)*MN];
        v4s * in2_vect8_2 = (v4s *) &in2[(j+2)*MN];
        v4s * in2_vect8_3 = (v4s *) &in2[(j+3)*MN];        

        for (int k = 0; k < vect_inloop; k++) {
           acc0 = __builtin_pulp_sdotsp4(in1_vect8[k], in2_vect8_0[k], acc0);
           acc1 = __builtin_pulp_sdotsp4(in1_vect8[k], in2_vect8_1[k], acc1);
           acc2 = __builtin_pulp_sdotsp4(in1_vect8[k], in2_vect8_2[k], acc2);
           acc3 = __builtin_pulp_sdotsp4(in1_vect8[k], in2_vect8_3[k], acc3);
        }
        // leftover innerloop
        for (int k = KN-remainder_inloop; k < KN; k++) {
           acc0 += in1[i*KN+k] * in2[(j+0)*KN+k];
           acc1 += in1[i*KN+k] * in2[(j+1)*KN+k];
           acc2 += in1[i*KN+k] * in2[(j+2)*KN+k];
           acc3 += in1[i*KN+k] * in2[(j+3)*KN+k];
        } //k
        out3[i*NN+(j+0)] = acc0;
        out3[i*NN+(j+1)] = acc1;
        out3[i*NN+(j+2)] = acc2;
        out3[i*NN+(j+3)] = acc3;
      }//j

      // leftover j-loop
      for (int j = NN-remainder_outloop; j < remainder_outloop; j++) {
        int acc = 0;
        v4s * in2_vect8 = (v4s *) &in2[j*MN];
        for (int k = 0; k < vect_inloop; k++) {
          acc = __builtin_pulp_sdotsp4(in1_vect8[k], in2_vect8[k], acc);
        }
        // leftover management
        for (int k = KN-remainder_inloop; k < KN; k++) {
           acc += in1[i*KN+k] * in2[j*KN+k];
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
    printf("\n\n*** MatMul Vectorized ***\n");

    reset_output_mat(MatC, N_MAT);

    pi_perf_stop(); // stop the performance counters
    pi_perf_reset();
    pi_perf_start();

    // task to profile
    mat_transpose_int8(MatB_INT8, MatBt_INT8, N_MAT, N_MAT);
    matmul_int8_vect(MatA_INT8, MatBt_INT8, MatC, N_MAT, N_MAT, N_MAT);
//    matmul_int8_vect_unroll1x4(MatA_INT8, MatBt_INT8, MatC, N_MAT, N_MAT, N_MAT);

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
