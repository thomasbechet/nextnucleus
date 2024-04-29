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

struct nu__field
{
    nu_ident_t        ident;
    nu_type_t         type;
    struct nu__field *next;
    struct nu__field *prev;
};

struct nu__component
{
    nu_ident_t        ident;
    struct nu__field *first_field;
};

static nu_size_t
nu__chunk_size (const struct nu__component *component, nu_size_t entry_count)
{
    struct nu__field *current = component->first_field;
    nu_size_t         size    = 0;
    while (current)
    {
        size += nu_type_size(current->type) * entry_count;
    }
    return size;
}

#endif

#endif
