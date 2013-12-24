#ifndef RFS_H
#define RFS_H

#define DFS_MSG_BUF_SIZE 512//(1024*16)

struct dfsmsg_st
{
    long int type;
    char buf[DFS_MSG_BUF_SIZE];
};

enum dfsmsg_type
{
    RFS_OPEN_ARGS = 1,
    RFS_OPEN_RET,
    RFS_WRITE_ARGS,
    RFS_WRITE_RET,
    RFS_CLOSE_ARGS,
    RFS_CLOSE_RET,
};

//open (const char *pathname, int flags, mode_t mode)
struct msg_open_args
{
    int flags;
    int mode;
    char path[0];
};

struct msg_open_ret
{
    int ret;
};

//ssize_t write(int fd, const void *buf, size_t count);
struct msg_write_args
{
    int fd;
    size_t count;
    char buf[0];
};

struct msg_write_ret
{
    ssize_t ret;
};

//int close(int fd)
struct msg_close_args
{
    int fd;
};

struct msg_close_ret
{
    int ret;
};

#endif // RFS_H

