#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

static void do_slice(regex_t *pattern, FILE *f);

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        fprintf(stderr, "%s: pattern not given.\n", argv[0]);
        exit(1);
    }

    regex_t pattern;
    int err = regcomp(&pattern, argv[1], REG_EXTENDED | REG_NEWLINE);
    if ( err != 0 ) {
        char buf[1024];
        
        regerror(err, &pattern, buf, sizeof buf);
        puts(buf);
        exit(1); 
    }

    if ( argc == 2 ) {
        do_slice(&pattern, stdin);
    } else {
        int i;
        for ( i=2; i<argc; i++ ) {
            FILE *f = fopen(argv[i], "r");

            if ( !f ) {
                perror(argv[i]);
                exit(1);
            }
            do_slice(&pattern, f);
            fclose(f);
        }
    }
    
    regfree(&pattern);
    exit(0);
}

static void do_slice(regex_t *pattern, FILE *f)
{
    char buf[2048];

    while ( fgets(buf, sizeof buf, f) ) {
        regmatch_t matched[1];
        if ( regexec(pattern, buf, 1, matched, 0) == 0 ) {
            char *str = buf + matched[0].rm_so;
            regoff_t len = matched[0].rm_eo - matched[0].rm_so;
            fwrite(str, len, 1, stdout);
            fputc('\n', stdout);
        }
    }
}
