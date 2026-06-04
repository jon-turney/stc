// trivial exerciser of POSIX.1-2024 _Fork() API
//
// https://cygwin.com/pipermail/cygwin-patches/2026q1/014899.html

#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int pre = 0;
int pnt = 0;
int chld = 0;

void
prepare (void)
{
  pre = 1;
}

void
parent (void)
{
  pnt = 1;
}

void
child (void)
{
  chld = 1;
}

void fork_test(int atfork_enable)
{
  printf ("Calling %s\n", atfork_enable ? "fork()" : "_Fork()");

  switch (atfork_enable ? fork (): _Fork ())
    {
    case -1:
      printf ("error %d %s\n", errno, strerror (errno));
      assert(0);
    case 0:
      printf ("child : prepare %d, parent %d, child %d\n", pre, pnt, chld);
      assert(pre == atfork_enable);
      assert(pnt == 0);
      assert(chld == atfork_enable);
      break;
    default:
      sleep (1);
      printf ("parent: prepare %d, parent %d, child %d\n", pre, pnt, chld);
      assert(pre == atfork_enable);
      assert(pnt == atfork_enable);
      assert(chld == 0);
      break;
    }
}

int
main (int argc, char **argv)
{
  pthread_atfork (prepare, parent, child);

  fork_test(0);
  sleep(5);
  fork_test(1);

  return 0;
}
