/* 
 * Copyright (C) 2021 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: 
 *  Manuele Rusci, UniBO (manuele.rusci@unibo.it),
 */

#include "pmsis.h"
#include "data.h" //include defines of data

// global variables for 32 bit data
int A[N] = DATA_A;  
int B[N] = DATA_B;

// global variables for 16 bit data
short int A_short[N] = DATA_A;  
short int B_short[N] = DATA_B; 

// global variables for 8 bit data
char A_byte[N] = DATA_A;  
char B_byte[N] = DATA_B;



int main()
{
  printf("***********************\n");
  printf("  FIR FILTER EXAMPLES  \n");
  printf("***********************\n");

  // define variables
  uint32_t instr_cnt,cycles_cnt;
  float cpi, mac_cyc, instr_mac;
  int C;  // accumulator

  // number of MAC
  int mac = N;


  //initialize the performance counters
  pi_perf_conf(
      1 << PI_PERF_CYCLES | 
      1 << PI_PERF_INSTR   
  );

/******************************************
         FIR with 32 bit variables
******************************************/
  printf("\n*** FIR with 32 variables (no vect) ***\n");
  pi_perf_reset();
  pi_perf_start();

  //********  task to profile ***************
  C = 0;
  for(int i=0;i<N; i++){
    C =__builtin_pulp_mac(A[i], B[i], C);
//    C += A[i] * B[i]; // uncomment and check the performance
  }
  //*****************************************

  pi_perf_stop(); // stop the performance counters

  instr_cnt = pi_perf_read(PI_PERF_INSTR);
  cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
  cpi = (float) cycles_cnt / instr_cnt;
  mac_cyc = (float) mac / cycles_cnt;
  instr_mac = (float) instr_cnt / mac;

  printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\nInstr/MAC: %f\n", 
      instr_cnt, cycles_cnt, cpi, mac_cyc, instr_mac);
  if (C == DATA_C)
    printf("Checksum: %d OK!\n", C);
  else
    printf("Checksum is wrong!", C);


/******************************************
       FIR with 16 bit variables
******************************************/
  printf("\n\n*** FIR with 16 variables (no vect) ***\n");
  pi_perf_reset();
  pi_perf_start();

  //********  task to profile ***************
  C = 0;
  for(int i=0;i<N; i++){
    C += A_short[i] * B_short[i];
  }
  //*****************************************

  pi_perf_stop(); // stop the performance counters

  instr_cnt = pi_perf_read(PI_PERF_INSTR);
  cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
  cpi = (float) cycles_cnt / instr_cnt;
  mac_cyc = (float) mac / cycles_cnt;
  instr_mac = (float) instr_cnt / mac;

  printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\nInstr/MAC: %f\n", 
      instr_cnt, cycles_cnt, cpi, mac_cyc, instr_mac);
  //print the checksum
  if (C == DATA_C)
    printf("Checksum: %d OK!\n", C);
  else
    printf("Checksum is wrong!", C);


/******************************************
       FIR with 16 bit variables with vectorization
******************************************/
  printf("\n\n*** FIR with 16 variables (v2s vectors) ***\n");
  pi_perf_reset();
  pi_perf_start();

  //********  task to profile ***************
  v2s * A_vect16 = (v2s *) A_short;
  v2s * B_vect16 = (v2s *) B_short;
  C = 0;
  for(int i=0;i<(N>>1); i++){
    C = __builtin_pulp_sdotsp2(A_vect16[i], B_vect16[i], C);
  }
  //*****************************************

  pi_perf_stop(); // stop the performance counters

  instr_cnt = pi_perf_read(PI_PERF_INSTR);
  cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
  cpi = (float) cycles_cnt / instr_cnt;
  mac_cyc = (float) mac / cycles_cnt;
  instr_mac = (float) instr_cnt / mac;

  printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\nInstr/MAC: %f\n", 
      instr_cnt, cycles_cnt, cpi, mac_cyc, instr_mac);
  //print the checksum
  if (C == DATA_C)
    printf("Checksum: %d OK!\n", C);
  else
    printf("Checksum is wrong!", C);



/******************************************
       FIR with 8 bit variables
******************************************/
  printf("\n\n*** FIR with 8 variables (no vect) ***\n");
  pi_perf_reset();
  pi_perf_start();

  //********  task to profile ***************
  C = 0;
  for(int i=0;i<N; i++){
    C += A_byte[i] * B_byte[i];
  }
  //*****************************************

  pi_perf_stop(); // stop the performance counters

  instr_cnt = pi_perf_read(PI_PERF_INSTR);
  cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
  cpi = (float) cycles_cnt / instr_cnt;
  mac_cyc = (float) mac / cycles_cnt;
  instr_mac = (float) instr_cnt / mac;

  printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\nInstr/MAC: %f\n", 
      instr_cnt, cycles_cnt, cpi, mac_cyc, instr_mac);
  //print the checksum
  if (C == DATA_C)
    printf("Checksum: %d OK!\n", C);
  else
    printf("Checksum is wrong!", C);


/******************************************
       FIR with 16 bit variables with vectorization
******************************************/
  printf("\n\n*** FIR with 8 variables (v4s vectors) ***\n");
  pi_perf_reset();
  pi_perf_start();

  //********  task to profile ***************
  v4s * A_vect8 = (v4s *) A_byte;
  v4s * B_vect8 = (v4s *) B_byte;
  C = 0;
  for(int i=0;i<(N>>2); i++){
    C = __builtin_pulp_sdotsp4(A_vect8[i], B_vect8[i], C);
  }
  //*****************************************

  pi_perf_stop(); // stop the performance counters

  instr_cnt = pi_perf_read(PI_PERF_INSTR);
  cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
  cpi = (float) cycles_cnt / instr_cnt;
  mac_cyc = (float) mac / cycles_cnt;
  instr_mac = (float) instr_cnt / mac;

  printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\nMAC/cyc: %f\nInstr/MAC: %f\n", 
      instr_cnt, cycles_cnt, cpi, mac_cyc, instr_mac);
  //print the checksum
  if (C == DATA_C)
    printf("Checksum: %d OK!\n", C);
  else
    printf("Checksum is wrong!", C);

  pmsis_exit(0);
}
