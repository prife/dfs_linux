#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

#define MAX_TEXT 512

struct dfsmsg_st {
    long int type;
    char buf[MAX_TEXT];
};

int main()
{
    int running = 1;
    struct dfsmsg_st msg;
    int msgid;
    char buffer[BUFSIZ];

    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: % d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (running) {
        printf("Enter some text: ");
        fgets(buffer, BUFSIZ, stdin);
        msg.type = 1;
        strcpy(msg.buf, buffer);

        if (msgsnd(msgid, (void *)&msg, MAX_TEXT, 0) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, "end", 3) == 0) {
            running = 0;
        }
    }
    exit(EXIT_SUCCESS);
}
