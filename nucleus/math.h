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

NU_API float
nu_pow (float a, float b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}

NU_API float
nu_floor (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_ceil (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_exp (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_log (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_log10 (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_sqrt (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_fmod (float a, float b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}

NU_API float
nu_sin (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_cos (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_tan (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_acos (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_asin (float a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

NU_API float
nu_atan2 (float a, float b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}

#endif
