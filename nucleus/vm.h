#ifndef NU_VM_H
#define NU_VM_H

#include <nucleus/vm/property.h>
#include <nucleus/vm/error.h>
#include <nucleus/vm/platform.h>
#include <nucleus/vm/table.h>
#include <nucleus/vm/allocator.h>
#include <nucleus/vm/renderer.h>
#include <nucleus/vm/cartridge.h>
#include <nucleus/vm/api.h>
#include <nucleus/vm/list.h>
#include <nucleus/vm/slotmap.h>

typedef struct nu__vm *nu_vm_t;

typedef struct
{
    nu_allocator_info_t allocator;
    nu_cartridge_info_t cartridge;
} nu_vm_info_t;

typedef nu_error_t (*nu_vm_exec_pfn_t)(nu_api_t api);

NU_API nu_error_t nu_vm_init(const nu_vm_info_t *info, nu_vm_t *vm);

NU_API nu_error_t nu_vm_free(nu_vm_t vm);

NU_API nu_error_t nu_vm_tick(nu_vm_t vm);

NU_API nu_error_t nu_vm_save(nu_vm_t vm);

NU_API nu_error_t nu_vm_load(nu_vm_t vm);

NU_API nu_error_t nu_vm_exec(nu_vm_t vm, nu_vm_exec_pfn_t exec);

NU_API nu_error_t nu_vm_bind_renderer(nu_vm_t                   vm,
                                      const nu_renderer_info_t *info);

#ifdef NU_IMPLEMENTATION

struct nu__vm
{
    nu__allocator_t        allocator;
    nu__renderer_t         renderer;
    nu__table_manager_t    tables;
    nu__property_manager_t properties;
};

static struct nu__api
nu__build_api (nu_vm_t vm)
{
    struct nu__api api;
    api.allocator = &vm->allocator;
    api.tables    = &vm->tables;
    return api;
}

nu_error_t
nu_vm_init (const nu_vm_info_t *info, nu_vm_t *vm)
{
    struct nu__vm     *data;
    nu_vm_properties_t properties;
    nu_error_t         error;

    NU_ASSERT(info->allocator.callback);

    data = info->allocator.callback(sizeof(struct nu__vm),
                                    16,
                                    NU_MEMORY_USAGE_CORE,
                                    info->allocator.userdata);
    NU_CHECK(data, return NU_ERROR_OUT_OF_MEMORY);

    error = nu__allocator_init(&info->allocator, &data->allocator);
    NU_ERROR_CHECK(error, return error);

    error = info->cartridge.load_properties(info->cartridge.userdata,
                                            &properties);
    NU_ERROR_CHECK(error, return error);

    error = nu__table_manager_init(&data->tables,
                                   &data->allocator,
                                   properties.table_capacity,
                                   properties.field_capacity,
                                   properties.chunk_capacity);
    NU_ERROR_CHECK(error, return error);

    *vm = data;

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
    struct nu__api api = nu__build_api(vm);
    (void)api;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_exec (nu_vm_t vm, nu_vm_exec_pfn_t exec)
{
    struct nu__api api = nu__build_api(vm);
    return exec(&api);
}

#endif

#endif
