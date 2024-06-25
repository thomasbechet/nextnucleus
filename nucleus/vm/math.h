#ifndef NU_MATH_H
#define NU_MATH_H

#include <nucleus/vm/types.h>
#include <nucleus/vm/platform.h>
#include <nucleus/vm/error.h>

#define NU_F32_FRAC  16
#define NU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef nu_i32_t nu_f32_t;

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API nu_f32_t  nu_pow(nu_f32_t a, nu_f32_t b);
NU_API nu_f32_t  nu_floor(nu_f32_t a);
NU_API nu_f32_t  nu_ceil(nu_f32_t a);
NU_API nu_f32_t  nu_exp(nu_f32_t a);
NU_API nu_f32_t  nu_log(nu_f32_t a);
NU_API nu_f32_t  nu_log10(nu_f32_t a);
NU_API nu_f32_t  nu_sqrt(nu_f32_t a);
NU_API nu_f32_t  nu_fmod(nu_f32_t a, nu_f32_t b);
NU_API nu_f32_t  nu_sin(nu_f32_t a);
NU_API nu_f32_t  nu_cos(nu_f32_t a);
NU_API nu_f32_t  nu_tan(nu_f32_t a);
NU_API nu_f32_t  nu_acos(nu_f32_t a);
NU_API nu_f32_t  nu_asin(nu_f32_t a);
NU_API nu_f32_t  nu_atan2(nu_f32_t a, nu_f32_t b);

NU_API nu_f32_t   nu_fadd(nu_f32_t a, nu_f32_t b);
NU_API nu_f32_t   nu_fsub(nu_f32_t a, nu_f32_t b);
NU_API nu_f32_t   nu_fmul(nu_f32_t a, nu_f32_t b);
NU_API nu_f32_t   nu_fdiv(nu_f32_t a, nu_f32_t b);
NU_API nu_f32_t   nu_itof(nu_i32_t a);
NU_API nu_i32_t   nu_ftoi(nu_f32_t a);
NU_API float      nu_ftofloat(nu_f32_t a);
NU_API double     nu_ftodouble(nu_f32_t a);
NU_API nu_error_t nu_fparse(const nu_char_t *s, nu_size_t n, nu_f32_t *v);
NU_API nu_error_t nu_iparse(const nu_char_t *s, nu_size_t n, nu_i32_t *v);

#ifdef NU_IMPL

nu_bool_t
nu_is_power_of_two (nu_size_t n)
{
    return (n & (n - 1)) == 0;
}
nu_size_t
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
nu_f32_t
nu_pow (nu_f32_t a, nu_f32_t b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_floor (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_ceil (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

nu_f32_t
nu_exp (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_log (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_log10 (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_sqrt (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_fmod (nu_f32_t a, nu_f32_t b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_sin (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

nu_f32_t
nu_cos (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_tan (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_acos (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_asin (nu_f32_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_f32_t
nu_atan2 (nu_f32_t a, nu_f32_t b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}

nu_f32_t
nu_fadd (nu_f32_t a, nu_f32_t b)
{
    return a + b;
}
nu_f32_t
nu_fsub (nu_f32_t a, nu_f32_t b)
{
    return a - b;
}
nu_f32_t
nu_fmul (nu_f32_t a, nu_f32_t b)
{
    return (nu_f32_t)(((nu_u64_t)a * (nu_u64_t)b) >> NU_F32_FRAC);
}
nu_f32_t
nu_fdiv (nu_f32_t a, nu_f32_t b)
{
    return (nu_f32_t)(((nu_i64_t)a << NU_F32_FRAC) / (nu_i64_t)b);
}
nu_f32_t
nu_itof (nu_i32_t a)
{
    return (nu_f32_t)(a << NU_F32_FRAC);
}
nu_i32_t
nu_ftoi (nu_f32_t a)
{
    return (nu_i32_t)(a >> NU_F32_FRAC);
}
float
nu_ftofloat (nu_f32_t a)
{
    return (float)a / (float)(1 << NU_F32_FRAC);
}
double
nu_ftodouble (nu_f32_t a)
{
    return (double)a / (double)(1 << NU_F32_FRAC);
}
static nu_u32_t
nu__fast_pow10 (nu_u32_t n)
{
    static nu_u32_t pow10[6] = { 1, 10, 100, 1000, 10000, 100000 };
    return pow10[n];
}
nu_error_t
nu_fparse (const nu_char_t *s, nu_size_t n, nu_f32_t *v)
{
    nu_u32_t  i, fixed, base;
    nu_bool_t sign = NU_FALSE, frac_flag = NU_FALSE;
    nu_u32_t  inte = 0, frac = 0, dp = 0;
    /* parse integer part, fractional part and decimal point */
    for (i = 0; i < n; ++i)
    {
        nu_char_t c = s[i];
        switch (c)
        {
            case '-':
                if (i != 0)
                {
                    return NU_ERROR_INVALID_STATE;
                }
                sign = NU_TRUE;
                break;
            case '.':
                frac_flag = NU_TRUE;
                break;
            default:
                if (c > '9' || c < '0')
                {
                    return NU_ERROR_INVALID_STATE;
                }
                if (frac_flag)
                {
                    if (dp >= 8)
                    {
                        return NU_ERROR_INVALID_STATE;
                    }
                    frac = frac * 10 + ((nu_u32_t)c - (nu_u32_t)'0');
                    dp++;
                }
                else
                {
                    inte = inte * 10 + ((nu_u32_t)c - (nu_u32_t)'0');
                }
                break;
        }
    }
    /* build fixed point float */
    fixed = inte << NU_F32_FRAC;
    base  = nu__fast_pow10(dp);
    i     = 0;
    while (i < NU_F32_FRAC)
    {
        frac <<= 1;
        if (frac >= base)
        {
            fixed |= 1 << (NU_F32_FRAC - 1 - i);
            frac -= base;
        }
        i++;
    }
    if (sign)
    {
        fixed = !fixed + 1;
    }
    *v = (nu_f32_t)fixed;
    return NU_ERROR_NONE;
}
nu_error_t
nu_iparse (const nu_char_t *s, nu_size_t n, nu_i32_t *v)
{
    nu_u32_t  i;
    nu_bool_t sign  = NU_FALSE;
    nu_i32_t  value = 0;
    /* parse integer part */
    for (i = 0; i < n; ++i)
    {
        nu_char_t c = s[i];
        switch (c)
        {
            case '-':
                if (i != 0)
                {
                    return NU_ERROR_INVALID_STATE;
                }
                sign = NU_TRUE;
                break;
            default:
                if (c > '9' || c < '0')
                {
                    return NU_ERROR_INVALID_STATE;
                }
                value = value * 10 + ((nu_u32_t)c - (nu_u32_t)'0');
                break;
        }
    }
    if (sign)
    {
        value = -value;
    }
    *v = value;
    return NU_ERROR_NONE;
}

#endif

#endif
