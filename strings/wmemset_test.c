// https://cygwin.com/pipermail/cygwin/2022-December/252699.html

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    char dest[50] = { 0 };
    wchar_t src[] = L"example";
    wmemset(src, L'E', 1);
    if (wcstombs(dest, src, 7) == -1) {
        printf("FAILED\n");
        return 1;
    }
    printf("OK: %s\n", dest);
    return 0;
}
