#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef struct
{
    int value;
} object_t;

#define OBJECT_COUNT 64
#define REMOVE_COUNT 18

static nu_u8_t memory[NU_MEM_64M];

static nu_u32_t
nu_random ()
{
    static nu_u32_t next = 1231125;
    next                 = (unsigned int)(next * 123423542) % 23423425;
    return next;
}

int
main (void)
{

    nu__allocator_t allocator;
    nu__slotmap_t   sm;
    nu__slotlist_t  sl;
    nu__slot_t      first, it;
    nu_size_t       i;
    nu__slot_t      removed[REMOVE_COUNT];
    object_t       *o;

    NU_ERROR_ASSERT(nu__allocator_init(memory, NU_MEM_64M, &allocator));

    nu__slotmap_init(
        &allocator, NU_ALLOC_FLAG_CORE, sizeof(object_t), OBJECT_COUNT, &sm);
    nu__slotlist_init(&allocator, NU_ALLOC_FLAG_CORE, OBJECT_COUNT, &sl);

    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        nu__slot_t slot = nu__slotmap_add(sm);
        o               = nu__slotmap_get(sm, slot);
        o->value        = i;
    }

    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        o = nu__slotmap_get(sm, i + 1);
        NU_ASSERT(o->value == i);
    }

    first = NU_SLOT_NULL;
    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        nu__slotlist_add_first(sl, &first, i + 1);
    }

    it = first;
    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        NU_ASSERT(it == i + 1);
        it = nu__slotlist_next(sl, it);
    }

    for (i = 0; i < OBJECT_COUNT; i += 2)
    {
        nu__slotlist_remove(sl, &first, i + 1);
    }

    it = first;
    for (i = 1; i < OBJECT_COUNT; i += 2)
    {
        NU_ASSERT(it == i + 1);
        it = nu__slotlist_next(sl, it);
    }

    for (i = 0; i < REMOVE_COUNT; ++i)
    {
        nu__slot_t r = nu_random() % OBJECT_COUNT;
        nu__slotmap_remove(sm, r);
        removed[i] = r;
    }

    for (i = 0; i < REMOVE_COUNT; ++i)
    {
        NU_ASSERT(nu__slotmap_add(sm) == removed[REMOVE_COUNT - i - 1]);
    }

    return 0;
}
