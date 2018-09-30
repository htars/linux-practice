#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int nlines;

static void do_tail(FILE *f);
static char *readline(FILE *f);

int main(int argc, char *argv[])
{
    if ( argc < 4 ) {
        fprintf(stderr, "%s: wrong number of arguments\n", argv[0]);
        exit(1);
    }

    char opt;
    while ( (opt = getopt(argc, argv, "n:")) != -1 ) {
        switch ( opt ) {
        case 'n':
           nlines = atoi(optarg); 
           break;
        case '?':
           fprintf(stderr, "%s: %c is unknown option\n", argv[0], opt);
           exit(1);
        }
    }
    if ( nlines == 0 ) exit(0);

    argc -= optind;
    argv += optind;

    int i;
    for ( i=0; i<argc; i++ ) {
        FILE *f;

        f = fopen(argv[i], "r");
        if ( !f ) {
            perror(argv[i]);
            exit(1);
        }
        do_tail(f);
        fclose(f);
    }
    exit(0);
}


static void do_tail(FILE *f)
{
    unsigned char **ringbuf;
    unsigned char **p;
    unsigned char *line;
    int i;

#define INC(ptrvar) do { \
    ptrvar++; \
    if ( ptrvar >= ringbuf + nlines ) \
        ptrvar = ringbuf; \
} while ( 0 )

    ringbuf = p = calloc(nlines, sizeof(char *));
    if ( !ringbuf ) exit(1);
    while ( line = readline(f) ) {
        if ( *p ) 
            free(*p);
        *p = line;
        INC(p);
    }
    if ( !(*p) )
        p = ringbuf;
    for ( i=nlines; i>0 && *p; i-- ) {
        printf("%s\n", *p);
        free(*p);
        INC(p);
    }
    free(ringbuf);
}

static char *readline(FILE *f)
{
    unsigned char *p, *buf;
    size_t buflen = BUFSIZ;

    buf = p = (char *)malloc(sizeof(char *) * buflen);
    if ( !buf ) exit(1);
    int c;
    while ( (c = getc(f)) != '\n' ) {
        if ( c == EOF ) {
            if ( p == buf ) {
                free(buf);
                return NULL;
            }
            break;
        }
        *p++ = c;
        if ( p >= buf + buflen - 1 ) {
            buflen *= 2;
            buf = realloc(buf, buflen);
            if ( !buf ) exit(1);
        }
    }
    *p++ = '\0';
    return buf;
}
