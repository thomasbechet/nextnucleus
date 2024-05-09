#ifndef NU_VM_H
#define NU_VM_H

#include "nucleus/ecs/system.h"
#include <nucleus/platform.h>
#include <nucleus/types.h>
#include <nucleus/macro.h>
#include <nucleus/allocator.h>
#include <nucleus/renderer.h>
#include <nucleus/ecs.h>

typedef struct nu__vm *nu_vm_t;

typedef struct
{
    void                    *userdata;
    nu_allocator_alloc_pfn_t alloc;
    nu_allocator_free_pfn_t  free;
    nu_size_t                heap_size;
    void                    *vaddr;
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

nu_error_t
nu_vm_init (const nu_vm_info_t *info, nu_vm_t *vm)
{
    void           *vaddr;
    nu__allocator_t allocator;
    struct nu__vm  *data;
    nu_error_t      error;

    vaddr = info->alloc(info->heap_size, 16, info->userdata);
    NU_CHECK(vaddr, return NU_ERROR_OUT_OF_MEMORY);

    error = nu__allocator_init(vaddr, info->heap_size, &allocator);
    NU_ERROR_CHECK(error, goto cleanup0);

    data = nu__alloc(&allocator, sizeof(struct nu__vm), NU_ALLOC_FLAG_CORE);
    NU_CHECK(data, goto cleanup0);

    data->allocator = allocator;

    error = nu__ecs_init(&data->allocator, &data->ecs);
    NU_ERROR_CHECK(error, goto cleanup0);

    *vm = data;

    return NU_ERROR_NONE;

cleanup0:
    info->free(vaddr, info->userdata);

    return error;
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
    (void)vm;
    (void)exec;
    return NU_ERROR_NONE;
}

#endif

#endif
