#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

void handler(int signum);

int target;
int pids[2];

int main(int argc, char** argv) 
{
    if (argc < 3) {
        printf("Not enough arguments! Exiting..\n");
        return 1;
    }

    int length = argc - 2;
    target = atoi(argv[1]); // Value to search for
    int* values = malloc(length * sizeof(int)); // array
    for (int i = 0; i < length; i++)
    {
        values[i] = atoi(argv[i + 2]);
    }

    printf("I am the parent, PID = %d.\n\n", getpid());
    signal(SIGUSR1, handler);
    int pid;
    int child = 1;
    pid = fork();
    if (pid > 0)
    {
        pids[0] = pid;
        child = 2;
        pid = fork();
        if (pid != 0)
        {
            pids[1] = pid;
            sleep(5);
            int stat_loc = 0;
            for (int i = 0; i < 2; i++)
            {
                waitpid(pids[i], &stat_loc, 0);
                if (WIFEXITED(stat_loc))
                {
                    printf("A child with pid %d terminated with exit code %d.\n\n", getpid(), stat_loc>>8);
                }
            }
            printf("Value Not Found\n\n");
        }
    }
    if (pid == 0)
    {
        int start, end;
        if (child == 1)
        {
            start = 0;
            if (length % 2 == 0)
                end = (length / 2) - 1;
            else
                end = (length / 2);

            printf("I am the first child, PID = %d, PPID = %d.\n\n", getpid(), getppid());
        }
        else
        {
            if (length % 2 == 0)
                start = (length / 2);
            else
                start = (length / 2) + 1;

            printf("I am the second child, PID = %d, PPID = %d.\n\n", getpid(), getppid());
        }
        for (int i = start; i <= end; i++)
        {
            if (values[i] == target)
            {
                kill(getppid(), SIGUSR1);
                exit(i);
            }
        }
        sleep(3);
        if (child == 1)
            printf("Child 1 terminates.\n\n");
        else
            printf("Child 2 terminates.\n\n");

        exit(0);
    }
}

void handler(int signum) 
{
    int stat_loc = 0;
    int pid = wait(&stat_loc);
    int found = stat_loc>>8;
    if (pid == pids[0])
        printf("Child 1: Value %d found at position %d.\n\n", target, found);
    else   
        printf("Child 2: Value %d found at position %d.\n\n", target, found);
    killpg(getpgrp(), SIGKILL);
    signal(SIGUSR1, handler);
}

/* Non-code questions

1) Parent
        Because the child who finds the target value in the array sends the SIGUSR1 signal
        to the parent using the kill(getppid, SIGUSR1) function so the parent recieves this
        signal and handles it as written

2) Even if we did not specify the exit code from the child, a default exit code (0) is sent.
   So it will always be the case that the handler will receive an exit code from Child 1/2.
*/