// https://cygwin.com/pipermail/cygwin/2024-October/256528.html

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int lock_at(int fd, int off, int size)
{
  if (lseek(fd, off, SEEK_SET) < 0) {
    perror("lseek"); exit(1);
  }
  if (lockf(fd, F_LOCK, size) < 0) {
    perror("lock");
    if (errno != ENOLCK)
      exit(1);
    return -1;
  }
  return 0;
}

int main()
{
  const char name[] = "many_locks.tmp";
  int fd = open(name, O_RDWR|O_CREAT, 0644);
  if (fd < 0) {
    perror(name); return 1;
  }

  if (   lock_at(fd, 0, 2)
      || lock_at(fd, 2, 2)
      || lock_at(fd, 1, 2)
      || lock_at(fd, 0, 3)
      || lock_at(fd, 1, 3)
      || lock_at(fd, 0, 4)) {
    fprintf(stderr, "Overlapped locks failed\n");
    return 1;
  }

  // ENOLCK expected after 908 locks (including one from above)
  for (int i = 1; ; i++) {
    if (i >= 1000) {
      fprintf(stderr, "%d locks succeeded unexpectedly\n", i);
      return 1;
    }
    if (lock_at(fd, 6 + 2 * i, 1)) {
      if (i <= 800) {
        fprintf(stderr, "Only %d locks succeeded\n", i);
        return 1;
      }
      break;
    }
  }

  close(fd);
  if (unlink(name)) {
    perror("unlink");
    return 1;
  }

  printf("OK\n");
  return 0;
}
