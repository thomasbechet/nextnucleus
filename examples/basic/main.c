#include <stdlib.h>
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

void *
alloc_callback (nu_size_t size, nu_size_t align, void *userdata)
{
    (void)size;
    (void)align;
    (void)userdata;
    return malloc(size);
}

void
free_callback (void *ptr, void *userdata)
{
    (void)userdata;
    free(ptr);
}

static const nu_allocator_info_t allocator = {
    NU_NULL,
    alloc_callback,
    free_callback,
};

/* typedef struct
{
    int dummy;
} my_system_t;

static nu_result_t
my_system_init (void *sys)
{
    (void)sys;
    return NU_SUCCESS;
}

static nu_result_t
my_system_run (const void *sys)
{
    (void)sys;
    return NU_SUCCESS;
}

static const nu_system_info_t my_system_info
    = { sizeof(my_system_t), my_system_init, my_system_run }; */

int
main (void)
{
    nu_engine_t engine;
    nu_u32_t    tick = 0;

    nu_engine_init(&allocator, &engine);
    while (tick < 10)
    {
        nu_engine_tick(engine);
        tick += 1;
    }
    nu_engine_free(engine);

    return 0;
}
