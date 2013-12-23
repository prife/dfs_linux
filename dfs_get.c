#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "rfs.h"

static int hfs_open(const char *pathname, int flags, mode_t mode)
{
    return open(pathname, flags, mode);
}

//ssize_t write(int fd, const void *buf, size_t count);
static ssize_t hfs_write(int fd, const void *buf, size_t count)
{
    return write(fd, buf, count);
}

//int close(int fd);
static int hfs_close(int fd)
{
    return close(fd);
}

int main()
{
    int running = 1;
    int msgid;
    struct dfsmsg_st msg_args;
    struct dfsmsg_st msg_ret;

    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: % d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (running) {
        if (msgrcv(msgid, (void *)&msg_args, BUFSIZ, 0, 0) == -1) {
            fprintf(stderr, "msgrcv failed with error: % d\n", errno);
            exit(EXIT_FAILURE);
        }

        LOGD("You wrote: % s", msg_args.buf);
        if (strncmp(msg_args.buf, "end", 3) == 0) {
            running = 0;
        }

        switch (msg_args.type)
        {
            case RFS_OPEN_ARGS:
            {
                struct msg_open_args *args = msg_args.buf;
                struct msg_open_ret ret;
                msg_open_ret.ret = hfs_open(args->path, args->flags, args->mode);
                msg_ret.type = RFS_OPEN_RET;
                memcpy(msg_ret.buf, &ret, sizeof(ret));
                break;
            }
            case RFS_WRITE_ARGS:
            {
                struct msg_write_args *args = msg_args.buf;
                struct msg_write_ret ret;
                msg_write_ret.ret = hfs_write(args->fd, args->buf, args->count);
                msg_write.type = RFS_WRITE_RET;
                memcpy(msg_ret.buf, &ret, sizeof(ret));
                break;
            }
            case RFS_CLOSE_ARGS:
            {
                struct msg_close_args *args = msg_args.buf;
                struct msg_close_ret ret;
                msg_close_ret.ret = hfs_close(args->fd);
                msg_close.type = RFS_WRITE_RET;
                memcpy(msg_ret.buf, &ret, sizeof(ret));
                break;
            }
            default:
                break;
        }

        //send back: FIXME, send size may vary!
        if (msgsnd(msgid, (void *)&msg_ret, DFS_MSG_BUF_SIZE, 0) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }
    }

    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

