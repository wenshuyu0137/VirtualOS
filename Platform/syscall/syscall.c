// syscalls.c
#include <sys/stat.h>
#include <unistd.h>

int _close_r(struct _reent *r, int file) {
    return 0;
}

int _fstat_r(struct _reent *r, int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty_r(struct _reent *r, int file) {
    return 1;
}

int _getpid_r(struct _reent *r) {
    return 1;
}

int _kill_r(struct _reent *r, int pid, int sig) {
    return 0;
}
