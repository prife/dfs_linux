#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "rfs.h"

#define LOGD  printf

static int hfs_open(const char *pathname, int flags, mode_t mode)
{
    LOGD("HFS: %s\n", __func__);
    return open(pathname, flags, mode);
}

//ssize_t write(int fd, const void *buf, size_t count);
static ssize_t hfs_write(int fd, const void *buf, size_t count)
{
    LOGD("HFS: %s\n", __func__);
    return write(fd, buf, count);
}

//int close(int fd);
static int hfs_close(int fd)
{
    LOGD("HFS: %s\n", __func__);
    return close(fd);
}

enum dfs_status
{
    RFS_STATUS_OK = 0,
    RFS_STATUS_UNKNOWN = 1
};

int main()
{
    int running = 1;
    int msgid;
    enum dfs_status status;
    struct dfsmsg_st msg_args;
    struct dfsmsg_st msg_ret;

    LOGD("%s:%d\n", __FILE__, __LINE__);
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (running) {
        if (msgrcv(msgid, (void *)&msg_args, DFS_MSG_BUF_SIZE, 0, 0) == -1) {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(EXIT_FAILURE);
        }

        if (strncmp(msg_args.buf, "end", 3) == 0) {
            running = 0;
        }

        switch (msg_args.type) {
        case RFS_OPEN_ARGS:
        {
            struct msg_open_args *args = (struct msg_open_args *)msg_args.buf;
            struct msg_open_ret *ret = (struct msg_open_ret *)msg_ret.buf;
            ret->ret = hfs_open(args->path, args->flags, args->mode);
            msg_ret.type = RFS_OPEN_RET;
            status = RFS_STATUS_OK;
            break;
        }
        case RFS_WRITE_ARGS:
        {
            struct msg_write_args *args = (struct msg_write_args *)msg_args.buf;
            struct msg_write_ret *ret = (struct msg_write_ret *)msg_ret.buf;
            ret->ret = hfs_write(args->fd, args->buf, args->count);
            msg_ret.type = RFS_WRITE_RET;
            status = RFS_STATUS_OK;
            break;
        }
        case RFS_CLOSE_ARGS:
        {
            struct msg_close_args *args = (struct msg_close_args *)msg_args.buf;
            struct msg_close_ret *ret = (struct msg_close_ret *) msg_ret.buf;
            ret->ret = hfs_close(args->fd);
            msg_ret.type = RFS_CLOSE_RET;
            status = RFS_STATUS_OK;
            break;
        }
        default:
        {
            status = RFS_STATUS_UNKNOWN;
            LOGD("HFS: unkown type <%ld>\n", msg_args.type);
            break;
        }

        } //end switch

        if (status == RFS_STATUS_UNKNOWN)
            continue;

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

