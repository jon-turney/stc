//
// exercise destructor ordering in dll directly and indirectly loaded
//
// See https://cygwin.com/pipermail/cygwin/2025-October/258919.html

extern void func2();
int main()
{
  func2();
  return 0;
}
