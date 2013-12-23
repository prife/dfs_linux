#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "rfs.h"

//int open(const char *pathname, int flags, mode_t mode);
int rfs_open(const char *pathname, int flags, mode_t mode)
{
    struct dfsmsg_st msg_args;
    struct dfsmsg_st msg_ret;
    struct msg_open_args *args = (struct msg_open_args *) msg_args.buf;
    struct msg_open_ret *ret = (struct msg_open_ret *)msg_ret.buf;

    msg_args.type = RFS_OPEN_RET;
    memcpy(args->path, &args, sizeof(ret));
    args->flags = flags;
    args->mode = mode;

    if (msgsnd(msgid, (void *)&msg_args, DFS_MSG_BUF_SIZE, 0) == -1) {
        fprintf(stderr, "msgsnd failed\n");
        exit(EXIT_FAILURE);
    }

    if (msgrcv(msgid, (void *)&msg_ret, DFS_MSG_BUF_SIZE, 0, 0) == -1) {
        fprintf(stderr, "msgrcv failed with error: % d\n", errno);
        exit(EXIT_FAILURE);
    }

    return msg_ret.ret;
}

//ssize_t write(int fd, const void *buf, size_t count);
ssize_t rfs_write(int fd, const void *buf, size_t count)
{
    return write(fd, buf, count);
}

//int close(int fd);
int rfs_close(int fd)
{
    return close(fd);
}

int main()
{
    int msgid;

    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: % d\n", errno);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
