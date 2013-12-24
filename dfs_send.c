#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "rfs.h"

#define LOGD  printf

static int msgid;

//int open(const char *pathname, int flags, mode_t mode);
int rfs_open(const char *pathname, int flags, mode_t mode)
{
    struct dfsmsg_st msg_args;
    struct dfsmsg_st msg_ret;
    struct msg_open_args *args = (struct msg_open_args *) msg_args.buf;
    struct msg_open_ret *ret = (struct msg_open_ret *)msg_ret.buf;

    msg_args.type = RFS_OPEN_ARGS;
    args->flags = flags;
    args->mode = mode;
    strcpy(args->path, pathname);

    if (msgsnd(msgid, (void *)&msg_args, DFS_MSG_BUF_SIZE, 0) == -1) {
        //fprintf(stderr, "RFS: <%s> msgsnd failed\n", __func__);
        perror("RFS: rfs_open msgsnd:");
        exit(EXIT_FAILURE);
    }
    LOGD("RFS: %s call remote\n", __func__);

    if (msgrcv(msgid, (void *)&msg_ret, DFS_MSG_BUF_SIZE, RFS_OPEN_RET, 0) == -1) {
        fprintf(stderr, "RFS: <%s> msgrcv failed with error: %d\n", __func__, errno);
        exit(EXIT_FAILURE);
    }
    LOGD("RFS: %s get return\n", __func__);

    return ret->ret;
}

//ssize_t write(int fd, const void *buf, size_t count);
ssize_t rfs_write(int fd, const void *buf, size_t count)
{
    struct dfsmsg_st msg_args;
    struct dfsmsg_st msg_ret;
    struct msg_write_args *args = (struct msg_write_args *) msg_args.buf;
    struct msg_write_ret *ret = (struct msg_write_ret *)msg_ret.buf;

    msg_args.type = RFS_WRITE_ARGS;
    args->fd = fd;
    memcpy(args->buf, buf, count);
    args->count = count;

    if (msgsnd(msgid, (void *)&msg_args, DFS_MSG_BUF_SIZE, 0) == -1) {
        fprintf(stderr, "RFS: <%s> msgsnd failed\n", __func__);
        exit(EXIT_FAILURE);
    }

    LOGD("RFS: %s call remote\n", __func__);

    if (msgrcv(msgid, (void *)&msg_ret, DFS_MSG_BUF_SIZE, RFS_WRITE_RET, 0) == -1) {
        fprintf(stderr, "RFS: <%s> msgrcv failed with error: %d\n", __func__, errno);
        exit(EXIT_FAILURE);
    }

    LOGD("RFS: %s get return\n", __func__);

    return ret->ret;
}

//int close(int fd);
int rfs_close(int fd)
{
    struct dfsmsg_st msg_args;
    struct dfsmsg_st msg_ret;
    struct msg_close_args *args = (struct msg_close_args *) msg_args.buf;
    struct msg_close_ret *ret = (struct msg_close_ret *)msg_ret.buf;

    msg_args.type = RFS_CLOSE_ARGS;
    args->fd = fd;

    if (msgsnd(msgid, (void *)&msg_args, DFS_MSG_BUF_SIZE, 0) == -1) {
        fprintf(stderr, "RFS: <%s> msgsnd failed\n", __func__);
        exit(EXIT_FAILURE);
    }

    LOGD("RFS: %s call remote\n", __func__);

    if (msgrcv(msgid, (void *)&msg_ret, DFS_MSG_BUF_SIZE, RFS_CLOSE_RET, 0) == -1) {
        fprintf(stderr, "RFS: <%s> msgrcv failed with error: %d\n", __func__, errno);
        exit(EXIT_FAILURE);
    }

    LOGD("RFS: %s get return\n", __func__);

    return ret->ret;
}

static char *const dfs_get_argv[] =
{"dfs_get", 0};
#define MSG "hello, world"
#include <fcntl.h>

int main(int argc, char *argv[])
{
    pid_t p;
    int fd;
    char *name;
    if (argc < 2)
    {
        printf("\n    usage:\n"
               "       %s file\n", argv[0]);
        return -1;
    }
    name = argv[1];

    /* fork */
    p = fork();
    if (p == -1) {
        //exec the dfs hfs process
    } else if (p == 0){
        printf("dfs_get\n");  //child
        execv("./dfs_get", dfs_get_argv);
    } else {
        printf("dfs_send\n"); //parent
    }

    /* create message queue */
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "RFS: msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    /* test the dfs_linux */
    fd = rfs_open(name, O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0)
    {
        LOGD("RFS: open error!\n");
        return -1;
    }

    if (rfs_write(fd, MSG, sizeof(MSG)) != sizeof(MSG))
    {
        LOGD("RFS: write error!\n");
        rfs_close(fd);
        return -1;
    }
    rfs_close(fd);

    exit(EXIT_SUCCESS);
}
