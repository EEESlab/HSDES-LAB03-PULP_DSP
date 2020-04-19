#include <stdio.h>
#include <stdint.h>

#define FRACTIONARY_BITS 7
#define ROUNDBIT   (1 << (FRACTIONARY_BITS -1))
#define SATURATION 255

void __attribute__ ((noinline)) ConvKxK_Naive  (uint8_t * In_Img, uint8_t * Out_Img, int R, int lb, int ub, int C, uint8_t  * Kernel, int K)
{
  int r, c, k, i, j, w, t;
  uint8_t coeff;
  uint8_t data;
  int S;

  //image board is black
  for (r=lb; r < ub; r++) {
    for (c=K/2; c < C-K/2; c++) {

        S = 0;
        t = r*R + c;
        //move in the window
        /* Coordinate window
            (-1;-1) (-1;0) (-1;+1)
            ( 0;-1) ( 0;0) ( 0;+1)
            (+1;-1) (+1;0) (+1;+1)
        */
        for (i = -K/2; i <= K/2; i++) {
            for (j = -K/2; j <= K/2; j++) {
                k = (r+i)*R + (c+j); //coeff for one dimension matrix
                data = In_Img[k];
                w = (i+1)*K + (j+1);
                coeff = Kernel[w];
                S = S + (int)(coeff*data);
            }
        }

        // Rounding
        // S = S + ROUNDBIT;
        // Normalization: Data are Q2.2*(FRACTIONARY_BITS-1), now Q2.FRACTIONARY_BITS-1
        S = S >> FRACTIONARY_BITS;
        // Saturation
        S = S > SATURATION ? SATURATION : S;
        S = S <          0 ?          0 : S;

        Out_Img[t] = (uint8_t)(S);
    }
  }
}
