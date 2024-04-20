#ifndef NU_ENGINE_H
#define NU_ENGINE_H

#include "nucleus/macro.h"
#include <nucleus/platform.h>
#include <nucleus/types.h>
#include <nucleus/allocator.h>
#include <nucleus/renderer.h>

NU_DECLARE_HANDLE(nu_engine_t);

typedef struct
{
    nu__allocator_t allocator;
    nu__renderer_t  renderer;
} nu__engine_t;

NU_API nu_result_t nu_engine_init(const nu_allocator_info_t *allocator,
                                  nu_engine_t               *engine);

NU_API nu_result_t nu_engine_free(nu_engine_t engine);

NU_API nu_result_t nu_engine_tick(nu_engine_t engine);

NU_API nu_result_t nu_engine_save(nu_engine_t engine);

NU_API nu_result_t nu_engine_load(nu_engine_t engine);

NU_API nu_result_t nu_engine_execute(nu_engine_t engine);

NU_API nu_result_t nu_engine_load_renderer(nu_engine_t               engine,
                                           const nu_renderer_info_t *info);

#ifdef NU_IMPLEMENTATION

nu_result_t
nu_engine_init (const nu_allocator_info_t *allocator, nu_engine_t *engine)
{
    nu__engine_t *data;

    data = allocator->alloc(sizeof(nu__engine_t), 0, allocator->userdata);
    data->allocator.userdata = allocator->userdata;
    data->allocator.alloc    = allocator->alloc;
    data->allocator.free     = allocator->free;

    *engine = (nu_engine_t)data;

    return NU_SUCCESS;
}

nu_result_t
nu_engine_free (nu_engine_t engine)
{
    (void)engine;
    return NU_SUCCESS;
}

nu_result_t
nu_engine_tick (nu_engine_t engine)
{
    (void)engine;
    return NU_SUCCESS;
}

nu_result_t
nu_engine_execute (nu_engine_t engine)
{
    (void)engine;
    return NU_SUCCESS;
}

#endif

#endif
