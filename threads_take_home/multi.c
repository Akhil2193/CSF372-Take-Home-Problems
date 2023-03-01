    #include <stdio.h>
    #include <pthread.h>
    #include <stdlib.h>
    int m;
    int n;
    int n1[10];
    int n2[10];
     
    void *solve(void *param);
     
    int main(int argc, char *argv[]){
        
        int x,y;
        printf("Enter 1st number: ");
        scanf("%d",&x);
        printf("Enter 2nd number: ");
        scanf("%d",&y);
     
        while(x>0){
            n1[m]=x%10;
            x/=10;
            m++;
        }
        while(y>0){
            n2[n] = y%10;
            y/=10;
            n++;
        }
        
        int k = m+n-1;
     
        pthread_t tid[k];
        int result_thread[k];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
     
     
        for(int i=0;i<k;i++){
            pthread_create(&tid[i],&attr,solve,i);
        }
        for(int i =0;i<k;i++){
            pthread_join(tid[i],&result_thread[i]);
        }
     
     
        int carry =0;
        for(int i=0;i<k;i++){
            result_thread[i]+=carry;
            carry = result_thread[i]/10;
            result_thread[i]%=10;
        }
        if(carry!=0)
            printf("%d",carry);
        for(int i=k-1;i>=0;i--){
            printf("%d",result_thread[i]);
        }
        printf("\n");
        return 0;
    }
     
    void *solve(void *param){
        int i = param;
        int sum = 0;
        for(int t=0;t<n;t++){
            if(i-t<m && i-t>=0){
                sum+= n2[t] * n1[i-t];
            }
        }
        pthread_exit(sum);
    }