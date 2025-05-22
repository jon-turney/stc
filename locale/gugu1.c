// https://cygwin.com/pipermail/cygwin/2016-August/229419.html

#define _GNU_SOURCE 1
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

//
int main() {
//
  locale_t locale = newlocale (LC_ALL_MASK, "C", (locale_t) 0);
  if (!locale) return 1;
  char const *nptr = "1.5";
  char *endptr;
  double r1 = strtod (nptr, &endptr);
  fprintf (stderr, "r1=%f\n", r1);
  double r2 = strtod_l (nptr, &endptr, locale);
  fprintf (stderr, "r2=%f\n", r2);
  printf ("exit\n");
  return 0;
};
