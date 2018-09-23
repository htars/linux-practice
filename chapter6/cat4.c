#include <stdio.h>
#include <stdlib.h>

static void do_cat(const char *path);
static void die(const char *path);

int main(int argc, char *argv[])
{
    int i;

    if ( argc < 2 ) {
        fprintf(stderr, "%s: file name not given.\n", argv[0]);
        exit(1);
    }

    for ( i=1; i<argc; i++ ) {
        do_cat(argv[i]);
    }
    exit(0);
}

#define BUFFER_SIZE 2048

static void do_cat(const char *path) {
    FILE *f = fopen(path, "r");
    unsigned char buf[BUFFER_SIZE];
    
    if ( !f ) die(path);
    while ( 1 ) {
        size_t n_read = fread(buf, 1, sizeof buf, f);
        if ( ferror(f) ) die(path);
        size_t n_write = fwrite(buf, 1, n_read, stdout);
        if ( n_write < n_read) die(path);
        if ( n_read < sizeof buf ) break;
    }
    fclose(f);
}

static void die(const char *path)
{
    perror(path);
    exit(1);
}
