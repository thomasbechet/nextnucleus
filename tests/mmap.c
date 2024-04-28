#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void)
{
    int i;
    unsigned char *p = malloc(1024*1024*1024);
    assert(p);
    for (i = 0; i < 512 * 1024; ++i)
    {
        p[i * 1024] = 'A';
    }
    getchar();
    return 0;
}
