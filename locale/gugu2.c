// https://cygwin.com/pipermail/cygwin/2016-August/229432.html

#include <stdio.h>
#include <locale.h>
#include <regex.h>

int main() {
//
  locale_t locale = newlocale (LC_ALL_MASK, "C", (locale_t) 0);
  if (!locale) return 1;
  locale_t old = uselocale (locale);
  if (!old) return 1;
  //
  char const *pattern[] = { "a", "a{2}" };
  regex_t regex;
  for ( int i = 0 ; i < 2 ; ++i ) {
    printf ("pattern=%s ...\n", pattern[i]);
    regcomp (&regex, pattern[i], REG_EXTENDED);
    printf ("re_nsub=%zd\n", regex.re_nsub);
    regfree (&regex);
  };
  return 0;
};
