// https://cygwin.com/pipermail/cygwin-developers/2023-July/012640.html

//
// This test verifies that a socket correctly indicates not ready to write
// when poll()ed if a subsequent write() would block
//
// 1) create a socketpair, then 2) repeatedly while polling if socket is
// writeable, write 100 zeroes to socket
//
// This should stop when socket is not writeable, but actually just seems to
// block in the write.
//
// See commit a841911ea45272b1333134aa0d8bd9b3860a2c7f for an abortive
// attempt to fix this. (Summarize the problems that caused)
//
// The behaviour this test is checking for is provided by linux, but not
// required by the standards. If you absolutely need to not block, use
// non-blocking sockets!

#include <assert.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static void timeout(int signum)
{
  exit(1);
}

int main()
{
  char wbuf[100] = { 0, };
  int out;

  signal(SIGALRM, timeout);

  {
    int sv[2];
    int s;

    s = socketpair (AF_UNIX, SOCK_STREAM, 0, sv);
    assert (s == 0);

    out = sv[0];
  }

  size_t in_flight = 0;
  while (1)
    {
      struct pollfd fds[1];
      fds[0].fd = out;
      fds[0].events = POLLOUT;

      int r = poll(fds, 1, 0);
      assert(r >= 0);

      // fd is not ready to write
      if (!(fds[0].revents & POLLOUT))
        break;

      alarm(5);

      // otherwise, fd is ready to write, implies some data may be written without blocking
      ssize_t s = write (out, wbuf, sizeof wbuf);
      assert (s > 0);
      in_flight += s;
      printf("%zd written, total in_flight %zd\n", s, in_flight);

      alarm(0);
    }
}
