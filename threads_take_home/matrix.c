    #include <stdio.h>
    #include <pthread.h>
    #include <stdlib.h>
    int m,k,n;
     
    int** A;
    int** B;
    int** C;
     
    struct _row_col{
        int i;
        int j;
    };
     
    void *runner(void *param);
    void *multiply(void *param);
     
    int main(int agrc, char* argv[]){
     
        printf("Enter m: ");
        scanf("%d",&m);
        printf("Enter k: ");
        scanf("%d",&k);
        printf("Enter n: ");
        scanf("%d",&n);
    
        A = (int **)malloc(m*sizeof(int *));
        for (int i = 0; i < m; i++)
        {
            A[i] = (int *)malloc(k*sizeof(int));
        }
        B = (int **)malloc(k*sizeof(int *));
        for (int i = 0; i < k; i++)
        {
            B[i] = (int *)malloc(n*sizeof(int));
        }
        C = (int **)malloc(m*sizeof(int *));
        for (int i = 0; i < m; i++)
        {
            C[i] = (int *)malloc(n*sizeof(int));
        }
        printf("Enter matrix A: \n");
        for(int i=0;i<m;i++){
            for(int j=0;j<k;j++){
                scanf("%d",&A[i][j]);
            }
        }
        printf("Enter matrix B: \n");
        for(int i=0;i<k;i++){
            for(int j=0;j<n;j++){
                scanf("%d",&B[i][j]);
            }
        }
        //create subthread
        pthread_t tid_solve;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid_solve,&attr,runner,0);
        pthread_join(tid_solve,NULL);
        //output the C matrix 
        printf("AxB = \n");
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                printf("%d ",C[i][j]);
            }
            printf("\n");
        }

        for (int i = 0; i < m; i++)
        {
            free(C[i]);
            free(A[i]);
        }
        free(C);
        free(A);
        for (int i = 0; i < k; i++)
        {
            free(B[i]);
        }
        free(B);
        
        return 0;
    }
     
    void *runner(void *param){
     
        pthread_t tid_c[m][n];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                struct _row_col *arg = malloc(sizeof(struct _row_col));
                arg->i = i;
                arg->j = j;
                pthread_create(&tid_c[i][j],&attr,multiply,arg);
            }
        }
     
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                pthread_join(tid_c[i][j],(void *)&C[i][j]);
            }
        }
        pthread_exit(0);
    }
     
    void *multiply(void *param){
        struct _row_col *arg = (struct _row_col *) param; 
     
        int i = arg->i,j=arg->j;
        int result = 0;
        for(int t=0;t<k;t++){
            result+= A[i][t]*B[t][j];
        }
        free(arg);
        pthread_exit((void *)result);
    }