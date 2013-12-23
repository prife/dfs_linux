#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

struct dfsmsg_st {
    long int type;
    char buf[BUFSIZ];
};

int main()
{
    int running = 1;
    int msgid;
    struct dfsmsg_st msg;
    long int msg_to_receive = 0;

    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: % d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (running) {
        if (msgrcv(msgid, (void *)&msg, BUFSIZ,
                   msg_to_receive, 0) == -1) {
            fprintf(stderr, "msgrcv failed with error: % d\n", errno);
            exit(EXIT_FAILURE);
        }
        printf("You wrote: % s", msg.buf);
        if (strncmp(msg.buf, "end", 3) == 0) {
            running = 0;
        }
    }

    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

