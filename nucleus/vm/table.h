#ifndef NU_TABLE_H
#define NU_TABLE_H

#include <nucleus/vm/allocator.h>
#include <nucleus/vm/error.h>
#include <nucleus/vm/string.h>
#include <nucleus/vm/types.h>

#define NU_TABLE_CHUNK_SIZE         4096      /* 4096 */
#define NU_TABLE_CHUNK_MAX_CAPACITY (1 >> 10) /* 1024 */
#define NU_TABLE_FIELD_MAX_SIZE     (1 >> 12) /* 4096 */
#define NU_FIELD_SCALE              2         /* multiple of 4 */
#define NU_TABLE_NONE               0xffff

/*
 * Entity Bits Layout
 * 16 : chunk
 * 16 : index
 * Field Bits Layout
 * 10 : offset
 * 10 : capacity
 * 10 : size
 */

typedef nu_u32_t nu_field_t;
typedef nu_u32_t nu_entity_t;
typedef nu_u32_t nu_table_t;

typedef struct
{
    const nu_char_t *name;
    nu_type_t        type;
    nu_u16_t         count;
} nu_field_info_t;

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_u16_t capacity;
    nu_u16_t entry_size;
    nu_u16_t first_chunk;
    nu_u16_t first_field;
    nu_u16_t field_count;
    nu_u16_t next;
} nu__table_entry_t;

typedef struct
{
    nu_ident_t name;
    nu_type_t  type;
    nu_u16_t   count;
} nu__field_entry_t;

typedef struct
{
    nu_u16_t next;
    nu_u16_t free;
} nu__chunk_entry_t;

typedef struct
{
    nu_u16_t           table_capacity;
    nu_u16_t           field_capacity;
    nu_u16_t           chunk_capacity;
    nu_u16_t           table_count;
    nu_u16_t           field_count;
    nu_u16_t           first_table;
    nu_u16_t           free_chunk;
    nu__table_entry_t *tables;
    nu__field_entry_t *fields;
    nu__chunk_entry_t *chunks;
    void              *data;
} nu__table_manager_t;

static nu_u16_t
nu__entity_index (nu_entity_t e)
{
    return (e - 1) & 0xffff;
}

static nu_u16_t
nu__entity_chunk (nu_entity_t e)
{
    return (e - 1) >> 16;
}

static nu_entity_t
nu__entity_build (nu_u16_t chunk, nu_u16_t index)
{
    return (((nu_u32_t)chunk << 16) | index) + 1;
}

static nu_u16_t
nu__field_offset (nu_field_t f)
{
    return (f & 0x3ff) << NU_FIELD_SCALE;
}

static nu_u16_t
nu__field_capacity (nu_field_t f)
{
    return ((f >> 10) & 0x3ff) << NU_FIELD_SCALE;
}

static nu_u16_t
nu__field_size (nu_field_t f)
{
    return ((f >> 20) & 0x3ff) << NU_FIELD_SCALE;
}

/* static nu_field_t
nu__field_build (nu_u16_t offset, nu_u16_t capacity, nu_u16_t size)
{
    nu_u32_t so = offset >> NU_FIELD_SCALE;
    nu_u32_t sc = capacity >> NU_FIELD_SCALE;
    nu_u32_t ss = size >> NU_FIELD_SCALE;

    NU_ASSERT((offset & 0x3) == 0);
    NU_ASSERT((capacity & 0x3) == 0);
    NU_ASSERT((size & 0x3) == 0);

    return so << 20 | sc << 10 | ss;
} */

static void *
nu__table_field (nu__table_manager_t *manager, nu_entity_t e, nu_field_t f)
{
    nu_u16_t chunk    = nu__entity_chunk(e);
    nu_u16_t index    = nu__entity_index(e);
    nu_u16_t offset   = nu__field_offset(f);
    nu_u16_t capacity = nu__field_capacity(f);
    nu_u16_t size     = nu__field_size(f);
    return (void *)((nu_size_t)manager->data + chunk * NU_TABLE_CHUNK_SIZE
                    + capacity * offset + index * size);
}

static nu_size_t
nu__field_type_size (nu_type_t t)
{
    switch (t)
    {
        case NU_TYPE_FV3:
            return 4 * 3;
        case NU_TYPE_QUAT:
            return 4 * 4;
        default:
            return 4;
    }
}

static nu_u16_t
nu__table_entry_size (const nu_field_info_t *fields, nu_u16_t field_count)
{
    nu_size_t i, entry_size = 0;
    for (i = 0; i < field_count; ++i)
    {
        entry_size += nu__field_type_size(fields[i].type) * fields[i].count;
    }
    return entry_size;
}
static nu_u16_t
nu__table_entry_capacity (const nu_field_info_t *fields, nu_u16_t field_count)
{
    nu_size_t entry_size;

    entry_size = nu__table_entry_size(fields, field_count);
    entry_size += sizeof(nu_entity_t);
    entry_size += sizeof(nu_u16_t);

    return NU_TABLE_CHUNK_SIZE / entry_size;
}

static nu_error_t
nu__table_create (nu__table_manager_t   *manager,
                  const nu_char_t       *name,
                  const nu_field_info_t *fields,
                  nu_u16_t               field_count,
                  nu_table_t            *table)
{
    nu_table_t         handle;
    nu_u16_t           first_field;
    nu__table_entry_t *entry;
    nu_size_t          i;

    (void)name;

    *table = NU_NULL;

    /* check capacity */
    if (manager->table_count + 1 > manager->table_capacity
        || manager->field_count + field_count > manager->field_capacity)
    {
        return NU_ERROR_OUT_OF_MEMORY;
    }

    /* insert fields */
    /* TODO: check duplicated entries */
    first_field = manager->field_count;
    for (i = 0; i < field_count; ++i)
    {
        nu_ident_set_str(manager->fields[manager->field_count].name,
                         fields[i].name);
        manager->fields[manager->field_count].type  = fields[i].type;
        manager->fields[manager->field_count].count = fields[i].count;
        manager->field_count++;
    }

    /* insert table */
    handle             = manager->table_count;
    entry              = &manager->tables[handle];
    entry->first_chunk = NU_TABLE_NONE;
    entry->entry_size  = nu__table_entry_size(fields, field_count);
    entry->capacity    = nu__table_entry_capacity(fields, field_count);
    entry->first_field = first_field;
    entry->field_count = field_count;
    entry->next        = manager->first_table;

    manager->first_table = handle;
    manager->table_count++;

    *table = handle;

    return NU_ERROR_NONE;
}

static nu_u16_t
nu__table_find_chunk (nu__chunk_entry_t *chunks, nu_u16_t first_chunk)
{
    nu_u16_t current = first_chunk;
    while (current != NU_TABLE_NONE)
    {
        if (chunks[current].free)
        {
            return current;
        }
        current = chunks[current].next;
    }
    return NU_TABLE_NONE;
}

static nu_entity_t *
nu__chunk_entities (nu__table_manager_t     *manager,
                    const nu__table_entry_t *table,
                    nu_u16_t                 chunk)
{
    return (nu_entity_t *)((nu_size_t)manager->data
                           + chunk * NU_TABLE_CHUNK_SIZE
                           + table->entry_size * table->capacity);
}
static nu_u16_t *
nu__chunk_indices (nu__table_manager_t     *manager,
                   const nu__table_entry_t *table,
                   nu_u16_t                 chunk)
{
    return (nu_u16_t *)((nu_size_t)manager->data + chunk * NU_TABLE_CHUNK_SIZE
                        + (table->entry_size + sizeof(nu_entity_t))
                              * table->capacity);
}

static nu_entity_t
nu__table_spawn (nu__table_manager_t *manager, nu_table_t table)
{
    nu__chunk_entry_t *chunk_entry;
    nu_u16_t           entity_count, index;
    nu_entity_t        entity;
    nu_u16_t          *indices;
    nu_entity_t       *entities;

    nu__table_entry_t *entry = &manager->tables[table];
    nu_u16_t chunk = nu__table_find_chunk(manager->chunks, entry->first_chunk);

    if (chunk != NU_TABLE_NONE)
    {
        chunk_entry = &manager->chunks[chunk];
    }
    else
    {
        /* create a new chunk */
        /* TODO: check out of chunk */
        NU_ASSERT(manager->free_chunk != NU_TABLE_NONE);
        chunk               = manager->free_chunk;
        chunk_entry         = &manager->chunks[chunk];
        manager->free_chunk = chunk_entry->next;
        chunk_entry->free   = entry->capacity;
    }
    indices  = nu__chunk_indices(manager, entry, chunk);
    entities = nu__chunk_entities(manager, entry, chunk);

    entity_count = entry->capacity - chunk_entry->free;
    chunk_entry->free--;

    index = entities[entity_count]; /* interpret entity as free index */
    indices[index] = entity_count;
    entity         = nu__entity_build(chunk, index);

    /* TODO: initialize entry */

    return entity;
}

static nu_error_t
nu__table_manager_init (nu__table_manager_t *manager,
                        nu__allocator_t     *alloc,
                        nu_u16_t             table_capacity,
                        nu_u16_t             field_capacity,
                        nu_u16_t             chunk_capacity)
{
    nu_size_t i;

    NU_ASSERT(table_capacity && field_capacity && chunk_capacity);

    manager->tables = nu__alloc(
        alloc, sizeof(nu__table_entry_t) * table_capacity, NU_MEMORY_USAGE_ECS);
    NU_CHECK(manager->tables, return NU_ERROR_OUT_OF_MEMORY);

    manager->fields = nu__alloc(
        alloc, sizeof(nu__field_entry_t) * field_capacity, NU_MEMORY_USAGE_ECS);
    NU_CHECK(manager->fields, return NU_ERROR_OUT_OF_MEMORY);

    manager->chunks = nu__alloc(
        alloc, sizeof(nu__chunk_entry_t) * chunk_capacity, NU_MEMORY_USAGE_ECS);
    NU_CHECK(manager->chunks, return NU_ERROR_OUT_OF_MEMORY);

    manager->data = nu__alloc(
        alloc, NU_TABLE_CHUNK_SIZE * chunk_capacity, NU_MEMORY_USAGE_ECS);
    NU_CHECK(manager->data, return NU_ERROR_OUT_OF_MEMORY);

    manager->table_capacity = table_capacity;
    manager->field_capacity = field_capacity;
    manager->chunk_capacity = chunk_capacity;
    manager->table_count    = 0;
    manager->field_count    = 0;
    manager->first_table    = NU_TABLE_NONE;
    manager->free_chunk     = 0;

    for (i = 0; i < chunk_capacity; ++i)
    {
        manager->chunks[i].next = i + 1;
    }
    manager->chunks[chunk_capacity - 1].next
        = NU_TABLE_NONE; /* out of chunks */

    return NU_ERROR_NONE;
}

#endif

#endif