#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct msgbuff
{
    long mtype;
    char mtext[256];
};

void handler(int signum);
void reverse(char* msg, char* reversed);

int UpQ, DownQ;

int main()
{
    signal(SIGINT, handler);

    key_t key_up, key_down;
    int send_val, rec_val;
    key_up = ftok("keyfile", 65);
    key_down = ftok("keyfile", 70);

    UpQ = msgget(key_up, 0666 | IPC_CREAT);
    DownQ = msgget(key_down, 0666 | IPC_CREAT);

    if (UpQ == -1 || DownQ == -1)
    {
        perror("Error in create.\n");
        exit(-1);
    }
    printf("-----------SERVER----------\n");
    printf("Up Queue ID = %d\n", UpQ);
    printf("Down Queue ID = %d\n", DownQ);

    struct msgbuff message;

    char reversed[256];

    while (1) {
        rec_val = msgrcv(UpQ, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
        if (rec_val == -1)
            printf("Error in receive.\n");

        printf("String received: %s\n", message.mtext);
        reverse(message.mtext, reversed);
        strcpy(message.mtext, reversed);

        send_val = msgsnd(DownQ, &message, sizeof(message.mtext), !IPC_NOWAIT);
        if (send_val == -1)
            perror("Error in send");

        printf("Reversed string sent..\n");
    }
}

/* reverse a message */
void reverse(char* msg, char* reversed) {
    for (int i = 0; i < strlen(msg); ++i)
        reversed[i] = msg[strlen(msg) - i - 1];
    reversed[strlen(msg)] = 0;
}

void handler(int signum)
{
    msgctl(UpQ, IPC_RMID, (struct msqid_ds *)0);
    msgctl(DownQ, IPC_RMID, (struct msqid_ds *)0);
    printf("\n");
    killpg(getpgrp(), SIGKILL);
}
