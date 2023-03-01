#ifndef __MATRIX_MULTIPLICATION_
#define __MATRIX_MULTIPLICATION_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

extern int m, n, k;
extern int A[1000][1000];
extern int B[1000][1000];
extern int C[1000][1000];
struct _row_col
{
    int i;
    int j;
};

void *runner(void *param);
void *multiply(void *param);

#endif