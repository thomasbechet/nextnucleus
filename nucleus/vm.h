#ifndef NU_VM_H
#define NU_VM_H

#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/types.h>
#include <nucleus/allocator.h>
#include <nucleus/renderer.h>
#include <nucleus/ecs.h>
#include <nucleus/ecs/api.h>

typedef struct nu__vm *nu_vm_t;

typedef enum
{
    NU_VM_STATE_STOPPED,
    NU_VM_STATE_RUNNING,
    NU_VM_STATE_FAILURE
} nu_vm_state_t;

typedef struct
{
    nu_ecs_info_t       ecs;
    nu_allocator_info_t allocator;
} nu_vm_info_t;

typedef nu_error_t (*nu_vm_exec_pfn_t)(nu_api_t api);

NU_API nu_error_t nu_vm_init(const nu_vm_info_t *info, nu_vm_t *vm);

NU_API nu_error_t nu_vm_start(nu_vm_t vm);

NU_API nu_error_t nu_vm_stop(nu_vm_t vm);

NU_API nu_error_t nu_vm_free(nu_vm_t vm);

NU_API nu_error_t nu_vm_tick(nu_vm_t vm);

NU_API nu_error_t nu_vm_save(nu_vm_t vm);

NU_API nu_error_t nu_vm_load(nu_vm_t vm);

NU_API nu_error_t nu_vm_exec(nu_vm_t vm, nu_vm_exec_pfn_t exec);

NU_API nu_error_t nu_vm_bind_renderer(nu_vm_t                   vm,
                                      const nu_renderer_info_t *info);

NU_API nu_error_t nu_register_component(nu_vm_t                    vm,
                                        const nu_component_info_t *info,
                                        nu_handle_t               *handle);
NU_API nu_error_t nu_register_archetype(nu_vm_t                    vm,
                                        const nu_archetype_info_t *info,
                                        nu_handle_t               *handle);
NU_API nu_error_t nu_register_system(nu_vm_t                 vm,
                                     const nu_system_info_t *info,
                                     nu_handle_t            *handle);

#ifdef NU_IMPLEMENTATION

struct nu__vm
{
    nu_vm_state_t   state;
    nu__allocator_t allocator;
    nu__ecs_t       ecs;
    nu__renderer_t  renderer;
};

static nu__api_t
nu__build_api (nu_vm_t vm)
{
    nu__api_t api;
    api.allocator = &vm->allocator;
    api.ecs       = &vm->ecs;
    return api;
}

nu_error_t
nu_vm_init (const nu_vm_info_t *info, nu_vm_t *vm)
{
    struct nu__vm *data;
    nu_error_t     error;

    NU_ASSERT(info->allocator.callback);

    data = info->allocator.callback(sizeof(struct nu__vm),
                                    16,
                                    NU_MEMORY_USAGE_CORE,
                                    info->allocator.userdata);
    NU_CHECK(data, return NU_ERROR_OUT_OF_MEMORY);

    data->state = NU_VM_STATE_STOPPED;

    error = nu__allocator_init(&info->allocator, &data->allocator);
    NU_ERROR_CHECK(error, return error);

    error = nu__ecs_init(&info->ecs, &data->allocator, &data->ecs);
    NU_ERROR_CHECK(error, return error);

    *vm = data;

    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_start (nu_vm_t vm)
{
    vm->state = NU_VM_STATE_RUNNING;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_stop (nu_vm_t vm)
{
    vm->state = NU_VM_STATE_STOPPED;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_free (nu_vm_t vm)
{
    (void)vm;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_tick (nu_vm_t vm)
{
    nu__api_t api = nu__build_api(vm);
    if (vm->state != NU_VM_STATE_RUNNING)
    {
        return NU_ERROR_INVALID_STATE;
    }
    return nu__ecs_tick(&vm->ecs, &api);
}

nu_error_t
nu_vm_exec (nu_vm_t vm, nu_vm_exec_pfn_t exec)
{
    nu__api_t api = nu__build_api(vm);
    if (vm->state != NU_VM_STATE_RUNNING)
    {
        return NU_ERROR_INVALID_STATE;
    }
    return exec(&api);
}

nu_error_t
nu_register_component (nu_vm_t                    vm,
                       const nu_component_info_t *info,
                       nu_handle_t               *handle)
{
    nu__slot_t component;
    nu_error_t error;
    if (vm->state != NU_VM_STATE_STOPPED)
    {
        return NU_ERROR_INVALID_STATE;
    }
    error = nu__ecs_register_component(&vm->ecs, info, &component);
    if (handle)
    {
        *handle = component;
    }
    return error;
}
nu_error_t
nu_register_archetype (nu_vm_t                    vm,
                       const nu_archetype_info_t *info,
                       nu_handle_t               *handle)
{

    nu__slot_t archetype;
    nu_error_t error;
    if (vm->state != NU_VM_STATE_STOPPED)
    {
        return NU_ERROR_INVALID_STATE;
    }
    error = nu__ecs_register_archetype(&vm->ecs, info, &archetype);
    if (handle)
    {
        *handle = archetype;
    }
    return error;
}
nu_error_t
nu_register_system (nu_vm_t                 vm,
                    const nu_system_info_t *info,
                    nu_handle_t            *handle)
{
    nu__slot_t system;
    nu_error_t error;
    if (vm->state != NU_VM_STATE_STOPPED)
    {
        return NU_ERROR_INVALID_STATE;
    }
    error = nu__ecs_register_system(&vm->ecs, info, &system);
    if (handle)
    {
        *handle = system;
    }
    return error;
}

#endif

#endif
