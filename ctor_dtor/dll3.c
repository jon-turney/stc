#include <stdio.h>

static void dllinit(void) __attribute__((constructor));
static void dllinit(void)
{
  printf("++++++++++++++\n");
}

static void dllquit(void) __attribute__((destructor));
static void dllquit(void)
{
  printf("--------------\n");
}

void func3()
{
  printf("oooooooooooooo\n");
}
