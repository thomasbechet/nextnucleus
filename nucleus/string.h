#ifndef NU_STRING_H
#define NU_STRING_H

#include <nucleus/types.h>
#include <nucleus/platform.h>
#include <nucleus/memory.h>

#define NU_IDENT_MAX  30
#define NU_IDENT_SIZE (NU_IDENT_MAX + 2)

typedef nu_u8_t nu_ident_t[NU_IDENT_SIZE];

NU_API nu_size_t nu_ident_len(const nu_ident_t ident);
NU_API void      nu_ident_empty(nu_ident_t ident);
NU_API void      nu_ident_set(nu_ident_t ident, const nu_u8_t *s, nu_size_t n);

#ifdef NU_IMPLEMENTATION

/*
 * Change the ident size. len must be in [0, NU_IDENT_MAX[ range.
 * If the string is expanded, filled data is undefined.
 */
static inline void
nu__ident_set_len (nu_ident_t ident, nu_size_t len)
{
    ident[NU_IDENT_MAX + 1] = (nu_u8_t)len;
    nu_memset(ident + len, 0, NU_IDENT_MAX - len);
}

nu_size_t
nu_ident_len (const nu_ident_t ident)
{
    return (nu_size_t)(*(ident + NU_IDENT_MAX + 1));
}

void
nu_ident_empty (nu_ident_t ident)
{
    nu_memset(ident, 0, NU_IDENT_SIZE);
    /* len implicitly set by memset */
}

void
nu_ident_set (nu_ident_t ident, const nu_u8_t *s, nu_size_t n)
{
    n = NU_MIN(NU_IDENT_SIZE, n);
    nu__ident_set_len(ident, n);
    nu_memcpy(ident, s, n);
}

#endif

#endif
