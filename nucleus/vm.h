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

typedef struct
{
    nu_ecs_info_t       ecs;
    nu_allocator_info_t allocator;
} nu_vm_info_t;

typedef nu_error_t (*nu_vm_exec_pfn_t)(nu_api_t api);

NU_API nu_error_t nu_vm_init(const nu_vm_info_t *info, nu_vm_t *vm);

NU_API nu_error_t nu_vm_free(nu_vm_t vm);

NU_API nu_error_t nu_vm_tick(nu_vm_t vm);

NU_API nu_error_t nu_vm_save(nu_vm_t vm);

NU_API nu_error_t nu_vm_load(nu_vm_t vm);

NU_API nu_error_t nu_vm_exec(nu_vm_t vm, nu_vm_exec_pfn_t exec);

NU_API nu_error_t nu_vm_load_renderer(nu_vm_t                   vm,
                                      const nu_renderer_info_t *info);

#ifdef NU_IMPLEMENTATION

struct nu__vm
{
    nu__allocator_t allocator;
    nu__ecs_t       ecs;
    nu__renderer_t  renderer;
};

static struct nu_api
nu__build_api (nu_vm_t vm)
{
    struct nu_api api;
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

    error = nu__allocator_init(&info->allocator, &data->allocator);
    NU_ERROR_CHECK(error, return error);

    error = nu__ecs_init(&info->ecs, &data->allocator, &data->ecs);
    NU_ERROR_CHECK(error, return error);

    *vm = data;

    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_free (nu_vm_t vm)
{
    nu_error_t error;

    error = nu__ecs_free(&vm->ecs);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_tick (nu_vm_t vm)
{
    (void)vm;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_exec (nu_vm_t vm, nu_vm_exec_pfn_t exec)
{
    struct nu_api api = nu__build_api(vm);
    return exec(&api);
}

#endif

#endif
