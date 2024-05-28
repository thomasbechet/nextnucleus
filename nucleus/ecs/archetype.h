#ifndef NU_ARCHETYPE_H
#define NU_ARCHETYPE_H

#include <nucleus/ecs/component.h>
#include <nucleus/ecs/entity.h>
#include <nucleus/slotmap.h>
#include <nucleus/string.h>

typedef struct
{
    const nu_char_t   *name;
    const nu_handle_t *components;
    nu_u16_t           component_count;
    nu_u16_t           entity_capacity;
} nu_archetype_info_t;

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_uid_t            uid;
    nu_component_type_t type;
    void               *data;
} nu__archetype_data_header_t;

typedef struct
{
    nu_uid_t                     uid;
    nu_u16_t                     component_count;
    nu_u16_t                     entity_capacity;
    nu_u16_t                     entity_count;
    nu_entity_t                 *entities;
    nu_u16_t                    *entity_to_index;
    nu__archetype_data_header_t *headers;
} nu__archetype_entry_t;

#define NU_ARCHETYPE_BITS_BLOCK_SIZE 32

static nu_handle_t
nu__archetype_handle (nu__slot_t slot)
{
    return slot;
}
/* static nu__slot_t
nu__archetype_slot (nu_handle_t handle)
{
    return handle;
} */

static nu_u32_t *
nu__archetype_bits_alloc (nu__allocator_t *alloc,
                          nu_u16_t         component_capacity,
                          nu_u16_t         archetype_capacity)
{
    nu_size_t i;
    nu_size_t block_per_archetype
        = (component_capacity + NU_ARCHETYPE_BITS_BLOCK_SIZE - 1)
          / NU_ARCHETYPE_BITS_BLOCK_SIZE;
    nu_size_t total_block = archetype_capacity * block_per_archetype;

    nu_u32_t *blocks
        = nu__alloc(alloc, sizeof(nu_u32_t) * total_block, NU_MEMORY_USAGE_ECS);

    for (i = 0; i < total_block; ++i)
    {
        blocks[i] = 0;
    }

    return blocks;
}

#endif

#endif
