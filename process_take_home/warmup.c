#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int total_levels = 0;
int current_level = 0;

void execute_child(int signum)
{
    // printf("[%d] Signal recieved from [%d] now executing.\n",getpid(),getppid());
}

int create_children(int child_num)
{
    if (current_level == total_levels)
    {
        return -1;
    }
    int pid_child[child_num];
    int k = ((child_num - 1) * child_num) / 2;
    for (int i = 1; i <= child_num; i++)
    {
        int pid_2 = fork();
        if (pid_2 == -1)
        {
            perror("Fork failed: ");
            return -2;
        }
        else if (pid_2 == 0)
        {
            current_level++;
            pause();
            // printf("[%d] is executing with parent [%d]\n",getpid(),getppid());
            int ret = create_children(i + k);
            printf("[%d] Process finished. Parent [%d]\n", getpid(), getppid());
            exit(1);
        }
        else
        {
            pid_child[i - 1] = pid_2;
            // printf("[%d] Created child process - [%d]\n",getpid(),pid_2);
        }
    }
    // printf("[%d] Process finished. Sending signal to children.\n\n",getpid());
    for (int i = 0; i < child_num; i++)
    {
        sleep(current_level);
        if (kill(pid_child[i], SIGUSR1) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }
    while (wait(NULL) != -1)
        ;
    return 0;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Usage: filename <arg1> <arg2>\n");
        return 1;
    }
    printf("[%d] Init.\n", getpid());
    int initial_lvl_child = atoi(argv[1]);
    int height_tree = atoi(argv[2]);
    total_levels = height_tree;
    current_level = 0;
    int pid_child[initial_lvl_child];

    signal(SIGUSR1, execute_child);

    for (int i = 1; i <= initial_lvl_child; i++)
    {
        int pid_1 = fork();
        if (pid_1 == -1)
        {
            perror("Fork failed: ");
            exit(-1);
        }
        else if (pid_1 == 0)
        {
            pause();
            current_level++;
            // printf("[%d] is executing with parent [%d]\n",getpid(),getppid());
            int ret = create_children(i);
            printf("[%d] Process finished. Parent [%d]\n", getpid(), getppid());
            exit(1);
        }
        else
        {
            pid_child[i - 1] = pid_1;
            // printf("[%d] Created child process - [%d]\n",getpid(),pid_1);
        }
    }
    // printf("[%d] Process finished. Sending signal to children.\n\n",getpid());

    for (int i = 0; i < initial_lvl_child; i++)
    {
        sleep(1);
        if (kill(pid_child[i], SIGUSR1) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }

    while (wait(NULL) != -1)
        ;
    printf("[%d] Process finished.\n", getpid());
    return 0;
}