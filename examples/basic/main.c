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
    nu_error_t          error;
    nu_component_info_t info;
    nu_property_info_t  prop;
    nu_component_t      component;

    info.name = "transform";
    error     = nu_register_component(api, &info, &component);
    NU_ERROR_CHECK(error, return error);

    prop.name = "position";
    prop.type = NU_TYPE_U32;
    prop.kind = NU_PROPERTY_SCALAR;
    error     = nu_register_property(api, component, &prop, NU_NULL);
    NU_ERROR_CHECK(error, return error);
    prop.name = "rotation";
    prop.type = NU_TYPE_U32;
    prop.kind = NU_PROPERTY_SCALAR;
    error     = nu_register_property(api, component, &prop, NU_NULL);
    NU_ERROR_CHECK(error, return error);
    prop.name = "scale";
    prop.type = NU_TYPE_U32;
    prop.kind = NU_PROPERTY_SCALAR;
    error     = nu_register_property(api, component, &prop, NU_NULL);
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

    info.userdata  = NU_NULL;
    info.alloc     = alloc_callback;
    info.free      = free_callback;
    info.heap_size = NU_MEM_1G;

    nu_vm_init(&info, &vm);
    nu_vm_exec(vm, bootstrap);
    tick = 60;
    while (--tick)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }
    nu_vm_free(vm);

    return 0;
}
