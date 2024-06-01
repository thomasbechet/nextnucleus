#ifndef NU_ENTITY_H
#define NU_ENTITY_H

#include <nucleus/slotmap.h>
#include <nucleus/types.h>

typedef nu_u32_t nu_entity_t;

#ifdef NU_IMPLEMENTATION

static nu_u16_t
nu__entity_index (nu_entity_t e)
{
    return e & 0xFFFF;
}

static nu_u16_t
nu__entity_group (nu_entity_t e)
{
    return (e >> 16) & 0xFFFF;
}

static nu_entity_t
nu__entity_build (nu__slot_t group, nu_u16_t index)
{
    return ((nu_u32_t)group << 16) | index;
}

#endif

#endif
