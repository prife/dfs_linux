#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>
#include <unistd.h>

#define O_ACCMODE      0003
#define O_RDONLY       00
#define O_WRONLY       01
#define O_RDWR         02
#define O_CREAT        0100 /* not fcntl */
#define O_EXCL         0200 /* not fcntl */
#define O_NOCTTY       0400 /* not fcntl */
#define O_TRUNC        01000 /* not fcntl */
#define O_APPEND       02000
#define O_NONBLOCK     04000
#define O_NDELAY       O_NONBLOCK
#define O_SYNC         010000
#define O_FSYNC        O_SYNC
#define O_ASYNC        020000

struct dfs_ops
{
    int (*open)(const char *pathname, int flags, mode_t mode);
    ssize_t (*write)(int fd, const void *buf, size_t count);
    int (*close)(int fd);
};

struct dfs_ops ops;

int open(const char *pathname, int flags, mode_t mode)
{
    printf("my open enter\n");
    return ops.open(pathname, flags, mode);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    printf("my write enter\n");
    return ops.write(fd, buf, count);
}

int close(int fd)
{
    printf("my close enter\n");
    return ops.close(fd);
}

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define LIBNAME "libdfs.so"
#define CWD "/home/prife/tempwork/rt-thread/bsp/simulator/abc/"
int libdfs_init()
{
    void *handle;
    char *error;
    //handle = dlopen(CWD LIBNAME, RTLD_LAZY);
    handle = dlopen(CWD LIBNAME, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    dlerror();    /* Clear any existing error */

    ops.open = dlsym(handle, "myopen");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    ops.write = dlsym(handle, "mywrite");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    ops.close = dlsym(handle, "myclose");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    return 0;
}

#define MSG "hello, world"
int main(int argc, char *argv[])
{
    int fd;
    char *name;
    if (argc < 2)
    {
        printf("\n    usage:\n"
               "       %s file\n", argv[0]);
        return -1;
    }

    /* load dfs library */
    if (libdfs_init() < 0)
    {
        printf("load libdfs.so failed!\n");
        return -1;
    }

    /* test */
    name = argv[1];
    fd = open(name, O_CREAT | O_TRUNC | O_RDWR, 0777);
    if (fd < 0)
    {
        perror("open error!");
        return -1;
    }

    if (write(fd, MSG, sizeof(MSG)) != sizeof(MSG))
    {
        perror("write error!");
        return -1;
    }
    close(fd);

    return 0;
}
