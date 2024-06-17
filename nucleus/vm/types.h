#ifndef NU_TYPES_H
#define NU_TYPES_H

#include <nucleus/vm/platform.h>

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
typedef unsigned long  nu_u64_t;
typedef signed long    nu_i64_t;

#define NU_U16_MAX 0xFFFF

typedef int  nu_int_t;
typedef char nu_char_t;

typedef int           nu_bool_t;
typedef unsigned long nu_size_t;

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

typedef nu_u32_t nu_handle_t;

typedef void (*nu_pfn_t)(void);

typedef enum
{
    NU_TYPE_BOOL,
    NU_TYPE_INT,
    NU_TYPE_FLOAT,
    NU_TYPE_UV2,
    NU_TYPE_IV2,
    NU_TYPE_FV2,
    NU_TYPE_UV3,
    NU_TYPE_IV3,
    NU_TYPE_FV3,
    NU_TYPE_ENTITY,
    NU_TYPE_QUAT,
    NU_TYPE_TEXTURE,
    NU_TYPE_MESH,
    NU_TYPE_ANIMATION
} nu_type_t;

#endif
