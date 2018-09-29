#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

struct strbuf {
    char *ptr;
    size_t len;
};

static void traverse(struct strbuf *buf);
static void traverse0(struct strbuf *buf, int first);
static struct strbuf *strbuf_new(void);
static void strbuf_realloc(struct strbuf *buf, size_t size);
static void print_error(char *s);

static char *program_name;

int main(int argc, char *argv[])
{
    struct strbuf *pathbuf;

    if ( argc != 2 ) {
        fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
        exit(1);
    }
    program_name = argv[0];
    pathbuf = strbuf_new();
    strbuf_realloc(pathbuf, strlen(argv[1]));
    strcpy(pathbuf->ptr, argv[1]);
    traverse(pathbuf);
    exit(0);
}

static void traverse(struct strbuf *pathbuf)
{
    traverse0(pathbuf, 1);
}

static void traverse0(struct strbuf *pathbuf, int first)
{
    DIR *d;
    struct dirent *ent;
    struct stat st;

    d = opendir(pathbuf->ptr);
    if ( !d ) {
        switch ( errno ) {
        case ENOTDIR:
            return;
        case ENOENT:
            if ( first ) {
                print_error(pathbuf->ptr);
                exit(1);
            } else {
                return;
            }
        case EACCES:
            puts(pathbuf->ptr);
            print_error(pathbuf->ptr);
            return;
        default:
            print_error(pathbuf->ptr);
            exit(1);
        }
    }
    puts(pathbuf->ptr);
    while ( ent = readdir(d) ) {
        if ( strcmp(ent->d_name, ".") == 0 ) continue;
        if ( strcmp(ent->d_name, "..") == 0 ) continue;
        strbuf_realloc(pathbuf, pathbuf->len + 1 + strlen(ent->d_name) + 1);
        if ( strcmp(pathbuf->ptr, "/") != 0 ) {
            strcat(pathbuf->ptr, "/");
        }
        strcat(pathbuf->ptr, ent->d_name);
        if ( lstat(pathbuf->ptr, &st) < 0 ) {
            switch ( errno ) {
            case ENOENT:
                break;
            case EACCES:
                print_error(pathbuf->ptr);
                break;
            default:
                print_error(pathbuf->ptr);
                exit(1);
            }
        } else {
            if ( S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode) ) {
               traverse0(pathbuf, 0);
            } else {
                puts(pathbuf->ptr);
            }
        }
        *strrchr(pathbuf->ptr, '/') = '\0';
    }
    closedir(d);
}

#define INITLEN 1024

/*
strbuf構造体に対するポインタを返す関数

文字列型変数ptrのメモリサイズをINITLENバイトだけ確保
*/
static struct strbuf *strbuf_new(void)
{
    struct strbuf *buf;

    buf = (struct strbuf *)malloc(sizeof(struct strbuf));
    if ( !buf ) {
        print_error("malloc(3)");
        exit(1);
    }
    buf->ptr = malloc(INITLEN);
    if ( !buf->ptr) {
        print_error("malloc(3)");
        exit(1);
    }
    buf->len = INITLEN;
    return buf;
}

/*
strbuf構造体のメンバptrのメモリがlenバイト以下であったら、
lenに拡張し直す関数
*/
static void strbuf_realloc(struct strbuf *buf, size_t len)
{
    char *tmp;

    if ( buf->len > len ) return;
    tmp = realloc(buf->ptr, len);
    if ( !tmp ) {
        print_error("realloc(3)");
        exit(1);
    }
    buf->ptr = tmp;
    buf->len = len;
}

static void print_error(char *s)
{
    fprintf(stderr, "%s: %s: %s\n", program_name, s, strerror(errno));
}
