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
bootstrap (nu_api_t api)
{
    nu_error_t     error;
    nu_component_t it   = NU_NULL;
    nu_archetype_t arch = NU_NULL;

    while (nu_next_component(api, &it))
    {
        nu_component_info_t info;
        error = nu_component_info(api, it, &info);
        NU_ERROR_CHECK(error, return error);
        if (info.name)
        {
            printf("%s\n", info.name);
        }
        else
        {
            printf("UNNAMED\n");
        }
    }
    while (nu_next_archetype(api, &arch))
    {
        const nu_char_t *name = nu_archetype_name(api, arch);
        if (name)
        {
            printf("%s\n", name);
        }
        else
        {
            printf("UNNAMED\n");
        }
    }

    {
        nu_group_t  group;
        nu_entity_t e;
        arch = nu_find_archetype(api, nu_uid("player"));
        NU_ASSERT(arch);
        group = nu_create_group(api, arch, 128);
        NU_ASSERT(group);
        e = nu_spawn(api, group);
        NU_ASSERT(e);
        printf("0x%08X\n", e);
        e = nu_spawn(api, group);
        NU_ASSERT(e);
        printf("0x%08X\n", e);
    }

    return NU_ERROR_NONE;
}

nu_error_t
my_system (nu_api_t api)
{
    nu_query_t           q;
    nu_entity_t          it;
    const nu_component_t components[] = { 0, 1 };

    printf("my_system\n");

    q = nu_query(api, components, 2);

    while (nu_query_next(api, q, &it))
    {
        nu_u32_t *positions = nu_field(api, it, 0);
        positions[0]        = 0;
    }

    return NU_ERROR_NONE;
}

int
main (void)
{
    nu_vm_t             vm;
    nu_vm_info_t        info;
    nu_component_info_t component;
    nu_archetype_info_t archetype;
    nu_system_info_t    system;
    nu_component_t      position, rotation;
    nu_component_t      components[10];
    nu_error_t          error;
    nu_size_t           tick;

    info.allocator.userdata = NU_NULL;
    info.allocator.callback = allocator_callback;
    nu_ecs_info_default(&info.ecs);

    nu_vm_init(&info, &vm);

    component.name = "position";
    component.size = 1;
    component.type = NU_TYPE_IV3;
    error          = nu_register_component(vm, &component, &position);
    NU_ERROR_CHECK(error, return 0);

    component.name = "rotation";
    component.size = 1;
    component.type = NU_TYPE_QUAT;
    error          = nu_register_component(vm, &component, &rotation);
    NU_ERROR_CHECK(error, return 0);

    components[0]             = position;
    components[1]             = rotation;
    archetype.name            = "player";
    archetype.components      = components;
    archetype.component_count = 2;
    error                     = nu_register_archetype(vm, &archetype, NU_NULL);
    NU_ERROR_ASSERT(error);

    system.name            = "my_system";
    system.component_count = 0;
    system.callback        = my_system;
    error                  = nu_register_system(vm, &system, NU_NULL);
    NU_ERROR_CHECK(error, return 0);

    error = nu_vm_start(vm);
    NU_ERROR_CHECK(error, return 0);

    error = nu_vm_exec(vm, bootstrap);
    NU_ERROR_CHECK(error, return 0);

    tick = 10;
    while (--tick)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }

    error = nu_vm_stop(vm);
    NU_ERROR_CHECK(error, return 0);
    nu_vm_free(vm);

    return 0;
}
