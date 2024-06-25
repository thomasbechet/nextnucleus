#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define NU_IMPL
#define NU_DEBUG
#define NU_STDLIB
#include <nucleus/vm.h>

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
    nu_table_t      table;
    nu_entity_t     e;
    nu_field_info_t fields[] = { { "position", NU_PRIMITIVE_INT, 1 },
                                 { "rotation", NU_PRIMITIVE_INT, 1 } };

    error = nu_create_table(api, "player", fields, 2, &table);
    NU_ERROR_ASSERT(error);

    e = nu_spawn(api, table);
    NU_ASSERT(e);

    (void)error;
    (void)e;
    (void)table;

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

    {
        nu_f32_t a = nu_itof(123);
        nu_f32_t b = nu_itof(32);
        nu_f32_t c = nu_fmul(a, b);
        (void)a;
        (void)b;
        (void)c;
        c = b;
    }

    info.allocator.userdata        = NU_NULL;
    info.allocator.callback        = allocator_callback;
    info.cartridge.userdata        = NU_NULL;
    info.cartridge.load_properties = load_properties;

    error = nu_vm_init(&info, &vm);
    NU_ERROR_CHECK(error, return 123);

    error = nu_vm_exec(vm, bootstrap);
    NU_ERROR_CHECK(error, return 124);

    tick = 10;
    while (--tick)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }

    nu_vm_free(vm);

    return 0;
}
