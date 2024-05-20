#ifndef NU_COMPONENT_H
#define NU_COMPONENT_H

#include <nucleus/slotmap.h>
#include <nucleus/types.h>
#include <nucleus/string.h>

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

typedef struct
{
    nu__slot_t slot;
} nu_property_t;

typedef struct
{
    nu__slot_t slot;
} nu_component_t;

typedef struct
{
    nu_ident_t         name;
    nu_type_t          type;
    nu_property_kind_t kind;
} nu__property_entry_t;

typedef struct
{
    nu_ident_t name;
    nu__slot_t first_property;
} nu__component_entry_t;

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
