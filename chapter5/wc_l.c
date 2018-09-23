#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int do_wc_l(const char *path);
static void die(const char *path);

int main(int argc, char *argv[])
{
    int i;

    if ( argc < 2 ) {
        fprintf(stderr, "%s: file name not given.\n", argv[0]);
        exit(1);
    }

    for ( i=1; i<argc; i++ ) {
        printf("%s: %d lines.\n", argv[i], do_wc_l(argv[i]));
    }
    exit(0);
}

#define BUFFER_SIZE 2048

static int do_wc_l(const char *path)
{
    int fd = open(path, O_RDONLY);
    int n, i, count = 0;
    unsigned char buf[BUFFER_SIZE];
    
    if ( fd < 0 ) die(path);
    while ( (n = read(fd, buf, sizeof buf)) != 0 ) {
        if ( n < 0 ) die(path);
        for ( i=0; i<n; i++ ) {
            if ( buf[i] == '\n' ) count++;
        }
    }
    close(fd);
    return count;
}

static void die(const char *path)
{
    perror(path);
    exit(1);
}
