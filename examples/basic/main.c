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

/* static nu_system_info_t my_system = {

}; */

nu_error_t
bootstrap (nu_api_t api)
{
    nu_component_info_t info;
    nu_error_t          error;
    nu_property_info_t  properties[3];

    properties[0].ident = "position";
    properties[0].type  = NU_TYPE_U32;
    properties[0].kind  = NU_PROPERTY_SCALAR;

    properties[1].ident = "rotation";
    properties[1].type  = NU_TYPE_U32;
    properties[1].kind  = NU_PROPERTY_SCALAR;

    properties[2].ident = "scale";
    properties[2].type  = NU_TYPE_U32;
    properties[2].kind  = NU_PROPERTY_SCALAR;

    info.ident          = "transform";
    info.properties     = properties;
    info.property_count = 3;
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
