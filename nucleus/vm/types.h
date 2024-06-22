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

#define NU_FOREACH_TYPE(TYPE) \
    TYPE(BOOL)                \
    TYPE(INT)                 \
    TYPE(FIX)                 \
    TYPE(IV2)                 \
    TYPE(IV3)                 \
    TYPE(IV4)                 \
    TYPE(FV2)                 \
    TYPE(FV3)                 \
    TYPE(FV4)                 \
    TYPE(ENTITY)              \
    TYPE(QUAT)                \
    TYPE(TEXTURE)             \
    TYPE(MESH)                \
    TYPE(ANIMATION)
#define NU_GENERATE_TYPE_ENUM(TYPE) NU_TYPE_##TYPE,
#define NU_GENERATE_TYPE_NAME(TYPE) #TYPE,
typedef enum
{
    NU_FOREACH_TYPE(NU_GENERATE_TYPE_ENUM) NU_TYPE_UNKNOWN
} nu_type_t;
const nu_char_t *NU_TYPE_NAMES[]
    = { NU_FOREACH_TYPE(NU_GENERATE_TYPE_NAME) "UNKNOWN" };

#define NU_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#endif
