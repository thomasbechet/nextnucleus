#ifndef NU_COMPONENT_H
#define NU_COMPONENT_H

#include <nucleus/types.h>
#include <nucleus/macro.h>
#include <nucleus/string.h>

typedef struct nu__component *nu_component_t;

typedef enum
{
    NU_PROPERTY_SCALAR,
    NU_PROPERTY_ARRAY,
    NU_PROPERTY_SPATIAL
} nu_property_kind_t;

typedef struct
{
    const nu_char_t   *ident;
    nu_type_t          type;
    nu_property_kind_t kind;
} nu_property_info_t;

typedef struct
{
    const nu_char_t          *ident;
    const nu_property_info_t *properties;
} nu_component_info_t;

#ifdef NU_IMPLEMENTATION

struct nu__property
{
    nu_ident_t           ident;
    nu_type_t            type;
    nu_property_kind_t   kind;
    struct nu__property *next;
};

struct nu__component
{
    nu_ident_t           ident;
    struct nu__property *first_property;
};

/* static nu_size_t
nu__chunk_size (const struct nu__component *component, nu_size_t entry_count)
{
    struct nu__field *current = component->first_field;
    nu_size_t         size    = 0;
    while (current)
    {
        size += nu_type_size(current->type) * entry_count;
    }
    return size;
} */

#endif

#endif
