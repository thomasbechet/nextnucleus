#ifndef NU_COMPONENT_H
#define NU_COMPONENT_H

#include <nucleus/types.h>
#include <nucleus/string.h>

typedef struct nu__component *nu_component_t;
typedef struct nu__property  *nu_property_t;

typedef enum
{
    NU_PROPERTY_SCALAR,
    NU_PROPERTY_ARRAY,
    NU_PROPERTY_SPATIAL
} nu_property_kind_t;

typedef struct
{
    const nu_char_t   *name;
    nu_type_t          type;
    nu_property_kind_t kind;
    nu_size_t          size;
} nu_property_info_t;

typedef struct
{
    const nu_char_t *name;
} nu_component_info_t;

struct nu__property
{
    nu_ident_t           name;
    nu_type_t            type;
    nu_property_kind_t   kind;
    struct nu__property *next;
};

struct nu__component
{
    nu_ident_t           name;
    struct nu__property *first_property;
};

#ifdef NU_IMPLEMENTATION

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
