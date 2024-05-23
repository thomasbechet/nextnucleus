#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

void *
allocator_callback (nu_size_t         size,
                    nu_size_t         align,
                    nu_memory_usage_t usage,
                    void             *userdata)
{
    (void)size;
    (void)align;
    (void)userdata;
    (void)usage;
    return malloc(size);
}

/* static nu_system_info_t my_system = {

}; */

nu_error_t
bootstrap (nu_api_t api)
{
    nu_error_t     error;
    nu_component_t component;

    error = nu_register_component(api, "transform", &component);
    NU_ERROR_CHECK(error, return error);

    error = nu_register_property(
        api, component, "position", NU_TYPE_U32, NU_NULL);
    NU_ERROR_CHECK(error, return error);
    error = nu_register_property(
        api, component, "rotation", NU_TYPE_U32, NU_NULL);
    NU_ERROR_CHECK(error, return error);
    error = nu_register_property(api, component, "scale", NU_TYPE_U32, NU_NULL);
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
