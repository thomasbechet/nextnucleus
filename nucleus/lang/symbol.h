#ifndef NULANG_SYMBOL_H
#define NULANG_SYMBOL_H

#include <nucleus/lang/allocator.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/lexer.h>
#include <nucleus/lang/token.h>
#include <nucleus/vm/types.h>

#define NULANG_FOREACH_SYMBOL(SYMBOL) \
    SYMBOL(FUNCTION)                  \
    SYMBOL(ARGUMENT)                  \
    SYMBOL(CONSTANT)                  \
    SYMBOL(VARIABLE)                  \
    SYMBOL(ARCHETYPE)                 \
    SYMBOL(MODULE)                    \
    SYMBOL(EXTERNAL)                  \
    SYMBOL(UNKNOWN)
#define NULANG_GENERATE_SYMBOL(SYMBOL)      SYMBOL_##SYMBOL,
#define NULANG_GENERATE_SYMBOL_NAME(SYMBOL) #SYMBOL,
typedef enum
{
    NULANG_FOREACH_SYMBOL(NULANG_GENERATE_SYMBOL)
} nulang__symbol_type_t;
static const nu_char_t *NULANG_SYMBOL_NAMES[]
    = { NULANG_FOREACH_SYMBOL(NULANG_GENERATE_SYMBOL_NAME) };

#define NULANG_FOREACH_BLOCK(BLOCK) \
    BLOCK(GLOBAL)                   \
    BLOCK(FUNCTION)                 \
    BLOCK(WHILE)                    \
    BLOCK(FOR)                      \
    BLOCK(IF)                       \
    BLOCK(LOOP)
#define NULANG_GENERATE_BLOCK(BLOCK)      BLOCK_##BLOCK,
#define NULANG_GENERATE_BLOCK_NAME(BLOCK) #BLOCK,
typedef enum
{
    NULANG_FOREACH_BLOCK(NULANG_GENERATE_BLOCK)
} nulang__block_type_t;
static const nu_char_t *NULANG_BLOCK_NAMES[]
    = { NULANG_FOREACH_BLOCK(NULANG_GENERATE_BLOCK_NAME) };

#define NULANG_FOREACH_VARTYPE(VARTYPE) \
    VARTYPE(ARCHETYPE)                  \
    VARTYPE(PRIMITIVE)                  \
    VARTYPE(UNKNOWN)
#define NULANG_GENERATE_VARTYPE(VARTYPE)      VARTYPE_##VARTYPE,
#define NULANG_GENERATE_VARTYPE_NAME(VARTYPE) #VARTYPE,
typedef enum
{
    NULANG_FOREACH_VARTYPE(NULANG_GENERATE_VARTYPE)
} nulang__vartype_type_t;
static const nu_char_t *NULANG_VARTYPE_NAMES[]
    = { NULANG_FOREACH_VARTYPE(NULANG_GENERATE_VARTYPE_NAME) };

typedef nu_u32_t nulang__symbol_id_t;
typedef nu_u32_t nulang__block_id_t;

#define NULANG_SYMBOL_NULL  0xffffffff
#define NULANG_BLOCK_NULL   0xffffffff
#define NULANG_BLOCK_GLOBAL 0

typedef union
{
    nu_primitive_t      primitive;
    nulang__symbol_id_t archetype;
} nulang__vartype_value_t;

typedef struct
{
    nulang__vartype_type_t  type;
    nulang__vartype_value_t value;
} nulang__vartype_t;

typedef struct
{
    nulang__vartype_t   return_type;
    nulang__symbol_id_t first_arg;
    nu_bool_t           exported;
} nulang__symbol_function_t;
typedef struct
{
    nulang__vartype_t   vartype;
    nulang__symbol_id_t next;
} nulang__symbol_argument_t;
typedef struct
{
    nulang__vartype_t vartype;
    nu_bool_t         exported;
} nulang__symbol_constant_t;
typedef struct
{
    nulang__vartype_t vartype;
} nulang__symbol_variable_t;
typedef struct
{
    int todo;
} nulang__symbol_external_t;

typedef union
{
    int                       ub;
    nulang__symbol_function_t function;
    nulang__symbol_argument_t argument;
    nulang__symbol_constant_t constant;
    nulang__symbol_variable_t variable;
    nulang__symbol_external_t external;
} nulang__symbol_value_t;

typedef struct
{
    nulang__block_type_t type;
    nulang__block_id_t   parent;
    nulang__block_id_t   last;
    nulang__block_id_t   previous_scope_symbol;
} nulang__block_t;

typedef struct
{
    nulang__symbol_type_t  type;
    nulang__symbol_value_t value;
    nulang__string_t       ident;
    nulang__span_t         span;
    nulang__block_id_t     block;
    nulang__symbol_id_t    previous_in_block;
    nulang__symbol_id_t    previous_in_scope;
} nulang__symbol_t;

typedef struct
{
    nulang__symbol_t *symbols;
    nu_u32_t          symbol_count;
    nu_u32_t          symbol_capacity;
    nulang__block_t  *blocks;
    nu_u32_t          block_count;
    nu_u32_t          block_capacity;
} nulang__symbol_table_t;

#ifdef NULANG_IMPL

static nulang__error_t
nulang__symbol_table_init (nu_size_t               symbol_capacity,
                           nu_size_t               block_capacity,
                           nulang__allocator_t    *alloc,
                           nulang__symbol_table_t *table)
{
    table->symbols
        = nulang__alloc(alloc, sizeof(nulang__symbol_t) * symbol_capacity);
    if (!table->symbols)
    {
        return NULANG_ERROR_OUT_OF_MEMORY;
    }
    table->symbol_capacity = symbol_capacity;
    table->symbol_count    = 0;
    table->blocks
        = nulang__alloc(alloc, sizeof(nulang__block_t) * block_capacity);
    if (!table->blocks)
    {
        return NULANG_ERROR_OUT_OF_MEMORY;
    }
    table->block_capacity = block_capacity;
    table->block_count    = 0;

    return NULANG_ERROR_NONE;
}
static void
nulang__symbol_table_clear (nulang__symbol_table_t *table)
{
    table->symbol_count = 0;
    table->block_count  = 0;
}
static nulang__error_t
nulang__symbol_add (nulang__symbol_table_t *table,
                    nulang__symbol_type_t   type,
                    nulang__symbol_value_t  value,
                    nulang__string_t        ident,
                    nulang__span_t          span,
                    nulang__block_id_t      block,
                    nulang__symbol_id_t    *id)
{
    nulang__block_id_t last_block;
    if (table->symbol_count >= table->symbol_capacity)
    {
        return NULANG_ERROR_OUT_OF_SYMBOL;
    }
    *id                                   = table->symbol_count++;
    table->symbols[*id].type              = type;
    table->symbols[*id].value             = value;
    table->symbols[*id].ident             = ident;
    table->symbols[*id].span              = span;
    table->symbols[*id].block             = block;
    table->symbols[*id].previous_in_scope = NULANG_SYMBOL_NULL;
    table->symbols[*id].previous_in_block = NULANG_SYMBOL_NULL;

    /* update previous in scope */
    last_block = table->blocks[block].last;
    if (last_block != NULANG_BLOCK_NULL)
    {
        table->symbols[*id].previous_in_scope = last_block;
    }
    else
    {
        table->symbols[*id].previous_in_scope
            = table->symbols[block].previous_in_scope;
    }

    /* update block list */
    table->symbols[*id].previous_in_block = table->blocks[block].last;
    table->blocks[block].last             = *id;

    return NULANG_ERROR_NONE;
}
static nu_bool_t
nulang__find_symbol_in_block (const nulang__symbol_table_t *table,
                              nulang__block_id_t            block,
                              nulang__string_t              name,
                              nulang__symbol_id_t          *id)
{
    nulang__symbol_id_t symbol;
    symbol = table->blocks[block].last;
    while (symbol != NULANG_SYMBOL_NULL)
    {
        if (NULANG_SOURCE_STRING_EQUALS(table->symbols[symbol].ident, name))
        {
            *id = symbol;
            return NU_TRUE;
        }
        symbol = table->symbols[symbol].previous_in_block;
    }
    return NU_FALSE;
}
static void
nulang__find_symbol_in_scope (const nulang__symbol_table_t *table,
                              nulang__block_id_t            block,
                              nulang__string_t              name,
                              nulang__symbol_id_t          *id)
{
    nulang__symbol_id_t symbol;
    symbol = table->blocks[block].last;
    if (symbol == NULANG_SYMBOL_NULL)
    {
        symbol = table->blocks[block].previous_scope_symbol;
    }
    while (symbol != NULANG_SYMBOL_NULL)
    {
        if (NULANG_SOURCE_STRING_EQUALS(table->symbols[symbol].ident, name))
        {
            *id = symbol;
            return;
        }
        symbol = table->symbols[symbol].previous_in_scope;
    }
    *id = NULANG_SYMBOL_NULL;
}

static nu_bool_t
nulang__check_in_loop (const nulang__symbol_table_t *table,
                       nulang__block_id_t            block)
{
    static nulang__block_type_t types[]
        = { BLOCK_LOOP, BLOCK_WHILE, BLOCK_FOR };
    while (block != NULANG_BLOCK_NULL)
    {
        nu_size_t            i;
        nulang__block_type_t type = table->blocks[block].type;
        for (i = 0; i < NU_ARRAY_SIZE(types); ++i)
        {
            if (types[i] == type)
            {
                return NU_TRUE;
            }
        }
        block = table->blocks[block].parent;
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__check_in_function (const nulang__symbol_table_t *table,
                           nulang__block_id_t            block)
{
    while (block != NULANG_BLOCK_NULL)
    {
        if (table->blocks[block].type == BLOCK_FUNCTION)
        {
            block = table->blocks[block].parent;
        }
    }
    return NU_FALSE;
}
static nulang__error_t
nulang__add_block (nulang__symbol_table_t *table,
                   nulang__block_type_t    type,
                   nulang__block_id_t      parent,
                   nulang__block_id_t     *id)
{
    nulang__symbol_id_t previous_scope_symbol;

    if (table->block_count >= table->block_capacity)
    {
        return NULANG_ERROR_OUT_OF_BLOCK;
    }

    *id = table->block_count++;

    previous_scope_symbol = NULANG_SYMBOL_NULL;
    if (parent != NULANG_BLOCK_NULL)
    {
        previous_scope_symbol = table->blocks[parent].last;
        if (previous_scope_symbol == NULANG_SYMBOL_NULL)
        {
            previous_scope_symbol = table->blocks[parent].previous_scope_symbol;
        }
    }

    table->blocks[*id].type                  = type;
    table->blocks[*id].parent                = parent;
    table->blocks[*id].last                  = NULANG_SYMBOL_NULL;
    table->blocks[*id].previous_scope_symbol = previous_scope_symbol;

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__lookup_symbol (nulang__symbol_table_t *table,
                       nulang__string_t        ident,
                       nulang__span_t          span,
                       nulang__block_id_t      block,
                       nulang__symbol_id_t    *id)
{
    nulang__error_t error;
    nulang__find_symbol_in_scope(table, block, ident, id);
    if (*id == NULANG_SYMBOL_NULL)
    {
        nulang__symbol_value_t unknown_value;
        unknown_value.ub = 0;
        error            = nulang__symbol_add(table,
                                   SYMBOL_UNKNOWN,
                                   unknown_value,
                                   ident,
                                   span,
                                   NULANG_BLOCK_GLOBAL,
                                   id);
        NULANG_ERROR_CHECK(error);
    }
    return NULANG_ERROR_NONE;
}

static nulang__error_t
nulang__lookup_archetype (nulang__symbol_table_t *table,
                          nulang__string_t        ident,
                          nulang__span_t          span,
                          nulang__symbol_id_t    *id)
{
    nu_size_t i;
    for (i = 0; i < table->symbol_count; ++i)
    {
        if (table->symbols[i].type == SYMBOL_ARCHETYPE
            && NULANG_SOURCE_STRING_EQUALS(table->symbols[i].ident, ident))
        {
            *id = i;
            return NULANG_ERROR_NONE;
        }
    }
    *id = NULANG_SYMBOL_NULL;
    if (table->symbol_count >= table->symbol_capacity)
    {
        return NULANG_ERROR_OUT_OF_SYMBOL;
    }
    *id                                   = table->symbol_count++;
    table->symbols[*id].type              = SYMBOL_ARCHETYPE;
    table->symbols[*id].ident             = ident;
    table->symbols[*id].span              = span;
    table->symbols[*id].block             = NULANG_BLOCK_NULL;
    table->symbols[*id].previous_in_scope = NULANG_SYMBOL_NULL;
    table->symbols[*id].previous_in_block = NULANG_SYMBOL_NULL;
    return NULANG_ERROR_NONE;
}

static nulang__error_t
nulang__define_symbol (nulang__symbol_table_t *table,
                       nulang__symbol_type_t   type,
                       nulang__symbol_value_t  value,
                       nulang__string_t        ident,
                       nulang__span_t          span,
                       nulang__block_id_t      block,
                       nulang__symbol_id_t    *id)
{
    nulang__error_t     error;
    nulang__symbol_id_t found;
    /* check existing symbol */
    nulang__find_symbol_in_scope(table, block, ident, &found);
    switch (type)
    {
        case SYMBOL_FUNCTION:
            /* fall-through */
        case SYMBOL_CONSTANT:
            /* fall-through */
        case SYMBOL_MODULE:
            /* fall-through */
        case SYMBOL_EXTERNAL:
            /* fall-through */
        case SYMBOL_ARCHETYPE:
            /* fall-through */
        case SYMBOL_VARIABLE:
            if (found != NULANG_SYMBOL_NULL)
            {
                /* constant and function shadowing is not allowed, we must check
                 * definition */
                if (table->symbols[found].type != SYMBOL_UNKNOWN)
                {
                    return NULANG_ERROR_SYMBOL_ALREADY_DEFINED;
                }
                else
                {
                    /* update symbol */
                    table->symbols[found].type  = type;
                    table->symbols[found].value = value;
                }
            }
            else
            {
                /* not defined or declared */
                error = nulang__symbol_add(
                    table, type, value, ident, span, block, id);
                NULANG_ERROR_CHECK(error);
            }
            break;
        case SYMBOL_ARGUMENT:
            /* fall-through */
        case SYMBOL_UNKNOWN:
            /* may shadow previous symbol */
            error = nulang__symbol_add(
                table, type, value, ident, span, block, id);
            NULANG_ERROR_CHECK(error);
            break;
    }
    return NULANG_ERROR_NONE;
}
static nu_bool_t
nulang__vartype_equals (nulang__vartype_t a, nulang__vartype_t b)
{
    if (a.type == b.type)
    {
        switch (a.type)
        {
            case VARTYPE_ARCHETYPE:
                return a.value.archetype == b.value.archetype;
            case VARTYPE_PRIMITIVE:
                return a.value.primitive == b.value.primitive;
            case VARTYPE_UNKNOWN:
                NU_UNREACHABLE;
                break;
        }
    }
    return NU_FALSE;
}

#endif

#endif
