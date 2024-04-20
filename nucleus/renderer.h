#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/types.h>

typedef struct
{
    nu_result_t (*load)(void *userdata);
    nu_result_t (*unload)(void *userdata);
    nu_result_t (*draw)(void *userdata);
} nu_renderer_api_t;

typedef struct
{
    void *userdata;
    nu_renderer_api_t api;
} nu_renderer_info_t;

typedef struct
{
    void *userdata;
    nu_renderer_api_t api;
} nu__renderer_t;

#endif
