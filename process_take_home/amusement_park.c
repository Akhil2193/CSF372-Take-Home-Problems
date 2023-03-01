#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

int ticket_counter = 0;
int ride_queue = 0;
bool not_alarmed = false;
pid_t pid_ticket;
pid_t pid_queue;
pid_t pid_driver;

void alarm_handler(int signum)
{

    if (kill(pid_ticket, SIGKILL) == -1)
    {
        kill(pid_ticket, SIGKILL);
    }
    if (kill(pid_queue, SIGKILL) == -1)
    {
        kill(pid_queue, SIGKILL);
    }
    if (kill(pid_driver, SIGKILL) == -1)
    {
        kill(pid_driver, SIGKILL);
    }

    printf("Simulation Completed!\n");
    not_alarmed = true;
}

void receive_signal_queue(int signum, siginfo_t *siginfo, void *ucontext)
{
    pid_queue = siginfo->si_pid;
    ticket_counter -= siginfo->si_int;
    if (ticket_counter < 0)
        ticket_counter = 0;
}

void receive_people(int signum, siginfo_t *siginfo, void *ucontext)
{
    int people = siginfo->si_int;
    int space = 20 - ride_queue;
    if (space >= people)
    {
        ride_queue += people;
        printf("%d sent to ride queue\n", people);
        sleep(1);
        union sigval sv;
        sv.sival_int = people;
        sigqueue(pid_ticket, SIGUSR1, sv);
    }
    else
    {
        ride_queue = 20;
        if (space != 0)
        {
            printf("%d sent to ride queue\n", space);
        }
        sleep(1);
        union sigval sv;
        sv.sival_int = space;
        sigqueue(pid_ticket, SIGUSR1, sv);
    }
}

void send_people(int signum, siginfo_t *siginfo, void *ucontext)
{
    int empty_seats = siginfo->si_int;
    printf("Ride Capacity: %d\n", empty_seats);
    if (ride_queue <= empty_seats)
    {
        printf("Queue Size before ride: %d\n", ride_queue);
        ride_queue = 0;
        printf("Queue Size after ride left: %d\n", ride_queue);
    }
    else
    {
        printf("Queue Size before ride: %d\n", ride_queue);
        ride_queue -= empty_seats;
        printf("Queue Size after ride left: %d\n", ride_queue);
    }
}

int main(int argv, char *argc[])
{
    signal(SIGALRM, alarm_handler);
    alarm(100);
    srand(time(0));

    // Create process for ticket guy
    pid_ticket = fork();
    if (!pid_ticket)
    {
        signal(SIGUSR1, SIG_DFL);
        signal(SIGUSR2, SIG_DFL);
        // printf("%d is parent of %d. This is ticket guy.\n",getppid(),getpid());
        // define signal to get response from queue maintainer
        struct sigaction act;
        act.sa_sigaction = receive_signal_queue;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO | SA_RESTART;
        sigaction(SIGUSR1, &act, NULL);
        // wait for the first signal to get the pid of the queue maintainer.
        pause();
        while (!not_alarmed)
        {
            // add random people to ticket counter
            int new_people = (rand() % 6) + 1;
            printf("%d people bought a ticket\n", new_people);
            ticket_counter += new_people;
            // send request to queue maintainer with no. of people
            union sigval sv;
            sv.sival_int = ticket_counter;
            sigqueue(pid_queue, SIGUSR2, sv);
            // wait for response
            pause();
            // add a random time interval to add people to ticket counter
            int num = (rand() % 16) + 5;
            sleep(num);
        }
        exit(1);
    }
    else
    {

        // create process for queue maintainer
        pid_queue = fork();
        if (!pid_queue)
        {
            signal(SIGUSR1, SIG_DFL);
            signal(SIGUSR2, SIG_DFL);

            // printf("%d is parent of %d. This is queue maintainer.\n",getppid(),getpid());

            // define signal to handle request from ticket guy
            struct sigaction act;
            act.sa_sigaction = receive_people;
            sigemptyset(&act.sa_mask);
            act.sa_flags = SA_SIGINFO | SA_RESTART;
            sigaction(SIGUSR2, &act, NULL);

            // send pid_queue to ticket guy process
            union sigval sv;
            sv.sival_int = 0;
            sigqueue(pid_ticket, SIGUSR1, sv);

            // define signal to handle request from driver
            struct sigaction act2;
            act2.sa_sigaction = send_people;
            sigemptyset(&act2.sa_mask);
            act2.sa_flags = SA_SIGINFO | SA_RESTART;
            sigaction(SIGUSR1, &act2, NULL);

            // stall the process
            while (!not_alarmed)
                ;

            exit(1);
        }
        else
        {
            // Create process for driver
            pid_driver = fork();
            if (!pid_driver)
            {
                // printf("%d is parent of %d. This is driver.\n",getppid(),getpid());

                while (!not_alarmed)
                {
                    // generate a random interval for tour
                    int num = (rand() % 6) + 15;
                    sleep(num);
                    // generate random no. of empty seats
                    int seats = (rand() % 7) + 1;

                    // send the seats to queue maintainer
                    union sigval sv;
                    sv.sival_int = seats;
                    sigqueue(pid_queue, SIGUSR1, sv);
                }
                exit(1);
            }
        }
    }

    while (wait(NULL) != -1)
        ;
    while (!not_alarmed)
        ;
    return 0;
}