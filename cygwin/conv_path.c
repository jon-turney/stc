// https://cygwin.com/pipermail/cygwin/2025-April/258068.html
//
// excercise that cygwin_conv_path accepts to=NULL with size=0

#include <stdio.h>
#include <sys/cygwin.h>

int main(void)
{
  wchar_t *buf = cygwin_create_path (CCP_POSIX_TO_WIN_W, "/dev/sda");
  if (!buf)
  {
    perror ("cygwin_create_path");
    return 1;
  }
  printf("%ls\n", buf);
  return 0;
}

