#ifndef NU_STAGE_H
#define NU_STAGE_H

#include <nucleus/slotmap.h>
#include <nucleus/string.h>
#include <nucleus/types.h>

#define NU_MAX_STAGE_NODE_COUNT 8

typedef nu_u32_t nu_stage_t;

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_uid_t   uid;
    nu__slot_t first_node;
} nu__stage_entry_t;

typedef struct
{
    nu_u8_t    count;
    nu__slot_t systems[NU_MAX_STAGE_NODE_COUNT];
} nu__stage_node_entry_t;

#endif

#endif
