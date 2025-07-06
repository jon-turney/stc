// https://cygwin.com/pipermail/cygwin-patches/2025q2/014001.html
// https://sourceware.org/pipermail/newlib/2025/021937.html

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static int test(const wchar_t * w, const char * s, const char * t)
{
  const size_t bufsiz = 42;
  char sbuf[bufsiz];
  size_t n1 = wcstombs(NULL, w, 0);
  size_t n2 = wcstombs(sbuf, w, bufsiz);
  if (n1 != n2) {
    fprintf(stderr, "%s: wcstombs: %zd != %zd\n", t, n1, n2); return 1;
  }
  if (n1 >= bufsiz) {
    fprintf(stderr, "%s: wcstombs: failed\n", t); return 1;
  }
  if (strcmp(sbuf, s)) {
    fprintf(stderr, "%s: wcstombs: invalid result\n", t); return 1;
  }

  wchar_t wbuf[bufsiz];
  size_t n3 = mbstowcs(NULL, s, 0);
  size_t n4 = mbstowcs(wbuf, s, bufsiz);
  if (n3 != n4) {
    fprintf(stderr, "%s: mbstowcs: %zd != %zd\n", t, n3, n4); return 1;
  }
  if (n3 >= bufsiz) {
    fprintf(stderr, "%s: mbstowcs: failed\n", t); return 1;
  }
  if (wcscmp(wbuf, w)) {
    fprintf(stderr, "%s: mbstowcs: invalid result\n", t); return 1;
  }
  return 0;
}

int main()
{
  setlocale(LC_ALL, "en_US.UTF-8");
  int rc = 0;
  rc |= test(L"\xFFFF", "\xEF\xBF\xBF", "U+FFFF");
  rc |= test(L"\xD800\xDC00", "\xF0\x90\x80\x80", "U+10000");
  rc |= test(L"\xD83C\xDF21", "\xF0\x9F\x8C\xA1", "U+1F321 (Thermometer)");
  rc |= test(L"\xDBFF\xDFFF", "\xF4\x8F\xBF\xBF", "U+10FFFF");
  rc |= test(L"t-\xDC00\xD800!", "t-\xED\xB0\x80\xED\xA0\x80!", "Lone UTF-16 surrogates");
  rc |= test(L"\xDC01", "\xED\xB0\x81", "Single UTF-16 low surrogate");
  rc |= test(L"\xD801", "\xED\xA0\x81", "Single UTF-16 high surrogate");
  rc |= test(L"\xDC01!", "\xED\xB0\x81!", "Leading UTF-16 low surrogate");
  rc |= test(L"t-\xD802", "t-\xED\xA0\x82", "Trailing UTF-16 high surrogate");
  return rc;
}
