#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct msgbuff
{
    long mtype;
    char mtext[256];
};

int main()
{
    key_t key_up, key_down;
    int send_val, rec_val, UpQ, DownQ;

    key_up = ftok("keyfile", 65);
    key_down = ftok("keyfile", 70);

    UpQ = msgget(key_up, 0666 | IPC_CREAT);
    DownQ = msgget(key_down, 0666 | IPC_CREAT);

    if (UpQ == -1 || DownQ == -1)
    {
        perror("Error in create.\n");
        exit(-1);
    }
    printf("-----------CLIENT----------\n");
    printf("Up Queue ID = %d\n", UpQ);
    printf("Down Queue ID = %d\n", DownQ);

    printf("Client PID: %d\n", getpid());
    int id = getpid() % 10000;
    printf("Client ID (Last 4 digits): %d\n", id);

    char msg[256];

    while (1) {
        size_t size = 256;
        printf("Enter the message to be sent: ");
        fgets(msg, size, stdin);
        msg[strlen(msg) - 1] = 0;

        struct msgbuff message_sent, message_rec;
        message_sent.mtype = id;
        strcpy(message_sent.mtext, msg);

        send_val = msgsnd(UpQ, &message_sent, sizeof(message_sent.mtext), !IPC_NOWAIT);
        if (send_val == -1)
            perror("Error in send.\n");

        rec_val = msgrcv(DownQ, &message_rec, sizeof(message_rec.mtext), id, !IPC_NOWAIT);
        if (rec_val == -1)
            perror("Error in receive.\n");
        else
            printf("Reversed string received: %s\n", message_rec.mtext);
    }
}
