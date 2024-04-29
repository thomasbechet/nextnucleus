#ifndef NU_TYPES_H
#define NU_TYPES_H

#include <nucleus/platform.h>

/**
 * This is not a complete implementation, each
 * compiler should define its own types to ensure
 * pedantry.
 */
typedef unsigned char  nu_u8_t;
typedef signed char    nu_i8_t;
typedef unsigned short nu_u16_t;
typedef signed short   nu_i16_t;
typedef unsigned int   nu_u32_t;
typedef signed int     nu_i32_t;

typedef int  nu_int_t;
typedef char nu_char_t;

typedef int           nu_bool_t;
typedef unsigned long nu_size_t;

typedef void (*nu_pfn_t)(void);

typedef enum
{
    NU_TYPE_BOOL,
    NU_TYPE_U8,
    NU_TYPE_I8,
    NU_TYPE_U16,
    NU_TYPE_I16,
    NU_TYPE_U32,
    NU_TYPE_I32
} nu_type_t;

NU_API nu_size_t nu_type_size(nu_type_t t);

#ifdef NU_IMPLEMENTATION

nu_size_t
nu_type_size (nu_type_t t)
{
    switch (t)
    {
        case NU_TYPE_BOOL:
            return 1;
        case NU_TYPE_U8:
            return 1;
        case NU_TYPE_I8:
            return 1;
        case NU_TYPE_U16:
            return 2;
        case NU_TYPE_I16:
            return 2;
        case NU_TYPE_U32:
            return 4;
        case NU_TYPE_I32:
            return 4;
        default:
            return 0;
    }
}

#endif

#endif
