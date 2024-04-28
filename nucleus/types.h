#ifndef NU_TYPES_H
#define NU_TYPES_H

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

typedef int nu_int_t;
typedef char nu_char_t;

typedef int           nu_bool_t;
typedef unsigned long nu_size_t;

typedef void (*nu_pfn_t)(void);

typedef enum
{
    NU_TYPE_U8,
    NU_TYPE_I8,
    NU_TYPE_U16,
    NU_TYPE_I16,
    NU_TYPE_U32,
    NU_TYPE_I32,
    NU_TYPE_BOOL
} nu_type_t;

#endif
