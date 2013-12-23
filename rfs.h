#ifndef RFS_H
#define RFS_H

#define FILE_NAME_MAX 256
#define DFS_MSG_BUF_SIZE (1024*32)
#define DFS_RW_BUF_SIZE  (1024*16)

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
    char path[FILE_NAME_MAX];
    int flags;
    int mode;
};

struct msg_open_ret
{
    int ret;
}

//ssize_t write(int fd, const void *buf, size_t count);
struct msg_write_args
{
    int fd;
    char buf[RFS_BUF_SIZE];
    size_t count;
};

struct msg_write_ret
{
    ssize_t ret;
}

//int close(int fd)
struct msg_close_args
{
    int fd;
};

struct msg_close_ret
{
    int ret;
}

#endif // RFS_H

