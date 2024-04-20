#ifndef NU_MEMORY_H
#define NU_MEMORY_H

#include <nucleus/platform.h>
#include <nucleus/types.h>

NU_API void *nu_memset(void *dst, nu_int_t c, nu_size_t n);
NU_API void  nu_memcpy(void *dst, const void *src, nu_size_t n);

#ifdef NU_IMPLEMENTATION

void *
nu_memset (void *dst, nu_int_t c, nu_size_t n)
{
    if (n != 0)
    {
        unsigned char *d = dst;
        do
        {
            *d++ = (unsigned char)c;
        } while (--n != 0);
    }
    return (dst);
}

void
nu_memcpy (void *dst, const void *src, nu_size_t n)
{
    nu_size_t      i;
    nu_u8_t       *u8_dst = dst;
    const nu_u8_t *u8_src = src;

    for (i = 0; i < n; ++i)
    {
        u8_dst[i] = u8_src[i];
    }
}

#endif

#endif
