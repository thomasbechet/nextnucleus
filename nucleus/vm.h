#ifndef NU_VM_H
#define NU_VM_H

#include "nucleus/error.h"
#include <nucleus/platform.h>
#include <nucleus/types.h>
#include <nucleus/macro.h>
#include <nucleus/allocator.h>
#include <nucleus/renderer.h>

typedef struct nu__vm *nu_vm_t;

typedef struct
{
    void                    *userdata;
    nu_allocator_alloc_pfn_t alloc;
    nu_allocator_free_pfn_t  free;
    nu_size_t                heap_size;
    void                    *vaddr;
} nu_vm_info_t;

NU_API nu_error_t nu_vm_init(const nu_vm_info_t *info, nu_vm_t *vm);

NU_API nu_error_t nu_vm_free(nu_vm_t vm);

NU_API nu_error_t nu_vm_tick(nu_vm_t vm);

NU_API nu_error_t nu_vm_save(nu_vm_t vm);

NU_API nu_error_t nu_vm_load(nu_vm_t vm);

NU_API nu_error_t nu_vm_execute(nu_vm_t vm);

NU_API nu_error_t nu_vm_load_renderer(nu_vm_t                   vm,
                                      const nu_renderer_info_t *info);

#ifdef NU_IMPLEMENTATION

struct nu__vm
{
    nu__allocator_t allocator;
    nu__renderer_t  renderer;
};

nu_error_t
nu_vm_init (const nu_vm_info_t *info, nu_vm_t *vm)
{
    void           *vaddr, *ptr;
    nu__allocator_t allocator;
    nu_error_t      error;

    vaddr = info->alloc(info->heap_size, 16, info->userdata);
    NU_CHECK(vaddr, return NU_ERROR_OUT_OF_MEMORY);

    error = nu__allocator_init(vaddr, info->heap_size, &allocator);
    NU_ERROR_CHECK(error, goto cleanup0);

    error = nu__malloc(
        &allocator, 0, sizeof(struct nu__vm), NU_ALLOC_FLAG_CORE, &ptr);
    NU_ERROR_CHECK(error, goto cleanup0);

    *vm = ptr;

    (*vm)->allocator = allocator;

    return NU_ERROR_NONE;

cleanup0:
    info->free(vaddr, info->userdata);

    return error;
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
    (void)vm;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_execute (nu_vm_t vm)
{
    (void)vm;
    return NU_ERROR_NONE;
}

#endif

#endif
