    #include <stdio.h>
    #include <pthread.h>
    #include <stdlib.h>
     
    int N,M;
    int *A,*B,*result;
    struct to_pass{
        int i;
        int arr;
    };
    void* get_input(void* param);
    void* find_position(void* param);
    void* print_result(void* param);
     
    int main(int argc,char *argv[]){
     
        //create thread to get input
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid,&attr,get_input,NULL);
        pthread_join(tid,NULL);
     
        //create N+M threads to merge the arrays
        pthread_t thread_list[N+M];
        for(int i=0;i<N+M;i++){
            struct to_pass *arg = malloc(sizeof(struct to_pass));
            
            if(i<N){
                arg->i = i;
                arg->arr = 1;
            }
            else{
                arg->i = i-N;
                arg->arr = 2;
            
            }
            pthread_create(&thread_list[i],&attr,find_position,arg);
        }
        for(int i=0;i<N+M;i++){
            pthread_join(thread_list[i],NULL);
        }
     
        //thread to print the result array
        pthread_t tid_result;
        pthread_create(&tid_result,&attr,print_result,NULL);
        pthread_join(tid_result,NULL);
        
        //free the allocated memory
        free(A);
        free(B);
        free(result);
        return 0;
    }
     
    void* get_input(void* param){
        
        printf("Enter N: ");
        scanf("%d",&N);
     
        printf("Enter M: ");
        scanf("%d",&M);
     
        A = (int *)malloc(N*sizeof(int));
        B = (int *)malloc(M*sizeof(int));
        result = (int *)malloc((N+M)*sizeof(int));
        
        printf("Array A:\n");
        for(int i=0;i<N;i++){
            scanf("%d",&A[i]);
        }
        
        printf("Array B:\n");
        for(int i=0;i<M;i++){
            scanf("%d",&B[i]);
        }
        
        pthread_exit(0);
    }
     
    void* find_position(void* param){
        struct to_pass *arg = (struct to_pass *)param;
        int i = arg->i;
        int *a,*b;
        if(arg->arr==1){
            a =A;
            b = B;
        }
        else{
            a=B;
            b=A;
        }
        free(arg);
        if(a[i]<b[0]){
            result[i] = a[i];
            pthread_exit(0);
        }
        else{
            int l = 0;
            int r = arg->arr==1?M:N;
            while(l<r){
                int mid = (l+r)/2;
                if(a[i]<=b[mid]){
                    r = mid;
                }
                else{
                    l = mid+1;
                }
            }
     
            result[i+l] = a[i];
            pthread_exit(0);
        }
    }
     
    void* print_result(void* param){
        printf("Result : ");
        for(int i=0;i<N+M;i++){
            printf("%d ",result[i]);
        }
        printf("\n");
     
        pthread_exit(0);
    }