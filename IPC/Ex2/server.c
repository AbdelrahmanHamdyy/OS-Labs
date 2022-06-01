#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>

void reverse(char* msg, char* reversed);
void handler(int signum);
void down(int sem, int num);
void up(int sem, int num);

int shmid, sem;

/* arg for semctl system calls. */
union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

int main() {

    signal(SIGINT, handler);

    key_t shmKey;
    shmKey = ftok("keyfile", 65);
    shmid = shmget(shmKey, 4096, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("Error in create.\n");
        exit(-1);
    }
    else
        printf("Shared memory ID = %d\n", shmid);

    union Semun semun;

    printf("------------SERVER-----------\n");

    key_t semKey;
    semKey = ftok("keyfile", 60);
    sem = semget(semKey, 4, 0666 | IPC_CREAT);
    void *shmaddr;

    int N;
    printf("Number of clients: ");
    scanf("%d", &N);

    semun.val = N; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem, 2, SETVAL, semun) == -1) {
        perror("Error in semctl");
        exit(-1);
    }

    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem, 1, SETVAL, semun) == -1) {
        perror("Error in semctl");
        exit(-1);
    }

    char reversed[256];

    while (1) {

        down(sem, 0);

        shmaddr = shmat(shmid, (void *)0, 0);
        if (shmaddr == -1)
        {
            perror("Server: Error in attaching to shared memory.\n");
            exit(-1);
        }

        printf("String received: %s\n", (char*) shmaddr);

        reverse((char*) shmaddr, reversed);

        strcpy((char *)shmaddr, reversed);

        printf("Reversed!\n");

        up(sem, 1);
    }

    shmdt(shmaddr);

    return 0;
}

/* reverse a message */
void reverse(char* msg, char* reversed) {
    for (int i = 0; i < strlen(msg); ++i)
        reversed[i] = msg[strlen(msg) - i - 1];
    reversed[strlen(msg)] = 0;
}

void handler(int signum)
{
    printf("\nDestroying the shared memory area & semaphore!\n");
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    for (int i = 0; i < 4; i++)
        semctl(sem, i, IPC_RMID);
    killpg(getpgrp(), SIGKILL);
}

void down(int sem, int num)
{
    struct sembuf p_op;

    p_op.sem_num = num;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem, int num)
{
    struct sembuf v_op;

    v_op.sem_num = num;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}
