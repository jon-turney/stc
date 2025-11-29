#include <stdio.h>
#include <dlfcn.h>

static void *dll3;
static void dllinit(void) __attribute__((constructor));
static void dllinit(void)
{
  printf("+++++++++++++++++++++++++++++\n");
  dll3 = dlopen("dll3.dll", RTLD_LOCAL|RTLD_NOW);
}

static void dllquit(void) __attribute__((destructor));
static void dllquit(void)
{
  printf("-----------------------------\n");
  dlclose(dll3);
}

void func2()
{
}
