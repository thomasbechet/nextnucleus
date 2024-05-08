#include <assert.h>
#define NU_IMPLEMENTATION
#define NU_DEBUG
#define NU_STDLIB
#include <nucleus/nucleus.h>

typedef struct
{
    int value;
} object_t;

static nu_u8_t memory[NU_MEM_64M];

#define OBJECT_COUNT 128

int
main (void)
{
    nu__allocator_t allocator;
    nu__list_t      list;
    object_t       *objs[OBJECT_COUNT];
    object_t       *it;
    nu_size_t       i;

    NU_ERROR_ASSERT(nu__allocator_init(memory, NU_MEM_64M, &allocator));
    nu__list_init(&list, sizeof(object_t));

    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        NU_ERROR_ASSERT(nu__list_append(
            &list, &allocator, NU_ALLOC_FLAG_CORE, (void **)&objs[i]));
        objs[i]->value = i;
    }

    it = nu__list_first(&list);
    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        NU_ASSERT(it);
        NU_ASSERT(it->value == i);
        it = nu__list_next(&list, it);
    }

    for (i = 0; i < OBJECT_COUNT; i += 2)
    {
        nu__list_remove(&list, objs[i]);
    }

    it = nu__list_first(&list);
    for (i = 1; i < OBJECT_COUNT; i += 2)
    {
        NU_ASSERT(it);
        NU_ASSERT(it->value == i);
        it = nu__list_next(&list, it);
    }

    for (i = 0; i < OBJECT_COUNT; i += 2)
    {
        nu__list_remove(&list, objs[i]);
    }

    return 0;
}
