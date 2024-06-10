#include <assert.h>
#include <nucleus/error.h>
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
    nu_error_t      error;
    nu_field_info_t fields[]
        = { { "position", NU_TYPE_U32, 1 }, { "rotation", NU_TYPE_U32, 1 } };

    error = nu_create_table(api, "player", fields, 2, NU_NULL);
    NU_ERROR_ASSERT(error);

    return NU_ERROR_NONE;
}

static nu_error_t
load_properties (void *userdata, nu_vm_properties_t *properties)
{
    (void)userdata;
    nu_vm_properties_default(properties);
    return NU_ERROR_NONE;
}

int
main (void)
{
    nu_vm_t      vm;
    nu_vm_info_t info;
    nu_error_t   error;
    nu_size_t    tick;

    info.allocator.userdata        = NU_NULL;
    info.allocator.callback        = allocator_callback;
    info.cartridge.userdata        = NU_NULL;
    info.cartridge.load_properties = load_properties;

    error = nu_vm_init(&info, &vm);
    NU_ERROR_CHECK(error, return 0);

    error = nu_vm_exec(vm, bootstrap);
    NU_ERROR_CHECK(error, return 0);

    tick = 10;
    while (--tick)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }

    nu_vm_free(vm);

    return 0;
}
