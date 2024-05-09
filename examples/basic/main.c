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

nu_error_t
bootstrap (nu_api_t api)
{
    nu_component_info_t info;
    nu_error_t          error;

    info.ident          = "my_component";
    info.properties     = NU_NULL;
    info.property_count = 0;
    error               = nu_register_component(api, &info);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}

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
    nu_vm_exec(vm, bootstrap);
    while (1)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }
    nu_vm_free(vm);

    return 0;
}
