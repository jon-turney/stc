// https://sourceware.org/pipermail/newlib/2018/015845.html
// realloc_fortify.c modified for _FORTIFY_SOURCE=3

#undef FORTIFY_SOURCE
#define FORTIFY_SOURCE 3

#include <stdio.h>
#if __has_builtin(__builtin_dynamic_object_size)
#include <stdlib.h>
#include <string.h>

volatile int i128 = 128; // prevent constant propagation

int main()
{
  const char *buf[128];
  char *line = reallocarray(NULL, i128, 2);
  // size of line is 128*2 = 256
  printf("%zd\n", __builtin_object_size(line, 0)); // (size_t)-1
  printf("%zd\n", __builtin_dynamic_object_size(line, 0));

  if (__builtin_dynamic_object_size(line, 0) == 256)
    return 0;

  // otherwise when we do e.g. memcpy below,  __mempcy_chk tests against size 2, and terminates
  memcpy(line, buf, 128);

  return 1;
}

#else // !__have_builtin( __builtin_dynamic_object_size)
int main()
{
  printf("_FORTIFY_SOURCE=3 not supported\n");
  return 0;
}
#endif
