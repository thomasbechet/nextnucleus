#ifndef NU_COMPONENT_H
#define NU_COMPONENT_H

#include <nucleus/slotmap.h>
#include <nucleus/types.h>
#include <nucleus/string.h>

typedef enum
{
    NU_TYPE_BOOL,
    NU_TYPE_U8,
    NU_TYPE_I8,
    NU_TYPE_U16,
    NU_TYPE_I16,
    NU_TYPE_U32,
    NU_TYPE_I32,
    NU_TYPE_UV2,
    NU_TYPE_IV2,
    NU_TYPE_FV2,
    NU_TYPE_UV3,
    NU_TYPE_IV3,
    NU_TYPE_FV3,
    NU_TYPE_ENTITY,
    NU_TYPE_QUAT,
    NU_TYPE_COMPONENT,
    NU_TYPE_PROPERTY,
    NU_TYPE_TEXTURE,
    NU_TYPE_MESH,
    NU_TYPE_ANIMATION
} nu_component_type_t;

typedef struct
{
    const nu_char_t    *name;
    nu_component_type_t type;
    nu_u16_t            size;
} nu_component_info_t;

NU_API nu_size_t nu_component_type_size(nu_component_type_t t);

#ifdef NU_IMPLEMENTATION

nu_size_t
nu_component_type_size (nu_component_type_t t)
{
    switch (t)
    {
        case NU_TYPE_FV3:
            return 4 * 3;
        case NU_TYPE_QUAT:
            return 4 * 4;
        default:
            return 0;
    }
}

#endif

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_uid_t            uid;
    nu_component_type_t type;
    nu_u16_t            size;
} nu__component_entry_t;

static nu_handle_t
nu__component_handle (nu__slot_t slot)
{
    return slot;
}
static nu__slot_t
nu__component_slot (nu_handle_t handle)
{
    return handle;
}

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
