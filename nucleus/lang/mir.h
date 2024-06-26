#ifndef NULANG_MIR_H
#define NULANG_MIR_H

#include <nucleus/lang/error.h>
#include <nucleus/lang/allocator.h>
#include <nucleus/lang/lexer.h>
#include <nucleus/vm/types.h>

#define NULANG_FOREACH_AST(AST) \
    AST(AST_ROOT)               \
    AST(AST_COMPOUND)           \
    AST(AST_LITERAL)            \
    AST(AST_SYMBOL)             \
    AST(AST_TYPE)               \
    AST(AST_PRIMITIVE)          \
    AST(AST_FIELDLOOKUP)        \
    AST(AST_BREAK)              \
    AST(AST_CONTINUE)           \
    AST(AST_RETURN)             \
    AST(AST_IF)                 \
    AST(AST_IFBODY)             \
    AST(AST_FOR)                \
    AST(AST_WHILE)              \
    AST(AST_LOOP)               \
    AST(AST_VARDECL)            \
    AST(AST_CONSTDECL)          \
    AST(AST_CALL)               \
    AST(AST_ASSIGN)             \
    AST(AST_BINOP)              \
    AST(AST_UNOP)
typedef enum
{
    NULANG_FOREACH_AST(NULANG_GENERATE_ENUM)
} nulang_node_type_t;
static const nu_char_t *NULANG_AST_NAMES[]
    = { NULANG_FOREACH_AST(NULANG_GENERATE_NAME) };

#define NULANG_FOREACH_BINOP(BINOP) \
    BINOP(BINOP_ADD)                \
    BINOP(BINOP_SUB)                \
    BINOP(BINOP_MUL)                \
    BINOP(BINOP_DIV)                \
    BINOP(BINOP_EQUAL)              \
    BINOP(BINOP_NEQUAL)             \
    BINOP(BINOP_LEQUAL)             \
    BINOP(BINOP_GEQUAL)             \
    BINOP(BINOP_LESS)               \
    BINOP(BINOP_GREATER)            \
    BINOP(BINOP_AND)                \
    BINOP(BINOP_OR)
typedef enum
{
    NULANG_FOREACH_BINOP(NULANG_GENERATE_ENUM)
} nulang__binop_t;
static const nu_char_t *NULANG_BINOP_NAMES[]
    = { NULANG_FOREACH_BINOP(NULANG_GENERATE_NAME) };

#define NULANG_FOREACH_UNOP(UNOP) \
    UNOP(UNOP_NOT)                \
    UNOP(UNOP_NEG)
typedef enum
{
    NULANG_FOREACH_UNOP(NULANG_GENERATE_ENUM)
} nulang__unop_t;
static const nu_char_t *NULANG_UNOP_NAMES[]
    = { NULANG_FOREACH_UNOP(NULANG_GENERATE_NAME) };

#define NULANG_FOREACH_SYMBOL(SYMBOL) \
    SYMBOL(SYMBOL_FUNCTION)           \
    SYMBOL(SYMBOL_ARGUMENT)           \
    SYMBOL(SYMBOL_CONSTANT)           \
    SYMBOL(SYMBOL_VARIABLE)           \
    SYMBOL(SYMBOL_MODULE)             \
    SYMBOL(SYMBOL_EXTERNAL)           \
    SYMBOL(SYMBOL_UNKNOWN)
typedef enum
{
    NULANG_FOREACH_SYMBOL(NULANG_GENERATE_ENUM)
} nulang__symbol_type_t;
static const nu_char_t *NULANG_SYMBOL_NAMES[]
    = { NULANG_FOREACH_SYMBOL(NULANG_GENERATE_NAME) };

#define NULANG_FOREACH_BLOCK(BLOCK) \
    BLOCK(BLOCK_GLOBAL)             \
    BLOCK(BLOCK_FUNCTION)           \
    BLOCK(BLOCK_WHILE)              \
    BLOCK(BLOCK_FOR)                \
    BLOCK(BLOCK_IF)                 \
    BLOCK(BLOCK_LOOP)
typedef enum
{
    NULANG_FOREACH_BLOCK(NULANG_GENERATE_ENUM)
} nulang__block_type_t;
static const nu_char_t *NULANG_BLOCK_NAMES[]
    = { NULANG_FOREACH_BLOCK(NULANG_GENERATE_NAME) };

#define NULANG_FOREACH_VARTYPE(VARTYPE) \
    VARTYPE(VARTYPE_ENTITY)             \
    VARTYPE(VARTYPE_PRIMITIVE)          \
    VARTYPE(VARTYPE_UNKNOWN)
typedef enum
{
    NULANG_FOREACH_VARTYPE(NULANG_GENERATE_ENUM)
} nulang__vartype_type_t;
static const nu_char_t *NULANG_VARTYPE_NAMES[]
    = { NULANG_FOREACH_VARTYPE(NULANG_GENERATE_NAME) };

typedef nu_u32_t nulang__node_id_t;
typedef nu_u32_t nulang__symbol_id_t;
typedef nu_u32_t nulang__type_id_t;
typedef nu_u32_t nulang__block_id_t;
#define NULANG_NODE_NULL    0xffffffff
#define NULANG_SYMBOL_NULL  0xffffffff
#define NULANG_TYPE_NULL    0xffffffff
#define NULANG_BLOCK_NULL   0xffffffff
#define NULANG_BLOCK_GLOBAL 0

typedef union
{
    nu_primitive_t    primitive;
    nulang__type_id_t type;
} nulang__vartype_value_t;

typedef struct
{
    nulang__vartype_type_t  type;
    nulang__vartype_value_t value;
} nulang__vartype_t;

typedef union
{
    nulang__lit_t       literal;
    nulang__binop_t     binop;
    nulang__unop_t      unop;
    nulang__vartype_t   vartype;
    nu_primitive_t      primitive;
    nulang__type_id_t   type;
    nulang__symbol_id_t symbol;
    nulang__string_t    fieldlookup;
} nulang__node_value_t;

typedef struct
{
    nulang_node_type_t   type;
    nulang__node_value_t value;
    nulang__span_t       span;
    nulang__node_id_t    parent;
    nulang__node_id_t    first_child;
    nulang__node_id_t    last_child;
    nulang__node_id_t    next_sibling;
} nulang__node_t;

typedef struct
{
    nulang__node_id_t root;
    nulang__node_t   *nodes;
    nu_u32_t          node_capacity;
    nu_u32_t          node_count;
} nulang__ast_t;

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
    nu_size_t         symbol_count;
    nu_size_t         symbol_capacity;
    nulang__block_t  *blocks;
    nu_size_t         block_count;
    nu_size_t         block_capacity;
} nulang__symbol_table_t;

typedef struct
{
    nulang__string_t ident;
    nulang__span_t   span;
} nulang__type_t;

typedef struct
{

    nulang__type_t *types;
    nu_size_t       type_count;
    nu_size_t       type_capacity;
} nulang__type_table_t;

#ifdef NULANG_IMPL

static nulang_error_t
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
static nulang_error_t
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
static nulang_error_t
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
static nulang_error_t
nulang__lookup_symbol (nulang__symbol_table_t *table,
                       nulang__string_t        ident,
                       nulang__span_t          span,
                       nulang__block_id_t      block,
                       nulang__symbol_id_t    *id)
{
    nulang_error_t error;
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

static nulang_error_t
nulang__type_table_init (nu_size_t             type_capacity,
                         nulang__allocator_t  *alloc,
                         nulang__type_table_t *table)
{
    table->types = nulang__alloc(alloc, sizeof(nulang__type_t) * type_capacity);
    if (!table->types)
    {
        return NULANG_ERROR_OUT_OF_MEMORY;
    }
    table->type_capacity = type_capacity;
    table->type_count    = 0;
    return NULANG_ERROR_NONE;
}
static void
nulang__type_table_clear (nulang__type_table_t *table)
{
    table->type_count = 0;
}

static nulang_error_t
nulang__lookup_type (nulang__type_table_t *table,
                     nulang__string_t      ident,
                     nulang__span_t        span,
                     nulang__type_id_t    *type)
{
    nu_size_t i;
    for (i = 0; i < table->type_count; ++i)
    {
        if (NULANG_SOURCE_STRING_EQUALS(table->types[i].ident, ident))
        {
            *type = i;
            return NULANG_ERROR_NONE;
        }
    }
    *type = NULANG_TYPE_NULL;
    if (table->type_count >= table->type_capacity)
    {
        return NULANG_ERROR_OUT_OF_SYMBOL;
    }
    *type                     = table->type_count++;
    table->types[*type].ident = ident;
    table->types[*type].span  = span;
    return NULANG_ERROR_NONE;
}

static nulang_error_t
nulang__define_symbol (nulang__symbol_table_t *table,
                       nulang__symbol_type_t   type,
                       nulang__symbol_value_t  value,
                       nulang__string_t        ident,
                       nulang__span_t          span,
                       nulang__block_id_t      block,
                       nulang__symbol_id_t    *id)
{
    nulang_error_t      error;
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

static nulang_error_t
nulang__ast_init (nu_u32_t             node_capacity,
                  nulang__allocator_t *alloc,
                  nulang__ast_t       *ast)
{
    NU_ASSERT(node_capacity);
    ast->nodes = nulang__alloc(alloc, sizeof(nulang__node_t) * node_capacity);
    if (!ast->nodes)
    {
        return NULANG_ERROR_OUT_OF_MEMORY;
    }
    ast->node_capacity         = node_capacity;
    ast->node_count            = 1;
    ast->nodes[0].type         = AST_ROOT;
    ast->nodes[0].parent       = NULANG_NODE_NULL;
    ast->nodes[0].first_child  = NULANG_NODE_NULL;
    ast->nodes[0].last_child   = NULANG_NODE_NULL;
    ast->nodes[0].next_sibling = NULANG_NODE_NULL;
    ast->root                  = 0;

    return NULANG_ERROR_NONE;
}
static void
nulang__ast_clear (nulang__ast_t *ast)
{
    ast->node_count            = 1; /* keep root node */
    ast->nodes[0].parent       = NULANG_NODE_NULL;
    ast->nodes[0].first_child  = NULANG_NODE_NULL;
    ast->nodes[0].last_child   = NULANG_NODE_NULL;
    ast->nodes[0].next_sibling = NULANG_NODE_NULL;
}
static nulang_error_t
nulang__ast_add_node (nulang__ast_t *ast, nulang__node_id_t *id)
{
    nulang__node_t *node;
    if (ast->node_count >= ast->node_capacity)
    {
        return NULANG_ERROR_OUT_OF_NODE;
    }
    node               = &ast->nodes[ast->node_count];
    *id                = ast->node_count++;
    node->parent       = NULANG_NODE_NULL;
    node->first_child  = NULANG_NODE_NULL;
    node->last_child   = NULANG_NODE_NULL;
    node->next_sibling = NULANG_NODE_NULL;
    return NULANG_ERROR_NONE;
}

static void
nulang__ast_append_child (nulang__ast_t    *ast,
                          nulang__node_id_t parent,
                          nulang__node_id_t child)
{
    nulang__node_id_t last_child;
    NU_ASSERT(child != 0);
    NU_ASSERT(parent != child);
    NU_ASSERT(parent != NULANG_NODE_NULL && child != NULANG_NODE_NULL);
    last_child = ast->nodes[parent].last_child;
    NU_ASSERT(last_child != child);
    NU_ASSERT(last_child != parent);
    if (last_child != NULANG_NODE_NULL)
    {
        ast->nodes[last_child].next_sibling = child;
        ast->nodes[parent].last_child       = child;
    }
    else
    {
        ast->nodes[parent].first_child = child;
        ast->nodes[parent].last_child  = child;
    }
    ast->nodes[child].parent = parent;
}

static nu_bool_t
nulang__ast_is_statement (nulang_node_type_t t)
{
    nu_size_t                       i;
    static const nulang_node_type_t statements[]
        = { AST_COMPOUND, AST_RETURN,  AST_IF,        AST_FOR,   AST_WHILE,
            AST_LOOP,     AST_VARDECL, AST_CONSTDECL, AST_ASSIGN };
    for (i = 0; i < NU_ARRAY_SIZE(statements); ++i)
    {
        if (t == statements[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__ast_is_expression (nulang_node_type_t t)
{
    nu_size_t                       i;
    static const nulang_node_type_t expressions[]
        = { AST_LITERAL, AST_SYMBOL, AST_FIELDLOOKUP,
            AST_CALL,    AST_BINOP,  AST_UNOP };
    for (i = 0; i < NU_ARRAY_SIZE(expressions); ++i)
    {
        if (t == expressions[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__ast_is_loop (nulang_node_type_t t)
{
    nu_size_t                       i;
    static const nulang_node_type_t loops[] = { AST_FOR, AST_WHILE, AST_LOOP };
    for (i = 0; i < NU_ARRAY_SIZE(loops); ++i)
    {
        if (t == loops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__ast_is_binop (nulang__token_type_t t)
{
    nu_size_t                         i;
    static const nulang__token_type_t binops[]
        = { TOKEN_ADD,     TOKEN_MUL,    TOKEN_DIV,    TOKEN_EQUAL,
            TOKEN_NEQUAL,  TOKEN_LEQUAL, TOKEN_GEQUAL, TOKEN_LESS,
            TOKEN_GREATER, TOKEN_AND,    TOKEN_OR };
    for (i = 0; i < NU_ARRAY_SIZE(binops); ++i)
    {
        if (t == binops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nulang__binop_t
nulang__binop_from_token (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_ADD:
            return BINOP_ADD;
        case TOKEN_SUB:
            return BINOP_SUB;
        case TOKEN_MUL:
            return BINOP_MUL;
        case TOKEN_DIV:
            return BINOP_DIV;
        case TOKEN_EQUAL:
            return BINOP_EQUAL;
        case TOKEN_NEQUAL:
            return BINOP_NEQUAL;
        case TOKEN_LEQUAL:
            return BINOP_LEQUAL;
        case TOKEN_GEQUAL:
            return BINOP_GEQUAL;
        case TOKEN_LESS:
            return BINOP_LESS;
        case TOKEN_GREATER:
            return BINOP_GREATER;
        case TOKEN_AND:
            return BINOP_AND;
        case TOKEN_OR:
            return BINOP_OR;
        default:
            return BINOP_ADD;
    }
}
static nu_bool_t
nulang__ast_is_unop (nulang__token_type_t t)
{
    nu_size_t                         i;
    static const nulang__token_type_t unops[] = { TOKEN_SUB, TOKEN_NOT };
    for (i = 0; i < NU_ARRAY_SIZE(unops); ++i)
    {
        if (t == unops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nulang__unop_t
nulang__unop_from_token (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_SUB:
            return UNOP_NEG;
        case TOKEN_NOT:
            return UNOP_NOT;
        default:
            return UNOP_NEG;
    }
}
static nu_u16_t
nulang__binop_precedence (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_ADD:
        case TOKEN_SUB:
            return 1;
        case TOKEN_MUL:
        case TOKEN_DIV:
            return 2;
        case TOKEN_EQUAL:
        case TOKEN_NEQUAL:
        case TOKEN_LEQUAL:
        case TOKEN_GEQUAL:
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_AND:
        case TOKEN_OR:
            return 3;
        default:
            NU_ASSERT(NU_FALSE);
            return 0;
    }
}
static nu_bool_t
nulang__binop_is_left_associative (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_ADD:
        case TOKEN_SUB:
        case TOKEN_MUL:
        case TOKEN_DIV:
            return NU_TRUE;
        default:
            return NU_FALSE;
    }
}

#endif

#endif
