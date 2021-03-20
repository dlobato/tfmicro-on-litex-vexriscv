#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <base/uart.h>
#include <generated/csr.h>

#include "riscv.h"

#ifdef __cplusplus
extern "C" {
#endif

               
void _exit(int code)
{
    while (1);
}

void *_sbrk(ptrdiff_t incr)
{
    extern char _heap_start[];
    extern char _heap_end[];
    static char *curbrk = _heap_start;

    if ((curbrk + incr < _heap_start) || (curbrk + incr > _heap_end))
    {
        return ((char *) - 1);
    }

    curbrk += incr;
    return curbrk - incr;
}

int _isatty(int fd)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        return 1;
    }

    errno = EBADF;
    return 0;
}

static int stub(int err)
{
    errno = err;
    return -1;
}

int _open(const char* name, int flags, int mode)
{
    return stub(ENOENT);
}

int _openat(int dirfd, const char* name, int flags, int mode)
{
    return stub(ENOENT);
}

int _close(int fd)
{
    return stub(EBADF);
}

int _execve(const char* name, char* const argv[], char* const env[])
{
    return stub(ENOMEM);
}

int _fork()
{
    return stub(EAGAIN);
}

int _fstat(int fd, struct stat *st)
{
    if (isatty(fd))
    {
        st->st_mode = S_IFCHR;
        return 0;
    }

    return stub(EBADF);
}

int _getpid()
{
    return 1;
}

int _kill(int pid, int sig)
{
    return stub(EINVAL);
}

int _link(const char *old_name, const char *new_name)
{
    return stub(EMLINK);
}

off_t _lseek(int fd, off_t ptr, int dir)
{
    if (_isatty(fd))
    {
        return 0;
    }

    return stub(EBADF);
}

ssize_t _read(int fd, void* ptr, size_t len)
{
    return stub(EBADF);
}

int _stat(const char* file, struct stat* st)
{
    return stub(EACCES);
}

clock_t _times(struct tms* buf)
{
    uint64_t cycles;

	cycles = read_csr(mcycle);
	cycles |= (uint64_t)read_csr(mcycleh) << 32;

    if (buf != NULL) {
        buf->tms_utime = cycles;
        buf->tms_stime = 0;
        buf->tms_cutime = 0;
        buf->tms_cstime = 0;
    }

    return cycles;
}

int _gettimeofday(struct timeval  *ptimeval, void *ptimezone)
{
    if (ptimeval != NULL) {
        uint64_t cycles = _times(NULL);

	    ptimeval->tv_sec = (time_t)(cycles / CONFIG_CLOCK_FREQUENCY);
        ptimeval->tv_usec = (suseconds_t)(cycles / (CONFIG_CLOCK_FREQUENCY/1000000));
    }

    (void)ptimezone;

	return 0;
}

int _unlink(const char* name)
{
    return stub(ENOENT);
}

int _wait(int* status)
{
    return stub(ECHILD);
}

ssize_t _write(int fd, const void* ptr, size_t len)
{
  const uint8_t * current = (const uint8_t *) ptr;
  size_t jj;

  if (_isatty(fd))
  {
    for (jj = 0; jj < len; jj++)
    {
        uart_write(*(current + jj));
        if (current[jj] == '\n')
        {
            uart_write('\r');
        }
    }
    return len;
  }
  return stub(EBADF);
}

#ifdef __cplusplus
}
#endif