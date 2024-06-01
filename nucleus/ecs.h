#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/allocator.h>
#include <nucleus/ecs/component.h>
#include <nucleus/ecs/entity.h>
#include <nucleus/ecs/group.h>
#include <nucleus/ecs/stage.h>
#include <nucleus/ecs/system.h>
#include <nucleus/ecs/archetype.h>
#include <nucleus/error.h>
#include <nucleus/list.h>
#include <nucleus/slotmap.h>
#include <nucleus/string.h>
#include <nucleus/types.h>

typedef struct
{
    nu_u16_t  component_capacity;
    nu_u16_t  archetype_capacity;
    nu_u16_t  group_capacity;
    nu_u16_t  system_capacity;
    nu_u16_t  stage_capacity;
    nu_u16_t  stage_node_capacity;
    nu_bool_t load_names;
} nu_ecs_info_t;

#define NU_DEFAULT_COMPONENT_CAPACITY  128
#define NU_DEFAULT_ARCHETYPE_CAPACITY  32
#define NU_DEFAULT_GROUP_CAPACITY      128
#define NU_DEFAULT_SYSTEM_CAPACITY     64
#define NU_DEFAULT_STAGE_CAPACITY      64
#define NU_DEFAULT_STAGE_NODE_CAPACITY 64
#define NU_DEFAULT_LOAD_NAMES          NU_TRUE

typedef struct
{
    nu__slotmap_t  components;
    nu__slotlist_t component_list;
    nu_ident_t    *component_names;
    nu__slot_t     first_component;

    nu__slotmap_t  archetypes;
    nu__slotlist_t archetype_list;
    nu__slotlist_t archetype_group_lists;
    nu_ident_t    *archetype_names;
    nu__slot_t     first_archetype;
    nu_u32_t      *archetype_bits;
    nu_u16_t       blocks_per_archetype;

    nu__slotmap_t groups;

    nu__slotmap_t  systems;
    nu__slotlist_t system_list;
    nu_ident_t    *system_names;
    nu__slot_t     first_system;

    nu__slotmap_t  stages;
    nu__slotlist_t stage_list;
    nu_ident_t    *stage_names;
    nu__slot_t     first_stage;

    nu__slotmap_t  stage_nodes;
    nu__slotlist_t stage_node_lists;

    nu_ecs_info_t info;
} nu__ecs_t;

NU_API void nu_ecs_info_default(nu_ecs_info_t *info);

#ifdef NU_IMPLEMENTATION

void
nu_ecs_info_default (nu_ecs_info_t *info)
{
    info->component_capacity  = NU_DEFAULT_COMPONENT_CAPACITY;
    info->archetype_capacity  = NU_DEFAULT_ARCHETYPE_CAPACITY;
    info->group_capacity      = NU_DEFAULT_GROUP_CAPACITY;
    info->system_capacity     = NU_DEFAULT_SYSTEM_CAPACITY;
    info->stage_capacity      = NU_DEFAULT_STAGE_CAPACITY;
    info->stage_node_capacity = NU_DEFAULT_STAGE_NODE_CAPACITY;
    info->load_names          = NU_DEFAULT_LOAD_NAMES;
}

static nu_error_t
nu__ecs_init (const nu_ecs_info_t *info, nu__allocator_t *alloc, nu__ecs_t *ecs)
{
    NU_ASSERT(info->component_capacity > 0);
    NU_ASSERT(info->archetype_capacity > 0);
    NU_ASSERT(info->group_capacity > 0);
    NU_ASSERT(info->system_capacity > 0);
    NU_ASSERT(info->stage_capacity > 0);

    /* components */
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__component_entry_t),
                     info->component_capacity,
                     &ecs->components);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->component_capacity,
                      &ecs->component_list);
    if (info->load_names)
    {
        ecs->component_names
            = nu__alloc(alloc,
                        sizeof(nu_ident_t) * info->component_capacity,
                        NU_MEMORY_USAGE_ECS);
    }
    ecs->first_component = NU_SLOT_NULL;

    /* archetypes */
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__archetype_entry_t)
                         + sizeof(nu__archetype_component_t)
                               * (info->component_capacity - 1),
                     info->archetype_capacity,
                     &ecs->archetypes);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->archetype_capacity,
                      &ecs->archetype_list);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->group_capacity,
                      &ecs->archetype_group_lists);
    if (info->load_names)
    {
        ecs->archetype_names
            = nu__alloc(alloc,
                        sizeof(nu_ident_t) * info->archetype_capacity,
                        NU_MEMORY_USAGE_ECS);
    }
    ecs->first_archetype = NU_SLOT_NULL;
    ecs->archetype_bits  = nu__archetype_bits_alloc(
        alloc, info->component_capacity, info->archetype_capacity);

    /* groups */
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__group_entry_t),
                     info->group_capacity,
                     &ecs->groups);

    /* systems */
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__system_entry_t),
                     info->system_capacity,
                     &ecs->systems);
    nu__slotlist_init(
        alloc, NU_MEMORY_USAGE_ECS, info->system_capacity, &ecs->system_list);
    if (info->load_names)
    {
        ecs->system_names
            = nu__alloc(alloc,
                        sizeof(nu_ident_t) * info->system_capacity,
                        NU_MEMORY_USAGE_ECS);
    }
    ecs->first_system = NU_SLOT_NULL;

    /* stages */
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__system_entry_t),
                     info->stage_capacity,
                     &ecs->stages);
    nu__slotlist_init(
        alloc, NU_MEMORY_USAGE_ECS, info->stage_capacity, &ecs->stage_list);
    if (info->load_names)
    {
        ecs->stage_names = nu__alloc(alloc,
                                     sizeof(nu_ident_t) * info->stage_capacity,
                                     NU_MEMORY_USAGE_ECS);
    }
    ecs->first_stage = NU_SLOT_NULL;

    /* system nodes */
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__stage_node_entry_t),
                     info->stage_node_capacity,
                     &ecs->stage_nodes);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->stage_node_capacity,
                      &ecs->stage_node_lists);

    ecs->info = *info;

    return NU_ERROR_NONE;
}

static nu__slot_t
nu__ecs_find_component (nu__ecs_t *ecs, nu_uid_t uid)
{
    nu__slot_t it = ecs->first_component;
    while (it)
    {
        nu__component_entry_t *entry = nu__slotmap_get(ecs->components, it);
        if (entry->uid == uid)
        {
            return it;
        }
        it = nu__slotlist_next(ecs->component_list, it);
    }
    return NU_SLOT_NULL;
}
static nu_error_t
nu__ecs_register_component (nu__ecs_t                 *ecs,
                            const nu_component_info_t *info,
                            nu__slot_t                *slot)
{
    nu__component_entry_t *entry;
    nu_uid_t               uid = nu_uid(info->name);

    /* find duplicated component */
    if (nu__ecs_find_component(ecs, uid))
    {
        return NU_ERROR_DUPLICATED_ENTRY;
    }

    /* insert component */
    *slot             = nu__slotmap_add(ecs->components);
    entry             = nu__slotmap_get(ecs->components, *slot);
    entry->uid        = uid;
    entry->array_size = info->size;
    entry->data_size  = info->size * nu_component_type_size(info->type);

    if (ecs->info.load_names)
    {
        nu_ident_set_str(ecs->component_names[nu_slot_index(*slot)],
                         info->name);
    }

    nu__slotlist_add_first(ecs->component_list, &ecs->first_component, *slot);

    return NU_ERROR_NONE;
}

static nu_error_t
nu__ecs_register_archetype (nu__ecs_t                 *ecs,
                            const nu_archetype_info_t *info,
                            nu__slot_t                *slot)
{
    nu_size_t              i;
    nu_u32_t              *blocks;
    nu__archetype_entry_t *entry;

    NU_ASSERT(info->component_count > 0);

    *slot                  = nu__slotmap_add(ecs->archetypes);
    entry                  = nu__slotmap_get(ecs->archetypes, *slot);
    entry->uid             = nu_uid(info->name);
    entry->component_count = info->component_count;
    entry->first_group     = NU_NULL;
    nu__slotlist_add_first(ecs->archetype_list, &ecs->first_archetype, *slot);

    blocks = &ecs->archetype_bits[ecs->blocks_per_archetype
                                  * nu_slot_index(*slot)];

    /* set bits */
    for (i = 0; i < ecs->blocks_per_archetype; ++i)
    {
        blocks[i] = 0;
    }
    for (i = 0; i < info->component_count; ++i)
    {
        nu_u16_t index = nu_slot_index(nu__component_slot(info->components[i]));
        nu_u16_t block_index  = index / NU_ARCHETYPE_BITS_BLOCK_SIZE;
        nu_u16_t block_offset = index % NU_ARCHETYPE_BITS_BLOCK_SIZE;
        if ((blocks[block_index] >> block_offset) & 1)
        {
            return NU_ERROR_DUPLICATED_ENTRY;
        }
        blocks[block_index] = blocks[block_index] | (1 << block_offset);
    }

    /* set name */
    if (ecs->info.load_names)
    {
        nu_ident_set_str(ecs->archetype_names[nu_slot_index(*slot)],
                         info->name);
    }

    /* setup components entries */
    entry->entry_size = 0;
    for (i = 0; i < info->component_count; ++i)
    {
        nu__archetype_component_t *comp;
        nu__slot_t             slot = nu__component_slot(info->components[i]);
        nu__component_entry_t *component
            = nu__slotmap_get(ecs->components, slot);
        NU_ASSERT(entry);
        comp            = &entry->components[nu_slot_index(slot)];
        comp->uid       = component->uid;
        comp->type      = component->type;
        comp->data_size = component->data_size;
        comp->offset    = entry->entry_size;
        entry->entry_size += comp->data_size;
    }

    return NU_ERROR_NONE;
}

static nu__slot_t
nu__ecs_find_system (nu__ecs_t *ecs, nu_uid_t uid)
{
    nu__slot_t it = ecs->first_system;
    while (it)
    {
        nu__system_entry_t *entry = nu__slotmap_get(ecs->systems, it);
        if (entry->uid == uid)
        {
            return it;
        }
        it = nu__slotlist_next(ecs->system_list, it);
    }
    return NU_SLOT_NULL;
}
static nu_error_t
nu__ecs_register_system (nu__ecs_t              *ecs,
                         const nu_system_info_t *info,
                         nu__slot_t             *slot)
{

    nu__system_entry_t *entry;
    nu_size_t           i;
    nu_uid_t            uid = nu_uid(info->name);

    /* find duplicated system */
    if (nu__ecs_find_system(ecs, uid))
    {
        return NU_ERROR_DUPLICATED_ENTRY;
    }

    /* insert system */
    *slot                  = nu__slotmap_add(ecs->systems);
    entry                  = nu__slotmap_get(ecs->systems, *slot);
    entry->uid             = uid;
    entry->callback        = info->callback;
    entry->component_count = 0;

    if (ecs->info.load_names)
    {
        nu_ident_set_str(ecs->system_names[nu_slot_index(*slot)], info->name);
    }

    nu__slotlist_add_first(ecs->system_list, &ecs->first_system, *slot);

    /* insert components */
    for (i = 0; i < info->component_count; ++i)
    {
        if (info->components[i].slot >= NU_SYSTEM_COMPONENT_CAPACITY)
        {
            return NU_ERROR_OUT_OF_SYSTEM_ITEM;
        }
        entry->component_count
            = NU_MAX(entry->component_count, info->components[i].slot + 1);
    }
    for (i = 0; i < info->component_count; ++i)
    {
        nu__system_component_t *component
            = &entry->components[info->components[i].slot];
        component->access = info->components[i].access;
        component->slot   = nu__component_slot(info->components[i].handle);
    }

    return NU_ERROR_NONE;
}

/* static nu__slot_t
nu__ecs_find_stage (nu__ecs_t *ecs, nu_uid_t uid)
{
    nu__slot_t it = ecs->first_stage;
    while (it)
    {
        nu__stage_entry_t *entry = nu__slotmap_get(ecs->stages, it);
        if (entry->uid == uid)
        {
            return it;
        }
        it = nu__slotlist_next(ecs->stage_list, it);
    }
    return NU_SLOT_NULL;
} */

static nu_entity_t
nu__ecs_spawn (nu__ecs_t *ecs, nu_handle_t group)
{
    nu__group_entry_t *entry
        = nu__slotmap_get(ecs->groups, nu__group_slot(group));
    nu_u16_t free = entry->free;

    nu_entity_t entity = nu__entity_build(nu__group_slot(group), free);

    if (entry->count == entry->capacity)
    {
        return NU_NULL;
    }

    entry->free                        = entry->free_reverse[free];
    entry->free_reverse[free]          = entry->count;
    entry->version_index[entry->count] = free;
    entry->count += 1;

    return entity;
}

static nu_handle_t
nu__ecs_create_group (nu__ecs_t       *ecs,
                      nu__allocator_t *alloc,
                      nu_handle_t      archetype,
                      nu_u16_t         capacity)
{
    nu__slot_t         slot;
    nu__group_entry_t *entry;
    nu_size_t          i;

    nu__archetype_entry_t *archetype_entry
        = nu__slotmap_get(ecs->archetypes, archetype);

    slot  = nu__slotmap_add(ecs->groups);
    entry = nu__slotmap_get(ecs->groups, slot);

    entry->capacity   = capacity;
    entry->archetype  = archetype;
    entry->count      = 0;
    entry->components = archetype_entry->components;
    /* TODO: custom allocator to allow dealloc */
    entry->data = nu__alloc(
        alloc, archetype_entry->entry_size * capacity, NU_MEMORY_USAGE_ECS);
    NU_ASSERT(entry->data);
    entry->version_index
        = nu__alloc(alloc, capacity * sizeof(nu_u32_t), NU_MEMORY_USAGE_ECS);
    NU_ASSERT(entry->version_index);
    entry->free_reverse
        = nu__alloc(alloc, sizeof(nu_u16_t), NU_MEMORY_USAGE_ECS);
    NU_ASSERT(entry->free_reverse);

    for (i = 0; i < capacity; ++i)
    {
        entry->free_reverse[i] = i + 1;
    }
    entry->free = 0;

    return slot;
}

static void *
nu__ecs_field (nu__ecs_t *ecs, nu_entity_t e, nu_handle_t c)
{
    nu_u16_t                   group, index;
    nu__group_entry_t         *entry;
    nu__archetype_component_t *component;

    group = nu__entity_group(e);
    index = nu__entity_index(e);
    entry = nu__slotmap_get(ecs->groups, group); /* 1 indirection */
    NU_ASSERT(entry);
    NU_ASSERT(c);
    component = &entry->components[nu_slot_index(c)]; /* 2 indirection */
    return (void *)((nu_size_t)entry->data + component->offset * entry->capacity
                    + component->data_size * index); /* 3 indirection */
}

#endif

#endif
