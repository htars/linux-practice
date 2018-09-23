#include <stdio.h>
#include <stdlib.h>

static int do_wc_l(const char *path);

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

static int do_wc_l(const char *path)
{
    FILE *f = fopen(path, "r");
    int c, prev='\n', count=0;

    if ( !f ) {
        perror(path);
        exit(1);
    }
    while ( (c = fgetc(f)) != EOF ) {
       if ( c == '\n' ) count++;
       prev = c; 
    }
    if ( prev != '\n' ) count++;

    return count;
}
