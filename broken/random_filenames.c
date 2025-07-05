// https://cygwin.com/pipermail/cygwin/2025-June/258378.html

// This test creates files with random names and checks whether
// - readdir() returns the same path, or
// - at least the returned path is access()ible, and
// - unlink() of the original path works.

// With Cygwin 3.7.0-0.192.gc85f42a4f3ee.x86_64 and no arguments,
// two errors are reported:
/*
$'t-\xEF\x82\xB9': access() failed, errno=2:
$'t-\xB9\xED\xAB\x9E': original path
L"t-\xF0B9\xDADE": Windows path

$'t-\xEF\x82\xA9\x0F': access() failed, errno=2:
$'t-\xA9\x0F\xED\xA8\xB9': original path
L"t-\xF0A9\xF00F\xDA39": Windows path
*/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <windows.h>

static void print_c(FILE * f, const char * s)
{
  fputs("$'", f);
  char c;
  for (int i = 0; (c = s[i]); i++) {
    if (c == '\'')
      fputs("'\\'$'", f);
    else if (' ' <= c && c <= '~')
      fputc(c, f);
    else
      fprintf(f, "\\x%02X", c & 0xff);
  }
  fputc('\'', f);
}

static void print_w(FILE * f, const wchar_t * s)
{
  fputs("L\"", f);
  wchar_t c;
  for (int i = 0; (c = s[i]); i++) {
    if (c == L'"' || c == L'\\')
      fprintf(f, "\\%c", c);
    else if (L' ' <= c && c <= L'~')
      fputc(c, f);
    else
      fprintf(f, "\\x%04X", c & 0xffff);
  }
  fputc('"', f);
}

static void get_winname(wchar_t * name)
{
  WIN32_FIND_DATAW e;
  HANDLE h = FindFirstFileW(L"*", &e);
  if (h == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "FindFirstFileW(): Error=%u\n", GetLastError());
    exit(1);
  }
  int i = 0;
  do {
    if (!wcscmp(e.cFileName, L".") || !wcscmp(e.cFileName, L".."))
      continue;
    wcscpy(name, e.cFileName);
    i++;
  } while (FindNextFileW(h, &e));
  FindClose(h);
  if (i != 1) {
    fprintf(stderr, "Error: %d Win32 files found\n", i);
    exit(1);
  }
}

static void get_cygname(char * name)
{
  DIR * d = opendir(".");
  if (!d) {
    perror("opendir");
    exit(1);
  }
  int i = 0;
  const struct dirent * e;
  while ((e = readdir(d))) {
    if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
      continue;
    strcpy(name, e->d_name);
    i++;
  }
  closedir(d);
  if (i != 1) {
    fprintf(stderr, "Error: %d Cygwin files found\n", i);
    exit(1);
  }
}

static void randname(char * name, int maxlen)
{
  int len = 1 + rand() % (maxlen + 1 - 1);
  for (int i = 0; i < len; i++) {
    char c = 1 + rand() % (256 - 2 - 1);
    if (c >= '/')
      c++;
    if (c >= '\\')
      c++;
    name[i] = c;
  }
  name[len] = 0;
}

static int testname(const char * name)
{
  int fd = open(name, O_WRONLY|O_CREAT, 0644);
  if (fd < 0) {
    print_c(stdout, name); printf(": open() failed, errno=%d\n", errno);
    exit(1);
  }
  close(fd);

  char cygname[MAX_PATH];
  get_cygname(cygname);
  wchar_t winname[MAX_PATH];
  get_winname(winname);

  int rc = 1;
  if (access(cygname, 0)) {
    print_c(stdout, cygname); printf(": access() failed, errno=%d:\n", errno);
    print_c(stdout, name); printf(": original path\n");
    print_w(stdout, winname); printf(": Windows path\n\n");
    rc = 0;
  }

  if (unlink(name)) {
    print_c(stdout, name); printf(": unlink() failed, errno=%d\n", errno);
    print_w(stdout, winname); printf(": Windows path\n");
    exit(1);
  }
  return rc;
}

int main(int argc, char **argv)
{
  // Usage: random_filenames [COUNT [SEED]]
  int n = (argc > 1 ? atoi(argv[1]) : 10000);
  srand(argc > 2 ? atoi(argv[2]) : 2);

  const char * dir = "test.tmp";
  rmdir(dir);
  if (mkdir(dir, 0755)) {
    perror(dir); return 1;
  }
  if (chdir(dir)) {
    perror(dir); return 1;
  }

  int errs = 0;
  for (int i = 0; i < n; i++) {
    char name[8] = "t-";
    randname(name + 2, sizeof(name) - 1 - 2);
    if (!testname(name) && ++errs >= 10)
      break;
    // Overwrite internal path buffer to check proper NUL termination
    // https://cygwin.com/pipermail/cygwin/2025-June/258363.html
    access("1234567890", 0);
  }
  return (!errs ? 0 : 1);
}
