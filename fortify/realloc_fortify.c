// https://sourceware.org/pipermail/newlib/2018/015845.html
//
// gcc test.c -o test.exe -g -O2 -Wp,-D_FORTIFY_SOURCE=2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// extracted from InputLineAddChar in xserver/xkb/maprules.c
//
int main()
{
  const char *buf[128];
  char *line = reallocarray(NULL, 128, 2);
  // size of line is 128*2 = 256
  printf("%zu\n", __builtin_object_size(line, 0));

  if (__builtin_object_size(line, 0) == 256)
    return 0;

  // otherwise when we do e.g. memcpy below,  __mempcy_chk tests against size 2, and terminates
  memcpy(line, buf, 128);

  return 1;
}

