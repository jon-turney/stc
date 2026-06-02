// https://cygwin.com/pipermail/cygwin/2025-October/258914.html

#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const int THREAD_COUNT = 10;
const int ITERATION_COUNT = 100;

const int MAX_PATH = 128;

typedef struct {
  int thread_id;
  pthread_barrier_t *barrier;
  const char *dir;
  const char *file;

} thread_args;

static void *thread_func(void *arg) {
  thread_args *args = (thread_args *)arg;

  int res = pthread_barrier_wait(args->barrier);
  assert(res == 0 || res == PTHREAD_BARRIER_SERIAL_THREAD);

  for (int i = 0; i < ITERATION_COUNT; ++i) {
    // open and lock dir
    int dir_fd = open(args->dir, O_RDONLY | O_CLOEXEC, 0);
    assert(dir_fd >= 0);
    int lock_res = flock(dir_fd, LOCK_EX);
    if (lock_res != 0) {
      printf("lock error: %d - %s\n", errno, strerror(errno));
    }
    assert(lock_res == 0);

    // open file
    char file[MAX_PATH];
    assert(snprintf(file, MAX_PATH, "%s/%s", args->dir, "append_file") <
           MAX_PATH);

    int file_fd = open(file, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0600);
    if (file_fd < 0) {
      printf("open file error: %d - %s\n\t%s\n", errno, strerror(errno), file);
    }
    assert(file_fd >= 0);

    // write data
    char data[MAX_PATH];
    int len = snprintf(data, MAX_PATH, "%d-%d\n", args->thread_id, i);
    assert(len < MAX_PATH);
    int write_res = write(file_fd, data, len);
    if (write_res != len) {
      printf("write error: %d != %d: %d - %s\n", write_res, len, errno,
             strerror(errno));
    }
    assert(write_res == len);

    assert(fsync(file_fd) == 0);

    assert(close(file_fd) == 0);
    assert(close(dir_fd) == 0);
  }

  printf("done[%i]\n", args->thread_id);
  return NULL;
}

int main() {
  char tmp_dir[MAX_PATH];
  assert(strncpy(tmp_dir, "/tmp/flockXXXXXX", MAX_PATH) == tmp_dir);
  assert(mkdtemp(tmp_dir) != NULL);
  printf("tmp_dir: %s\n", tmp_dir);

  pthread_barrier_t barrier;
  assert(pthread_barrier_init(&barrier, NULL, THREAD_COUNT) == 0);

  pthread_t thread[THREAD_COUNT];
  thread_args args[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; ++i) {
    args[i].thread_id = i;
    args[i].barrier = &barrier;
    args[i].dir = tmp_dir;
    args[i].file = NULL;
    assert(pthread_create(&thread[i], NULL, &thread_func, &args[i]) == 0);
  }

  void *ret;
  for (int i = 0; i < THREAD_COUNT; ++i) {
    assert(pthread_join(thread[i], &ret) == 0);
  }
  assert(pthread_barrier_destroy(&barrier) == 0);
  printf("done[main]\n");
  return 0;
}
