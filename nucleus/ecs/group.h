#ifndef NU_GROUP_H
#define NU_GROUP_H

#include <nucleus/ecs/archetype.h>
#include <nucleus/slotmap.h>

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu__slot_t             archetype;
    nu_u16_t               count;
    nu_u16_t               capacity;
    nu_u32_t              *version_index;
    nu_u16_t              *free_reverse;
    nu_u16_t               free;
    nu__archetype_field_t *components;
    void                  *data;
} nu__group_entry_t;

static nu__slot_t
nu__group_slot (nu_handle_t handle)
{
    return handle;
}

#endif

#endif
