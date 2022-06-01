#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

/* arg for semctl system calls. */
union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

int sem;

void handler(int signum);
void down(int sem, int num);
void up(int sem, int num);

int main() {

    signal(SIGINT, handler);

    key_t shmKey;
    shmKey = ftok("keyfile", 65);
    int shmid = shmget(shmKey, 4096, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("Error in create.\n");
        exit(-1);
    }
    else
        printf("Shared memory ID = %d\n", shmid);

    union Semun semun;

    key_t semKey;
    semKey = ftok("keyfile", 60);
    sem = semget(semKey, 4, 0666 | IPC_CREAT);

    void *shmaddr;

    printf("------------CLIENT-----------\n");

    semun.val = 1; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem, 3, SETVAL, semun) == -1) {
        perror("Error in semctl");
        exit(-1);
    }

    down(sem, 2);

    char msg[256];

    while (1) {

        down(sem, 3);
        semun.val = 0; /* initial value of the semaphore, Binary semaphore */
        if (semctl(sem, 0, SETVAL, semun) == -1) {
            perror("Error in semctl");
            exit(-1);
        }

        size_t size = 256;
        printf("Message: ");
        fgets(msg, size, stdin);
        msg[strlen(msg) - 1] = 0;

        shmaddr = shmat(shmid, (void *)0, 0);
        if ((long) shmaddr == -1)
        {
            perror("Client: Error in attaching to shared memory.\n");
            exit(-1);
        }

        strcpy((char *)shmaddr, msg);

        up(sem, 0);

        down(sem, 1);

        printf("Reversed String: %s\n", (char*) shmaddr);

        up(sem, 3);
    }

    up(sem, 2);

    shmdt(shmaddr);

    return 0;
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

void handler(int signum) {
    printf("\nFreeing client.\n");
    up(sem, 2);
    raise(SIGKILL);
}
