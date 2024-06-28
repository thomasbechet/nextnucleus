#ifndef NU_CARTRIDGE_H
#define NU_CARTRIDGE_H

#include <nucleus/vm/resource.h>
#include <nucleus/vm/property.h>
#include <nucleus/vm/types.h>
#include <nucleus/vm/error.h>

#define NU_FOREACH_CARTDATA(CARTDATA) \
    CARTDATA(VM_PROPERTIES)           \
    CARTDATA(BUNDLE)                  \
    CARTDATA(RESOURCE)
#define NU_GENERATE_CARTDATA_ENUM(CARTDATA) NU_CARTDATA_##CARTDATA,
#define NU_GENERATE_CARTDATA_NAME(CARTDATA) #CARTDATA,
typedef enum
{
    NU_FOREACH_CARTDATA(NU_GENERATE_CARTDATA_ENUM) NU_CARTDATA_UNKNOWN
} nu_cartdata_type_t;
const nu_char_t *NU_CARTDATA_NAMES[]
    = { NU_FOREACH_CARTDATA(NU_GENERATE_CARTDATA_NAME) "UNKNOWN" };

typedef struct
{
    nu_vm_properties_t *props;
} nu_cartdata_vm_properties_t;

typedef struct
{
    void *userdata;
    nu_error_t (*load)(void *userdata, nu_cartdata_type_t type, void *data);
} nu_cartridge_api_t;

typedef struct
{
    nu_uid_t uid;
    nu_u32_t entry_count;
    nu_uid_t first_entry;
} nu_cartdata_bundle_t;

typedef struct
{
    const nu_char_t   *name;
    nu_resource_type_t type;
} nu_cartdata_resource_t;

#ifdef NU_IMPL

typedef struct
{
    nu_cartridge_api_t api;
} nu__cartdrige_t;

#endif

#endif
