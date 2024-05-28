#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define NU_IMPLEMENTATION
#define NU_DEBUG
#define NU_STDLIB
#include <nucleus/nucleus.h>

static nu_size_t mem_total = 0;

void *
allocator_callback (nu_size_t         size,
                    nu_size_t         align,
                    nu_memory_usage_t usage,
                    void             *userdata)
{
    (void)align;
    (void)userdata;
    mem_total += size;
    printf("alloc %ld from %d (total: %ld)\n", size, usage, mem_total);
    return malloc(size);
}

nu_error_t
my_system (nu_api_t api)
{
    (void)api;

    return NU_ERROR_NONE;
}

static nu_error_t
register_my_system (nu_api_t api)
{
    nu_system_info_t      info;
    nu_error_t            error;
    nu_system_component_t components[2];

    NU_CHECK(components[0].handle = nu_find_component(api, nu_uid("position")),
             return NU_ERROR_RESOURCE_NOT_FOUND);
    components[0].access = NU_COMPONENT_READ;
    components[0].slot   = 0;

    NU_CHECK(components[1].handle = nu_find_component(api, nu_uid("rotation")),
             return NU_ERROR_RESOURCE_NOT_FOUND);
    components[1].access = NU_COMPONENT_READ;
    components[1].slot   = 1;

    info.name            = "my_system";
    info.callback        = my_system;
    info.components      = components;
    info.component_count = 2;
    error                = nu_register_system(api, &info, NU_NULL);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}

nu_error_t
bootstrap (nu_api_t api)
{
    nu_error_t          error;
    nu_size_t           i;
    nu_archetype_info_t arch;

    const nu_component_info_t components[]
        = { { "position", NU_TYPE_FV3, 1 }, { "rotation", NU_TYPE_QUAT, 1 } };
    const nu_size_t component_count
        = sizeof(components) / sizeof(components[0]);
    nu_handle_t player_components[2];

    for (i = 0; i < component_count; ++i)
    {
        printf("register '%s' component\n", components[i].name);
        error = nu_register_component(api, &components[i], NU_NULL);
        NU_ERROR_CHECK(error, return error);
    }

    player_components[0] = nu_find_component(api, nu_uid("position"));
    player_components[1] = nu_find_component(api, nu_uid("rotation"));
    arch.name            = "player";
    arch.components      = player_components;
    arch.component_count = 2;
    arch.entity_capacity = 128;
    error                = nu_register_archetype(api, &arch, NU_NULL);
    NU_ERROR_CHECK(error, return error);

    error = register_my_system(api);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}

int
main (void)
{
    nu_vm_t      vm;
    nu_vm_info_t info;
    void        *p = malloc(NU_MEM_1G);
    nu_size_t    tick;
    NU_ASSERT(p);
    (void)p;

    info.allocator.userdata = NU_NULL;
    info.allocator.callback = allocator_callback;
    nu_ecs_info_default(&info.ecs);

    nu_vm_init(&info, &vm);
    NU_ERROR_ASSERT(nu_vm_exec(vm, bootstrap));
    tick = 60;
    while (--tick)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }
    nu_vm_free(vm);

    return 0;
}
