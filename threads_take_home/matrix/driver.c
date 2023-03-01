#include <header.h>

int m, k, n;

int A[1000][1000];
int B[1000][1000];
int C[1000][1000];

int main(int agrc, char *argv[])
{

    printf("Enter m: ");
    scanf("%d", &m);
    printf("Enter k: ");
    scanf("%d", &k);
    printf("Enter n: ");
    scanf("%d", &n);

    printf("Enter matrix A: \n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            scanf("%d", &A[i][j]);
        }
    }
    printf("Enter matrix B: \n");
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &B[i][j]);
        }
    }
    // create subthread
    pthread_t tid_solve;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid_solve, &attr, runner, 0);
    pthread_join(tid_solve, NULL);
    // output the C matrix
    printf("AxB = \n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    return 0;
}