#ifndef NU_API_H
#define NU_API_H

#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/types.h>
#include <nucleus/table.h>

typedef struct nu__api *nu_api_t;

NU_API nu_bool_t nu_next_table(nu_api_t api, nu_table_t *it);
/* NU_API nu_bool_t   nu_next_system(nu_api_t api, nu_system_t *it); */
NU_API nu_error_t  nu_create_table(nu_api_t               api,
                                   const nu_char_t       *name,
                                   const nu_field_info_t *fields,
                                   nu_u16_t               field_count,
                                   nu_table_t            *handle);
NU_API nu_table_t  nu_find_table(nu_api_t api, nu_uid_t uid);
NU_API nu_field_t  nu_find_field(nu_api_t api, nu_table_t table, nu_uid_t uid);
NU_API nu_entity_t nu_spawn(nu_api_t api, nu_table_t table);
NU_API void       *nu_field(nu_api_t api, nu_entity_t e, nu_field_t f);

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

struct nu__api
{
    nu__allocator_t     *allocator;
    nu__table_manager_t *tables;
};

nu_error_t
nu_create_table (nu_api_t               api,
                 const nu_char_t       *name,
                 const nu_field_info_t *fields,
                 nu_u16_t               field_count,
                 nu_table_t            *handle)
{
    return nu__table_create(api->tables, name, fields, field_count, handle);
}
nu_entity_t
nu_spawn (nu_api_t api, nu_table_t table)
{
    return nu__table_spawn(api->tables, table);
}
void *
nu_field (nu_api_t api, nu_entity_t e, nu_field_t f)
{
    return nu__table_field(api->tables, e, f);
}

#endif

#endif
