# Unrolling a MatMul Functions

This example code includes the function _matmul_int8_ that computes the multiplication betweent two _int8_ 2D arrays *A* and *B*.
The result of the matrix multiplication is checked against the provided golden sample data. 

## Getting Started
To get started with the example, simply:
~~~~~shell
cd matmul_vect/
make clean all run
~~~~~

## Try on your own
1) Optimize the matmul function by means of vectorial SIMD instructions. 
Hint: may beusefull to transpose the second operator.
2) Try to further optimize the operation by means of loop unrolling.
Hint: pay attention to the left-over.

Optional 3) Cast the matrix A to _short int_ and design an optimized matrix multiplication function.


### Update 
The optimized vectorized MatMul can be find on _test2.c_.
