// https://cygwin.com/pipermail/cygwin/2025-December/259189.html

// #define SLEEP

#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const int FORK_COUNT = 10;

typedef struct {
  int id;
} call_args;

static void *flock_call(void *arg) {
  call_args *args = (call_args *)arg;

#ifdef SLEEP
  if (args->id != -1) {
    struct timespec duration;
    duration.tv_sec = 0;
    duration.tv_nsec = 1;
    nanosleep(&duration, NULL);
  }
#endif

  // open and lock dir
  int dir_fd = open("/cygdrive/c/cygwin64/etc", O_RDONLY, 0);
  if (dir_fd < 0) {
    printf("open dir error: %d - %s\n", errno, strerror(errno));
  }
  assert(dir_fd >= 0);

  // lock
  int lock_res = flock(dir_fd, LOCK_SH);
  if (lock_res != 0) {
    printf("lock error: %d - %s\n", errno, strerror(errno));
  }
  assert(lock_res == 0);

  // close
  assert(close(dir_fd) == 0);

  printf("done[%i]\n", args->id);
  return NULL;
}

int main() {
  call_args main_args;
  main_args.id = -1;

  // spawn children
  pid_t children[FORK_COUNT];
  for (int i = 0; i < FORK_COUNT; ++i) {
    call_args args;
    args.id = i;

    children[i] = fork();
    if (children[i] == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    if (children[i] == 0) {
      flock_call(&args);
      return 0;
    }
    flock_call(&main_args);
  }

  // wait children
  for (int i = 0; i < FORK_COUNT; ++i) {
    int wstatus;
    pid_t w;
    do {
      w = waitpid(children[i], &wstatus, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }

  printf("done[main]\n");
  return 0;
}
