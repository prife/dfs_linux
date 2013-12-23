#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//int open(const char *pathname, int flags, mode_t mode);
int myopen(const char *pathname, int flags, mode_t mode)
{
    return open(pathname, flags, mode);
}

//ssize_t write(int fd, const void *buf, size_t count);
ssize_t mywrite(int fd, const void *buf, size_t count)
{
    return write(fd, buf, count);
}

//int close(int fd);
int myclose(int fd)
{
    return close(fd);
}
