#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int N;

int create_path(int i, int j, int matrix[][N])
{
    int n = N;
    if (i >= n || j >= n)
        return 0;
    if (matrix[i][j] == 1)
    {
        return 0;
    }
    if (matrix[i][j] == 0 && i == n - 1 && j == n - 1)
    {
        printf("(%d,%d) -> ", i, j);
        return 1;
    }
    int down;
    int left;
    pid_t down_child = fork();
    if (!down_child)
    {
        int s = create_path(i + 1, j, matrix);
        exit(s);
    }
    pid_t left_child = fork();
    if (!left_child)
    {
        int s = create_path(i, j + 1, matrix);
        exit(s);
    }
    waitpid(down_child, &down, 0);
    waitpid(left_child, &left, 0);
    left /= 256;
    down /= 256;

    int ret = down || left;
    if (ret == 1)
    {
        printf("(%d,%d) -> ", i, j);
        return 1;
    }
    else
        return 0;
    return 0;
}

int main(int argc, char *argv[])
{
    int n;
    printf("Enter dimension : ");
    scanf("%d", &n);
    N = n;

    int matrix[n][n];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &matrix[i][j]);
        }
    }
    if (create_path(0, 0, matrix))
    {
        printf("(%d,%d)\nMJ successfully rescued.\n", 0, 0);
    }
    else
    {
        printf("Spiderman cannot rescue MJ.\n");
    }
}