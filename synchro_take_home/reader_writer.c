#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

int n,m,x,y,front,rear;
int read_count = 0;
sem_t modify_readcount;
sem_t read_operation;
sem_t write_operation;
sem_t queue;

struct rw_queue{
    int tid; //store thread id
    char type; // check if it is reader = 0 writer = 1 
    int overridden; //no. of times someone swapped it 
};

struct rw_queue* ready_queue;

void* reader(void* params){

    printf("[%d] Reader arrived.\n",gettid());
    //insert the thread into the ready queue
    sem_wait(&queue);

    ready_queue[rear].tid = gettid();
    ready_queue[rear].type = 'r';
    ready_queue[rear].overridden = 0;
    rear++;

    int n = rear-1;
    while (front<n  && ready_queue[n-1].type=='w' && ready_queue[n-1].overridden<x)
    {
        ready_queue[n-1].overridden+=1;
        struct rw_queue tmp = ready_queue[n];
        ready_queue[n] = ready_queue[n-1];
        ready_queue[n-1] =tmp;
        n--;
    }
    sem_post(&queue);
    
    //execute if the thread reaches the front on the queue 
    sem_wait(&queue);
    while (ready_queue[front].tid!=gettid())
    {
        sem_post(&queue);

        sem_wait(&queue);
    }
    sem_post(&queue);

    sem_wait(&read_operation);

    //increment the front to let the next thread to execute
    sem_wait(&queue);
    front++;
    sem_post(&queue);
    //modify readcount 
    sem_wait(&modify_readcount);
    read_count++;
    if (read_count==1)
    {
        sem_wait(&write_operation);
    }
    sem_post(&modify_readcount);
    
    //perform read_operation operation
    printf("[%d] In the critical section. Reading...\n",gettid());

    sleep(rand()%5+1);
    sem_post(&read_operation);
    
    //decrease the readcount 
    sem_wait(&modify_readcount);
    read_count--;
    if (read_count==0)
    {
        sem_post(&write_operation);
    }
    sem_post(&modify_readcount);
    printf("[%d] Reader finished.\n",gettid());

    pthread_exit(NULL);
    
}
void* writer(void* params){

    printf("[%d] Writer arrived\n",gettid()); 
    sem_wait(&queue);

    ready_queue[rear].tid = gettid();
    ready_queue[rear].type = 'w';
    ready_queue[rear].overridden = 0;
    rear++;

    sem_post(&queue);

    //execute if the thread reaches the front on the queue 
    sem_wait(&queue);
    while (ready_queue[front].tid!=gettid())
    {
        sem_post(&queue);

        sem_wait(&queue);
    }
    
    sem_post(&queue);
    //get the lock
    sem_wait(&write_operation);

    //increment the front to let the next thread to execute
    sem_wait(&queue);
    front++;
    sem_post(&queue);
    //execute write in critical section
    printf("[%d] In the critical section. Writing...\n",gettid());
    sleep(rand()%5+1);

    //release the lock 
    sem_post(&write_operation);
    printf("[%d] Writer finished.\n",gettid());
    pthread_exit(NULL);
}
void* reader_maker(void* params){
    pthread_t reader_tid[n];
    
    for (int i = 0; i < n; i++)
    {
        pthread_create(&reader_tid[i],NULL,reader,NULL);
        sleep(rand()%3+1);
    }
    
    for (int i = 0; i < n; i++)
    {
        pthread_join(reader_tid[i],NULL);
    }
    pthread_exit(0);
}
void* writer_maker(void* params){
    pthread_t writer_tid[m];
    for (int i = 0; i < m; i++)
    {
        pthread_create(&writer_tid[i],NULL,writer,NULL);
        sleep(rand()%3+1);
    }
    for (int i = 0; i < m; i++)
    {
        pthread_join(writer_tid[i],NULL);
    }
    pthread_exit(0);

}
int main(int argv, char* argc[]){

    // scanf("%d %d %d %d",&n,&m,&x,&y);
    if(argv!=5){
        printf("Usage: %s n m x y\n",argc[0]);
        return -1;
    }

    n = atoi(argc[1]);
    m = atoi(argc[2]);
    x = atoi(argc[3]);
    y = atoi(argc[4]);
    front =0;
    rear =0;
    srand(time(0));

    ready_queue = (struct rw_queue*)malloc((n+m)*sizeof(struct rw_queue));

    //initialise semaphores 
    sem_init(&modify_readcount,0,1);
    sem_init(&read_operation,0,y);
    sem_init(&write_operation,0,1);
    sem_init(&queue,0,1);

    //threads to create reader and writer at random interval    
    pthread_t reader_tid,writer_tid;
    pthread_create(&reader_tid,NULL,reader_maker,NULL);
    pthread_create(&writer_tid,NULL,writer_maker,NULL);
    pthread_join(reader_tid,NULL);
    pthread_join(writer_tid,NULL);
    
    //uncomment to print the ready queue
    // for (int i = 0; i < n+m; i++)
    // {
    //     printf("%c%d ",ready_queue[i].type,ready_queue[i].overridden);
    // }
    // printf("\n");

    free(ready_queue);
    sem_destroy(&modify_readcount);
    sem_destroy(&read_operation);
    sem_destroy(&write_operation);
    return 0;
}