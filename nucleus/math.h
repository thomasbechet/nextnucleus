#ifndef NU_MATH_H
#define NU_MATH_H

#include <nucleus/types.h>
#include <nucleus/platform.h>

#define NU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b) (((a) > (b)) ? (a) : (b))

NU_API nu_bool_t
nu_is_power_of_two (nu_size_t n)
{
    return (n & (n - 1)) == 0;
}

NU_API nu_size_t
nu_log2 (nu_size_t n)
{
    nu_size_t result = 0;
    n                = n >> 1;
    while (n != 0)
    {
        result++;
        n = n >> 1;
    }
    return result;
}

#endif
