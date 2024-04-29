#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
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

typedef struct
{
    nu_property_t transform_position;
} my_system_t;

static nu_error_t
my_system_init (void *sys)
{
    (void)sys;
    sys->transform_position = nu_prop_find("transform", "position", NU_TYPE_VEC3);
    return NU_ERROR_NONE;
}

static nu_error_t
my_system_run (const void *sys, nu_api_t api)
{
    /* nu_query_iter_t it;
    nu_size_t       i; */

    (void)sys;
    (void)api;

    it = nu_query_begin(api, sys->q);
    while (it = nu_query_next(api, it))
    {
        nu_vec3_t *positions = nu_prop_vec3(it, sys->transform_position);
        for (i = 0; i < it->count; ++i)
        {
            position[i] =
        }
    }
    return NU_ERROR_NONE;
}

const nu_system_info_t my_system_info
    = { "my_system", sizeof(my_system_t), my_system_init, my_system_run };
#include <stdio.h>
typedef struct
{
    int a;
} mystruct;
int
main (void)
{
    nu_vm_t      vm;
    nu_vm_info_t info;
    void        *p = malloc(NU_MEM_1G);
    NU_ASSERT(p);
    (void)p;

    info.userdata  = NU_NULL;
    info.alloc     = alloc_callback;
    info.free      = free_callback;
    info.heap_size = NU_MEM_1G;

    nu_vm_init(&info, &vm);
    while (1)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }
    nu_vm_free(vm);

    return 0;
}
