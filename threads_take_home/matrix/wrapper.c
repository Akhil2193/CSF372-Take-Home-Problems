#include <header.h>
void *runner(void *param)
{

    pthread_t tid_c[m][n];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            struct _row_col *arg = malloc(sizeof(struct _row_col));
            arg->i = i;
            arg->j = j;
            pthread_create(&tid_c[i][j], &attr, multiply, arg);
        }
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            pthread_join(tid_c[i][j], (void *)&C[i][j]);
        }
    }
    pthread_exit(0);
}

void *multiply(void *param)
{
    struct _row_col *arg = (struct _row_col *)param;

    int i = arg->i, j = arg->j;
    int result = 0;
    for (int t = 0; t < k; t++)
    {
        result += A[i][t] * B[t][j];
    }
    free(arg);
    pthread_exit((void *)result);
}