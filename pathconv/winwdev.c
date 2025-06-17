/* https://cygwin.com/pipermail/cygwin/2025-April/258069.html */

#include <stdio.h>
#include <stdlib.h>
#include <sys/cygwin.h>
#include <wchar.h>

int main () {
  wchar_t *p = cygwin_create_path (CCP_POSIX_TO_WIN_W, "/dev/sda");
  if (!p)
    {
      perror ("cygwin_create_path");
      return 1;
    }
  printf ("Path returned %S\n", p);
  free (p);
  return 0;
}
