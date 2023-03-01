#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>

sem_t modify_seats;
sem_t barber_semaphore;
sem_t customers_semaphore;
sem_t serve_customer;
sem_t queue;

void* barber(void* args);
void* customer(void* args);
void* staff_maker(void* args);
void* student_maker(void* args);

int available_seats=0;
int customers_left = 0;
int n1 = 0;
int n2 = 0;
int y =0;
int front=0,rear=0;
int barbers=0;

struct queue_info{
    int tid;
    int type; //1 for student 2 for staff
    int overtake; //no. of students overtook
};

struct queue_info* ready_queue;

int main(int argv,char* argc[]){

    if(argv!=6){
        printf("Usage: %s <waiting capacity> <staff number> <student number> <students allowed to overtake> <barbers>\n",argc[0]);
        return -1;
    }
    int n = atoi(argc[1]);
    n1 = atoi(argc[2]);
    n2 = atoi(argc[3]);
    y = atoi(argc[4]);
    barbers = atoi(argc[5]);

    available_seats = n;
    srand(time(0));

    ready_queue = (struct queue_info*)malloc((n1+n2)*sizeof(struct queue_info));

    sem_init(&barber_semaphore,0,0);
    sem_init(&customers_semaphore,0,0);
    sem_init(&modify_seats,0,1);
    sem_init(&serve_customer,0,1);
    sem_init(&queue,0,1);

    //barber thread
    pthread_t barber_t;
    pthread_create(&barber_t,NULL,barber,NULL);   

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    //create students and staff
    pthread_t student_tid,staff_tid;
    pthread_create(&student_tid,&attr,student_maker,NULL);
    pthread_create(&staff_tid,&attr,staff_maker,NULL);
        
    pthread_join(barber_t,NULL);

    for (int i = 0; i < n1+n2; i++)
    {
        printf("(%d,%d) ",ready_queue[i].type,ready_queue[i].overtake);
    }
    printf("\n");

    free(ready_queue);
    sem_destroy(&barber_semaphore);
    sem_destroy(&customers_semaphore);
    sem_destroy(&modify_seats);
    sem_destroy(&serve_customer);
    sem_destroy(&queue);
    return 0;
}

void* student_maker(void* args){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    for (int i = 0; i < n2; i++)
    {
        sleep(rand()%5+1);
        pthread_t customer_tid;
        pthread_create(&customer_tid,&attr,customer,1);

    }
}

void* staff_maker(void* args){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    for (int i = 0; i < n1; i++)
    {
        sleep(rand()%5+1);
        pthread_t customer_tid;
        pthread_create(&customer_tid,&attr,customer,2);

    }
}

void* barber(void* args){
    int counter = 0;
    while (1)
    {
        //check if there are any customers else go to sleep
        sem_wait(&customers_semaphore);

        sem_wait(&modify_seats);
        available_seats++;
        sem_post(&modify_seats);

        sem_post(&barber_semaphore);
        
        sem_wait(&serve_customer);
        sleep(rand()%5+5);
        sem_post(&serve_customer);

        printf("Customer was served.\n");
        counter++;

        if(counter+customers_left==n1+n2){
            break;
        }
    }
    pthread_exit(NULL);
    
}

void* customer(void* args){

    int type = args;
    sem_wait(&queue);
    
    ready_queue[rear].tid = gettid();
    ready_queue[rear].type = type;
    ready_queue[rear].overtake = 0;
    rear++;

    if (type==1)
    {
        int n = rear-1;
        while (front<n && ready_queue[n-1].type==2 && ready_queue[n-1].overtake<y)
        {
            ready_queue[n-1].overtake+=1;
            struct queue_info temp = ready_queue[n];
            ready_queue[n] = ready_queue[n-1];
            ready_queue[n-1] = temp;
            printf("[%d] Swapped with a staff.\n",gettid());
            n--;
        }
        
    }
    sem_post(&queue);

    sem_wait(&queue);
    while (ready_queue[front].tid!=gettid())
    {
        sem_post(&queue);
        sem_wait(&queue);
    }
    sem_post(&queue);

    //check the no. of seats 
    sem_wait(&modify_seats);

    

    if(available_seats>=1){
        available_seats--;

        printf("[%d] %s is waiting.\n",gettid(),type==1?"Student":"Staff");
        //wakeup the barber if it is sleeping
        sem_post(&customers_semaphore);
        sem_post(&modify_seats);
        //wait for barber
        sem_wait(&barber_semaphore);
        sem_wait(&queue);
        front++;
        sem_post(&queue);
        pthread_exit(NULL);
    }
    else{
        sem_post(&modify_seats);
        customers_left++;
        printf("[%d] %s left.\n",gettid(),type==1?"Student":"Staff");
    }

    pthread_exit(NULL);
}