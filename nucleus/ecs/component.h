#ifndef NU_COMPONENT_H
#define NU_COMPONENT_H

#include <nucleus/types.h>
#include <nucleus/macro.h>
#include <nucleus/string.h>

typedef struct nu__component *nu_component_t;

typedef struct
{
    const nu_char_t *ident;
    nu_size_t        size;
} nu_component_info_t;

#ifdef NU_IMPLEMENTATION

typedef nu_u16_t nu__component_id_t;

typedef enum
{
    NU_FIELD_BINARY,
    NU_FIELD_U32
} nu__field_type_t;

struct nu__field
{
    nu_ident_t        ident;
    nu__field_type_t  type;
    struct nu__field *next;
    struct nu__field *prev;
};

struct nu__component
{
    nu_ident_t        ident;
    nu_size_t         size;
    struct nu__field *first_field;
};

#endif

#endif
