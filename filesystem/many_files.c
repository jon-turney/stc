// https://sourceware.org/pipermail/cygwin/2026-May/259664.html
//
// This test creates files until no file descriptors are left and checks whether
// - open() fails if and only if RLIMIT_NOFILE has been exactly reached,
// - open() fails only with errno=EMFILE,
// - file descriptors are allocated in ascending order without gaps,
// - stat() and unlink() succeed if and only if open() succeeded.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>

int main()
{
  struct rlimit rl;
  if (getrlimit(RLIMIT_NOFILE, &rl)) {
    fprintf(stderr, "getrlimit() failed, errno=%d\n", errno);
    return 1;
  }
  int n = (rl.rlim_cur < 10000 ? (int)rl.rlim_cur : 10000);

  if (close_range(3, ~0U, 0)) {
    fprintf(stderr, "close_range() failed, errno=%d\n", errno);
    return 1;
  }

  int status = 0;
  char name[32];
  int i;
  for (i = 3; !status && i <= n; i++) {
    snprintf(name, sizeof(name), "file-%04d.tmp", i);
    if (unlink(name) && errno != ENOENT) {
      fprintf(stderr, "%s: unlink() failed with unexpected errno=%d\n", name, errno);
      status = 1;
      break;
    }

    int fd = open(name, O_WRONLY | O_CREAT, 0600);
    struct stat st;
    if (fd == -1) {
      if (!(errno == EMFILE && (rlim_t)i == rl.rlim_cur)) {
        fprintf(stderr, "%s: open() failed, errno=%d\n", name, errno);
        status = 1;
      }
      if (!stat(name, &st)) {
        fprintf(stderr, "%s: open() failed but stat() succeeded\n", name);
        i++;
        status = 1;
      }
      break;
    }

    if ((rlim_t)i >= rl.rlim_cur) {
      fprintf(stderr, "%s: open() succeeded despite %d >= %lu\n", name, i, rl.rlim_cur);
      status = 1;
    }
    if (fd != i) {
      fprintf(stderr, "%s: unexpected fd %d, expected %d\n", name, fd, i);
      status = 1;
    }
    if (stat(name, &st)) {
      fprintf(stderr, "%s: stat() failed, errno=%d\n", name, errno);
      status = 1;
    }
  }

  if (i > 3 && close_range(3, i - 1, 0)) {
    fprintf(stderr, "close_range() failed, errno=%d\n", errno);
    status = 1;
  }

  while (--i >= 3) {
    snprintf(name, sizeof(name), "file-%04d.tmp", i);
    if (unlink(name)) {
      fprintf(stderr, "%s: unlink() failed, errno=%d\n", name, errno);
      status = 1;
    }
  }

  if (!status)
    printf("%d files created and removed successfully\n", n - 3);
  return status;
}
