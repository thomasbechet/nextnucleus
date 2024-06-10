#ifndef NU_ARCHETYPE_H
#define NU_ARCHETYPE_H

#include <nucleus/ecs/entity.h>
#include <nucleus/slotmap.h>
#include <nucleus/string.h>

typedef nu_u32_t nu_archetype_t;
typedef nu_u32_t nu_field_t;

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
    NU_TYPE_ARCHETYPE,
    NU_TYPE_FIELD,
    NU_TYPE_GROUP,
    NU_TYPE_SYSTEM,
    NU_TYPE_TEXTURE,
    NU_TYPE_MESH,
    NU_TYPE_ANIMATION
} nu_field_type_t;

typedef struct
{
    const nu_char_t *name;
    nu_field_type_t  type;
    nu_u16_t         size;
} nu_field_info_t;

typedef struct
{
    const nu_char_t       *name;
    const nu_field_info_t *fields;
    nu_u16_t               field_count;
} nu_archetype_info_t;

NU_API nu_size_t nu_field_type_size(nu_field_type_t t);

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_uid_t        uid;
    nu_field_type_t type;
    nu_u16_t        data_size;
    nu_u16_t        array_size;
} nu__field_entry_t;

typedef struct
{
    nu_uid_t           uid;
    nu_u16_t           field_count;
    nu_size_t          entry_size;
    nu__slot_t         first_group;
    nu__field_entry_t *fields;
} nu__archetype_entry_t;

#define NU_ARCHETYPE_BITS_BLOCK_SIZE 32

nu_size_t
nu_field_type_size (nu_field_type_t t)
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

static nu_u8_t
nu__field_index (nu_field_t field)
{
    return field >> 16;
}
static nu_u16_t
nu__field_offset (nu_field_t field)
{
    return field & 0xFFFF;
}
static nu_field_t
nu__field_build (nu_u8_t index, nu_u16_t offset)
{
    return ((nu_u32_t)index << 16) | (nu_u32_t)offset;
}

static nu_handle_t
nu__archetype_handle (nu__slot_t slot)
{
    return slot;
}
static nu__slot_t
nu__archetype_slot (nu_handle_t handle)
{
    return handle;
}

#endif

#endif
