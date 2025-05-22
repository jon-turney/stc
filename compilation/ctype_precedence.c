// https://sourceware.org/pipermail/newlib/2009/008132.html

#include <assert.h>
#include <ctype.h>

int main()
{
  int c;
  int t = isspace(c = ' ');
  assert(t);
  return 0;
}
