#ifndef NU_PARSER_H
#define NU_PARSER_H

#include <nucleus/compiler/error.h>
#include <nucleus/compiler/allocator.h>
#include <nucleus/compiler/lexer.h>
#include <nucleus/vm/error.h>

#ifdef NU_IMPLEMENTATION

#define NU_FOREACH_AST(AST) \
    AST(AST_ROOT)           \
    AST(AST_COMPOUND)       \
    AST(AST_LITERAL)        \
    AST(AST_IDENTIFIER)     \
    AST(AST_PRIMITIVE)      \
    AST(AST_FIELDLOOKUP)    \
    AST(AST_BREAK)          \
    AST(AST_CONTINUE)       \
    AST(AST_RETURN)         \
    AST(AST_IF)             \
    AST(AST_IFBODY)         \
    AST(AST_FOR)            \
    AST(AST_WHILE)          \
    AST(AST_LOOP)           \
    AST(AST_VARDECL)        \
    AST(AST_CONSTDECL)      \
    AST(AST_CALL)           \
    AST(AST_ASSIGN)         \
    AST(AST_BINOP)          \
    AST(AST_UNOP)
typedef enum
{
    NU_FOREACH_AST(NU_GENERATE_ENUM)
} nu__ast_type_t;
static const nu_char_t *NU_AST_NAMES[] = { NU_FOREACH_AST(NU_GENERATE_NAME) };

typedef nu_u32_t nu__ast_node_id_t;
typedef nu_u32_t nu__symbol_id_t;
typedef nu_u32_t nu__block_id_t;
#define NU_AST_NODE_NULL 0xffffffff
#define NU_SYMBOL_NULL   0xffffffff
#define NU_BLOCK_NULL    0xffffffff
#define NU_BLOCK_GLOBAL  0

typedef union
{
    nu__lit_t           literal;
    nu__token_type_t    binop;
    nu__token_type_t    unop;
    nu_type_t           primitive;
    nu__symbol_id_t     symbol;
    nu__source_string_t fieldlookup;
} nu__ast_value_t;

typedef struct
{
    nu__ast_type_t    type;
    nu__ast_value_t   value;
    nu__source_span_t span;
    nu__ast_node_id_t parent;
    nu__ast_node_id_t first_child;
    nu__ast_node_id_t last_child;
    nu__ast_node_id_t next_sibling;
} nu__ast_node_t;

typedef struct
{
    nu__ast_node_id_t root;
    nu__ast_node_t   *nodes;
    nu_u32_t          node_capacity;
    nu_u32_t          node_count;
} nu__ast_t;

#define NU_FOREACH_SYMBOL(SYMBOL) \
    SYMBOL(SYMBOL_FUNCTION)       \
    SYMBOL(SYMBOL_ARGUMENT)       \
    SYMBOL(SYMBOL_CONSTANT)       \
    SYMBOL(SYMBOL_LOCAL)          \
    SYMBOL(SYMBOL_MODULE)         \
    SYMBOL(SYMBOL_EXTERNAL)       \
    SYMBOL(SYMBOL_UNKNOWN)
typedef enum
{
    NU_FOREACH_SYMBOL(NU_GENERATE_ENUM)
} nu__symbol_type_t;
static const nu_char_t *NU_SYMBOL_NAMES[]
    = { NU_FOREACH_SYMBOL(NU_GENERATE_NAME) };

#define NU_FOREACH_BLOCK(BLOCK) \
    BLOCK(BLOCK_GLOBAL)         \
    BLOCK(BLOCK_FUNCTION)       \
    BLOCK(BLOCK_WHILE)          \
    BLOCK(BLOCK_FOR)            \
    BLOCK(BLOCK_IF)             \
    BLOCK(BLOCK_LOOP)
typedef enum
{
    NU_FOREACH_BLOCK(NU_GENERATE_ENUM)
} nu__block_type_t;
static const nu_char_t *NU_BLOCK_NAMES[]
    = { NU_FOREACH_BLOCK(NU_GENERATE_NAME) };

typedef struct
{
    nu_type_t       return_type;
    nu__symbol_id_t first_arg;
    nu_bool_t       exported;
} nu__symbol_function_t;
typedef struct
{
    nu_type_t       type;
    nu__symbol_id_t next;
} nu__symbol_argument_t;
typedef struct
{
    nu_type_t type;
    nu_bool_t exported;
} nu__symbol_constant_t;
typedef struct
{
    nu_type_t type;
} nu__symbol_local_t;
typedef struct
{
    nu__symbol_id_t symbol;
} nu__symbol_external_t;

typedef union
{
    int                   ub;
    nu__symbol_function_t function;
    nu__symbol_argument_t argument;
    nu__symbol_constant_t constant;
    nu__symbol_local_t    local;
    nu__symbol_external_t external;
} nu__symbol_value_t;

typedef struct
{
    nu__block_type_t type;
    nu__block_id_t   parent;
    nu__block_id_t   last;
    nu__block_id_t   previous_scope_symbol;
} nu__block_t;

typedef struct
{
    nu__symbol_type_t   type;
    nu__symbol_value_t  value;
    nu__source_string_t ident;
    nu__source_span_t   span;
    nu__block_id_t      block;
    nu__symbol_id_t     previous_in_block;
    nu__symbol_id_t     previous_in_scope;
} nu__symbol_t;

typedef struct
{
    nu__symbol_t *symbols;
    nu_size_t     symbol_count;
    nu_size_t     symbol_capacity;
    nu__block_t  *blocks;
    nu_size_t     block_count;
    nu_size_t     block_capacity;
} nu__symbol_table_t;

typedef struct
{
    nu__source_span_t span;
    nu__token_type_t  expect;
    nu__token_type_t  got;
} nu__parser_unexpected_token_t;
typedef struct
{
    nu__source_span_t span;
} nu__parser_symbol_already_defined_t;
typedef struct
{
    nu__token_type_t  got;
    nu__source_span_t span;
} nu__parser_invalid_atom_expression_t;
typedef struct
{
    nu__source_span_t span;
} nu__parser_unexpected_binop_t;
typedef struct
{
    nu__token_type_t  got;
    nu__source_span_t span;
} nu__parser_non_statement_token_t;

typedef struct
{
    nu__parser_unexpected_token_t        unexpected_token;
    nu__parser_symbol_already_defined_t  symbol_already_defined;
    nu__parser_invalid_atom_expression_t invalid_atom_expression;
    nu__parser_unexpected_binop_t        unexpected_binop;
    nu__parser_non_statement_token_t     non_statement_token;
} nu__parser_error_t;

typedef struct
{
    nu__parser_error_t  error;
    nu__lexer_t        *lexer;
    nu__ast_t          *ast;
    nu__symbol_table_t *symbols;
} nu__parser_t;

static nu_compiler_error_t
nu__symbol_table_init (nu_size_t                 symbol_capacity,
                       nu_size_t                 block_capacity,
                       nu__compiler_allocator_t *alloc,
                       nu__symbol_table_t       *table)
{
    table->symbols
        = nu__compiler_alloc(alloc, sizeof(nu__symbol_t) * symbol_capacity);
    if (!table->symbols)
    {
        return NU_COMPERR_OUT_OF_MEMORY;
    }
    table->symbol_capacity = symbol_capacity;
    table->symbol_count    = 0;
    table->blocks
        = nu__compiler_alloc(alloc, sizeof(nu__block_t) * block_capacity);
    if (!table->blocks)
    {
        return NU_COMPERR_OUT_OF_MEMORY;
    }
    table->block_capacity = block_capacity;
    table->block_count    = 0;

    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__symbol_add (nu__symbol_table_t *table,
                nu__symbol_type_t   type,
                nu__symbol_value_t  value,
                nu__source_string_t ident,
                nu__source_span_t   span,
                nu__block_id_t      block,
                nu__symbol_id_t    *id)
{
    nu__block_id_t last_block;
    if (table->symbol_count >= table->symbol_capacity)
    {
        return NU_COMPERR_OUT_OF_SYMBOL;
    }
    *id                       = table->symbol_count++;
    table->symbols[*id].type  = type;
    table->symbols[*id].value = value;
    table->symbols[*id].ident = ident;
    table->symbols[*id].span  = span;

    /* update previous in scope */
    last_block = table->blocks[block].last;
    if (last_block != NU_BLOCK_NULL)
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

    return NU_COMPERR_NONE;
}
static nu_bool_t
nu__find_symbol_in_block (const nu__symbol_table_t *table,
                          nu__block_id_t            block,
                          nu__source_string_t       name,
                          nu__symbol_id_t          *id)
{
    nu__symbol_id_t symbol;
    symbol = table->blocks[block].last;
    while (symbol != NU_SYMBOL_NULL)
    {
        if (NU_SOURCE_STRING_EQUALS(table->symbols[symbol].ident, name))
        {
            *id = symbol;
            return NU_TRUE;
        }
        symbol = table->symbols[symbol].previous_in_block;
    }
    return NU_FALSE;
}
static void
nu__find_symbol_in_scope (const nu__symbol_table_t *table,
                          nu__block_id_t            block,
                          nu__source_string_t       name,
                          nu__symbol_id_t          *id)
{
    nu__symbol_id_t symbol;
    symbol = table->blocks[block].last;
    if (symbol == NU_BLOCK_NULL)
    {
        symbol = table->blocks[block].previous_scope_symbol;
    }
    while (symbol != NU_SYMBOL_NULL)
    {
        if (NU_SOURCE_STRING_EQUALS(table->symbols[symbol].ident, name))
        {
            *id = symbol;
        }
        symbol = table->symbols[symbol].previous_in_scope;
    }
    *id = NU_SYMBOL_NULL;
}
static nu_bool_t
nu__check_in_loop (const nu__symbol_table_t *table, nu__block_id_t block)
{
    static nu__block_type_t types[] = { BLOCK_LOOP, BLOCK_WHILE, BLOCK_FOR };
    while (block != NU_BLOCK_NULL)
    {
        nu_size_t        i;
        nu__block_type_t type = table->blocks[block].type;
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
nu__check_in_function (const nu__symbol_table_t *table, nu__block_id_t block)
{
    while (block != NU_BLOCK_NULL)
    {
        if (table->blocks[block].type == BLOCK_FUNCTION)
        {
            block = table->blocks[block].parent;
        }
    }
    return NU_FALSE;
}
static nu_compiler_error_t
nu__add_block (nu__symbol_table_t *table,
               nu__block_type_t    type,
               nu__block_id_t      parent,
               nu__block_id_t     *id)
{
    nu__symbol_id_t previous_scope_symbol;

    if (table->block_count >= table->block_capacity)
    {
        return NU_COMPERR_OUT_OF_BLOCK;
    }

    *id = table->block_count++;

    previous_scope_symbol = NU_SYMBOL_NULL;
    if (parent != NU_BLOCK_NULL)
    {
        previous_scope_symbol = table->blocks[parent].last;
        if (previous_scope_symbol == NU_SYMBOL_NULL)
        {
            previous_scope_symbol = table->blocks[parent].previous_scope_symbol;
        }
    }

    table->blocks[*id].type                  = type;
    table->blocks[*id].parent                = parent;
    table->blocks[*id].last                  = NU_SYMBOL_NULL;
    table->blocks[*id].previous_scope_symbol = previous_scope_symbol;

    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__lookup_symbol (nu__symbol_table_t *table,
                   nu__source_string_t ident,
                   nu__source_span_t   span,
                   nu__block_id_t      block,
                   nu__symbol_id_t    *id)
{
    nu_compiler_error_t error;
    nu__find_symbol_in_scope(table, block, ident, id);
    if (*id == NU_SYMBOL_NULL)
    {
        nu__symbol_value_t unknown_value;
        unknown_value.ub = 0;
        error            = nu__symbol_add(table,
                               SYMBOL_UNKNOWN,
                               unknown_value,
                               ident,
                               span,
                               NU_BLOCK_GLOBAL,
                               id);
        NU_COMPERR_CHECK(error);
    }
    return NU_COMPERR_NONE;
}

static nu_compiler_error_t
nu__define_symbol (nu__symbol_table_t *table,
                   nu__symbol_type_t   type,
                   nu__symbol_value_t  value,
                   nu__source_string_t ident,
                   nu__source_span_t   span,
                   nu__block_id_t      block,
                   nu__parser_error_t *error_data,
                   nu__symbol_id_t    *id)
{
    nu_compiler_error_t error;
    nu__symbol_id_t     found;
    /* check existing symbol */
    nu__find_symbol_in_scope(table, block, ident, &found);
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
        case SYMBOL_LOCAL:
            NU_ASSERT(block == BLOCK_GLOBAL);
            if (found != NU_SYMBOL_NULL)
            {
                /* constant and function shadowing is not allowed, we must check
                 * definition */
                if (table->symbols[found].type != SYMBOL_UNKNOWN)
                {
                    error_data->symbol_already_defined.span = span;
                    return NU_COMPERR_SYMBOL_ALREADY_DEFINED;
                }
                else
                {
                    /*update symbol */
                    table->symbols[found].type  = type;
                    table->symbols[found].value = value;
                }
            }
            else
            {
                /* not defined or declared */
                error = nu__symbol_add(
                    table, type, value, ident, span, block, id);
                NU_COMPERR_CHECK(error);
            }
            break;
        case SYMBOL_ARGUMENT:
            /* fall-through */
        case SYMBOL_UNKNOWN:
            /* may shadow previous symbol */
            error = nu__symbol_add(table, type, value, ident, span, block, id);
            NU_COMPERR_CHECK(error);
            break;
    }
    return NU_COMPERR_NONE;
}

static nu_compiler_error_t
nu__ast_init (nu_u32_t                  node_capacity,
              nu__compiler_allocator_t *alloc,
              nu__ast_t                *ast)
{
    NU_ASSERT(node_capacity);
    ast->nodes
        = nu__compiler_alloc(alloc, sizeof(nu__ast_node_t) * node_capacity);
    if (!ast->nodes)
    {
        return NU_COMPERR_OUT_OF_MEMORY;
    }
    ast->node_capacity        = node_capacity;
    ast->node_count           = 1;
    ast->nodes[0].type        = AST_ROOT;
    ast->nodes[0].parent      = NU_AST_NODE_NULL;
    ast->nodes[0].first_child = NU_AST_NODE_NULL;
    ast->nodes[0].last_child  = NU_AST_NODE_NULL;
    ast->root                 = 0;

    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__ast_add_node (nu__ast_t *ast, nu__ast_node_id_t *id)
{
    nu__ast_node_t *node;
    if (ast->node_count >= ast->node_capacity)
    {
        return NU_COMPERR_OUT_OF_NODE;
    }
    node = &ast->nodes[ast->node_count];
    *id  = ast->node_count;
    ast->node_count++;
    node->parent      = NU_AST_NODE_NULL;
    node->first_child = NU_AST_NODE_NULL;
    node->last_child  = NU_AST_NODE_NULL;
    return NU_COMPERR_NONE;
}

static void
nu__ast_append_child (nu__ast_t        *ast,
                      nu__ast_node_id_t parent,
                      nu__ast_node_id_t child)
{
    nu__ast_node_id_t last_child;
    NU_ASSERT(parent && child);
    last_child = ast->nodes[parent].last_child;
    if (last_child != NU_AST_NODE_NULL)
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
nu__ast_is_statement (nu__ast_type_t t)
{
    nu_size_t                   i;
    static const nu__ast_type_t statements[]
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
nu__ast_is_expression (nu__ast_type_t t)
{
    nu_size_t                   i;
    static const nu__ast_type_t expressions[]
        = { AST_LITERAL, AST_IDENTIFIER, AST_FIELDLOOKUP,
            AST_CALL,    AST_BINOP,      AST_UNOP };
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
nu__ast_is_loop (nu__ast_type_t t)
{
    nu_size_t                   i;
    static const nu__ast_type_t loops[] = { AST_FOR, AST_WHILE, AST_LOOP };
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
nu__ast_is_binop (nu__token_type_t t)
{
    nu_size_t                     i;
    static const nu__token_type_t binops[]
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
static nu_bool_t
nu__ast_is_unop (nu__token_type_t t)
{
    nu_size_t                     i;
    static const nu__token_type_t unops[] = { TOKEN_SUB, TOKEN_NOT };
    for (i = 0; i < NU_ARRAY_SIZE(unops); ++i)
    {
        if (t == unops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_u16_t
nu__binop_precedence (nu__token_type_t t)
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
nu__binop_is_left_associative (nu__token_type_t t)
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

static nu_compiler_error_t nu__parse_expression(nu__parser_t  *parser,
                                                nu_u16_t       min_precedence,
                                                nu__block_id_t block,
                                                nu__ast_node_id_t *node);
static nu_compiler_error_t
nu__parser_peek (nu__parser_t *parser, nu_size_t lookahead, nu__token_t *token)
{
    return nu__lexer_peek(parser->lexer, lookahead, token);
}
static nu_compiler_error_t
nu__parser_consume (nu__parser_t *parser, nu__token_t *token)
{
    return nu__lexer_next(parser->lexer, token);
}
static nu_compiler_error_t
nu__parser_expect (nu__parser_t    *parser,
                   nu__token_type_t type,
                   nu__token_t     *token)
{
    nu__token_t         tok;
    nu_compiler_error_t error;
    error = nu__parser_consume(parser, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type != type)
    {
        parser->error.unexpected_token.got    = tok.type;
        parser->error.unexpected_token.expect = type;
        parser->error.unexpected_token.span   = tok.span;
        return NU_COMPERR_UNEXPECTED_TOKEN;
    }
    *token = tok;
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parser_accept (nu__parser_t    *parser,
                   nu__token_type_t type,
                   nu__token_t     *token,
                   nu_bool_t       *found)
{
    nu__token_t         tok;
    nu_compiler_error_t error;
    error = nu__lexer_peek(parser->lexer, 0, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type == type)
    {
        *found = NU_TRUE;
        return nu__lexer_next(parser->lexer, token);
    }
    *found = NU_FALSE;
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__try_parse_primitive (nu__parser_t *parser,
                         nu_type_t    *primitive,
                         nu_bool_t    *found)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    error = nu__parser_accept(parser, TOKEN_COLON, &tok, found);
    NU_COMPERR_CHECK(error);
    if (*found)
    {
        error = nu__parser_expect(parser, TOKEN_PRIMITIVE, &tok);
        NU_COMPERR_CHECK(error);
        *primitive = tok.value.primitive;
    }
    return NU_COMPERR_NONE;
}
typedef nu_compiler_error_t (*nu__parse_identifier_list_pfn_t)(
    const nu__token_t *, void *);
static nu_compiler_error_t
nu__parse_identifier_list (nu__parser_t                   *parser,
                           nu__token_type_t                separator,
                           nu_size_t                      *count,
                           nu__parse_identifier_list_pfn_t callback,
                           void                           *userdata)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    nu_bool_t           found;
    *count = 0;
    error  = nu__parser_accept(parser, TOKEN_IDENTIFIER, &tok, &found);
    NU_COMPERR_CHECK(error);
    if (found)
    {
        error = callback(&tok, userdata);
        NU_COMPERR_CHECK(error);
        (*count)++;
        for (;;)
        {
            error = nu__parser_accept(parser, separator, &tok, &found);
            NU_COMPERR_CHECK(error);
            if (!found)
            {
                break;
            }
            error = nu__parser_expect(parser, TOKEN_LITERAL, &tok);
            NU_COMPERR_CHECK(error);
            error = callback(&tok, userdata);
            NU_COMPERR_CHECK(error);
            (*count)++;
        }
    }
    return NU_COMPERR_NONE;
}
typedef nu_compiler_error_t (*nu__parse_function_argument_list_pfn_t)(
    const nu__token_t *, nu_type_t, void *);
static nu_compiler_error_t
nu__parse_function_argument_list (
    nu__parser_t                          *parser,
    nu__token_type_t                       separator,
    nu_size_t                             *count,
    nu__parse_function_argument_list_pfn_t callback,
    void                                  *userdata)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    nu_bool_t           found;
    *count = 0;
    error  = nu__parser_accept(parser, TOKEN_IDENTIFIER, &tok, &found);
    NU_COMPERR_CHECK(error);
    if (found)
    {
        nu_type_t primitive;
        error = nu__try_parse_primitive(parser, &primitive, &found);
        NU_COMPERR_CHECK(error);
        error = callback(&tok, primitive, userdata);
        NU_COMPERR_CHECK(error);
        (*count)++;
        for (;;)
        {
            error = nu__parser_accept(parser, separator, &tok, &found);
            NU_COMPERR_CHECK(error);
            if (!found)
            {
                break;
            }
            error = nu__try_parse_primitive(parser, &primitive, &found);
            NU_COMPERR_CHECK(error);
            error = callback(&tok, primitive, userdata);
            NU_COMPERR_CHECK(error);
            (*count)++;
        }
    }
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_field_lookup_chain (nu__parser_t      *parser,
                              nu__ast_node_id_t  child,
                              nu__ast_node_id_t *parent)
{
    nu_compiler_error_t error;
    nu__ast_node_id_t   node;
    nu__token_t         tok;
    error = nu__parser_expect(parser, TOKEN_DOT, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__ast_add_node(parser->ast, &node);
    NU_COMPERR_CHECK(error);
    parser->ast->nodes[node].type              = AST_FIELDLOOKUP;
    parser->ast->nodes[node].span              = tok.span;
    parser->ast->nodes[node].value.fieldlookup = tok.value.identifier;
    nu__ast_append_child(parser->ast, node, child);
    error = nu__parser_peek(parser, 0, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type == TOKEN_DOT)
    {
        return nu__parse_field_lookup_chain(parser, node, parent);
    }
    else
    {
        *parent = node;
        return NU_COMPERR_NONE;
    }
}
static nu_compiler_error_t
nu__try_parse_field_lookup_chain (nu__parser_t      *parser,
                                  nu__ast_node_id_t  node,
                                  nu__ast_node_id_t *parent)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    error = nu__parser_peek(parser, 0, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type == TOKEN_DOT)
    {
        return nu__parse_field_lookup_chain(parser, node, parent);
    }
    else
    {
        *parent = node;
        return NU_COMPERR_NONE;
    }
}
static nu_compiler_error_t
nu__parse_identifier (nu__parser_t      *parser,
                      nu__ast_node_id_t  block,
                      nu__ast_node_id_t *node)
{
    nu_compiler_error_t error;
    nu__symbol_id_t     symbol;
    nu__token_t         tok;
    error = nu__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__lookup_symbol(
        parser->symbols, tok.value.identifier, tok.span, block, &symbol);
    NU_COMPERR_CHECK(error);
    error = nu__ast_add_node(parser->ast, node);
    NU_COMPERR_CHECK(error);
    parser->ast->nodes[*node].type         = AST_IDENTIFIER;
    parser->ast->nodes[*node].value.symbol = symbol;
    parser->ast->nodes[*node].span         = tok.span;
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_call (nu__parser_t      *parser,
                nu__ast_node_id_t  ident,
                nu__ast_node_id_t  block,
                nu__ast_node_id_t *call)
{
    nu_compiler_error_t error;
    nu__ast_node_id_t   node, expr;
    nu__token_t         tok;
    nu_bool_t           found;
    error = nu__ast_add_node(parser->ast, &node);
    NU_COMPERR_CHECK(error);
    parser->ast->nodes[node].type = AST_CALL;
    nu__ast_append_child(parser->ast, node, ident);
    error = nu__parser_expect(parser, TOKEN_LPAREN, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__lexer_peek(parser->lexer, 0, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type != TOKEN_RPAREN)
    {
        error = nu__parse_expression(parser, 0, block, &expr);
        NU_COMPERR_CHECK(error);
        nu__ast_append_child(parser->ast, node, expr);
    }
    for (;;)
    {
        error = nu__parser_accept(parser, TOKEN_COMMA, &tok, &found);
        NU_COMPERR_CHECK(error);
        if (!found)
        {
            break;
        }
        error = nu__parse_expression(parser, 0, block, &expr);
        NU_COMPERR_CHECK(error);
        nu__ast_append_child(parser->ast, node, expr);
    }
    error = nu__parser_expect(parser, TOKEN_RPAREN, &tok);
    NU_COMPERR_CHECK(error);
    *call = node;
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__try_parse_call (nu__parser_t      *parser,
                    nu__ast_node_id_t  node,
                    nu__block_id_t     block,
                    nu__ast_node_id_t *id)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    error = nu__parser_peek(parser, 0, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type == TOKEN_LPAREN)
    {
        error = nu__parse_call(parser, node, block, id);
        NU_COMPERR_CHECK(error);
    }
    else
    {
        *id = node;
    }
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_atom (nu__parser_t      *parser,
                nu__block_id_t     block,
                nu__ast_node_id_t *node)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    error = nu__parser_peek(parser, 0, &tok);
    NU_COMPERR_CHECK(error);
    switch (tok.type)
    {
        case TOKEN_IDENTIFIER:
            error = nu__parse_identifier(parser, block, node);
            NU_COMPERR_CHECK(error);
            error = nu__try_parse_field_lookup_chain(parser, *node, node);
            NU_COMPERR_CHECK(error);
            error = nu__try_parse_call(parser, *node, block, node);
            NU_COMPERR_CHECK(error);
            break;
        case TOKEN_PRIMITIVE:
            error = nu__parser_expect(parser, TOKEN_PRIMITIVE, &tok);
            NU_COMPERR_CHECK(error);
            error = nu__ast_add_node(parser->ast, node);
            NU_COMPERR_CHECK(error);
            parser->ast->nodes[*node].type            = AST_PRIMITIVE;
            parser->ast->nodes[*node].span            = tok.span;
            parser->ast->nodes[*node].value.primitive = tok.value.primitive;
            error = nu__try_parse_field_lookup_chain(parser, *node, node);
            NU_COMPERR_CHECK(error);
            error = nu__try_parse_call(parser, *node, block, node);
            NU_COMPERR_CHECK(error);
            break;
        case TOKEN_LITERAL:
            error = nu__parser_consume(parser, &tok);
            NU_COMPERR_CHECK(error);
            error = nu__ast_add_node(parser->ast, node);
            NU_COMPERR_CHECK(error);
            parser->ast->nodes[*node].type          = AST_LITERAL;
            parser->ast->nodes[*node].span          = tok.span;
            parser->ast->nodes[*node].value.literal = tok.value.literal;
            break;
        default:
            parser->error.invalid_atom_expression.span = tok.span;
            parser->error.invalid_atom_expression.got  = tok.type;
            return NU_COMPERR_INVALID_ATOM_EXPRESSION;
    }
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_primary (nu__parser_t      *parser,
                   nu__block_id_t     block,
                   nu__ast_node_id_t *node)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    error = nu__parser_peek(parser, 0, &tok);
    NU_COMPERR_CHECK(error);
    if (tok.type == TOKEN_LPAREN)
    {
        error = nu__parser_consume(parser, &tok);
        NU_COMPERR_CHECK(error);
        error = nu__parse_expression(parser, 1, block, node);
        NU_COMPERR_CHECK(error);
        error = nu__parser_expect(parser, TOKEN_RPAREN, &tok);
        NU_COMPERR_CHECK(error);
    }
    else if (nu__ast_is_unop(tok.type))
    {
        nu__ast_node_id_t expr;
        error = nu__parser_consume(parser, &tok);
        NU_COMPERR_CHECK(error);
        error = nu__parse_primary(parser, block, &expr);
        NU_COMPERR_CHECK(error);
        error = nu__ast_add_node(parser->ast, node);
        NU_COMPERR_CHECK(error);
        parser->ast->nodes[*node].type       = AST_UNOP;
        parser->ast->nodes[*node].value.unop = tok.type;
        parser->ast->nodes[*node].span       = tok.span;
        nu__ast_append_child(parser->ast, *node, expr);
    }
    else if (nu__ast_is_binop(tok.type))
    {
        parser->error.unexpected_binop.span = tok.span;
        return NU_COMPERR_UNEXPECTED_BINOP;
    }
    else
    {
        error = nu__parse_atom(parser, block, node);
        NU_COMPERR_CHECK(error);
    }
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_expression (nu__parser_t      *parser,
                      nu_u16_t           min_precedence,
                      nu__block_id_t     block,
                      nu__ast_node_id_t *node)
{
    nu__ast_node_id_t   lhs;
    nu_compiler_error_t error;
    error = nu__parse_primary(parser, block, &lhs);
    NU_COMPERR_CHECK(error);
    for (;;)
    {
        nu__token_t tok;
        error = nu__parser_peek(parser, 0, &tok);
        NU_COMPERR_CHECK(error);
        if (nu__ast_is_binop(tok.type))
        {
            nu_u16_t          next_min_assoc;
            nu__ast_node_id_t op_node, rhs;
            nu_u16_t          prec = nu__binop_precedence(tok.type);
            nu_bool_t         is_left_associative
                = nu__binop_is_left_associative(tok.type);
            NU_ASSERT(prec);

            if (prec < min_precedence)
            {
                break;
            }
            if (is_left_associative)
            {
                next_min_assoc = prec + 1;
            }
            else
            {
                next_min_assoc = prec;
            }

            error = nu__parser_consume(parser, &tok);
            NU_COMPERR_CHECK(error);
            error = nu__parse_expression(parser, next_min_assoc, block, &rhs);
            NU_COMPERR_CHECK(error);

            error = nu__ast_add_node(parser->ast, &op_node);
            NU_COMPERR_CHECK(error);
            parser->ast->nodes[op_node].type        = AST_BINOP;
            parser->ast->nodes[op_node].value.binop = tok.type;
            parser->ast->nodes[op_node].span        = tok.span;
            nu__ast_append_child(parser->ast, op_node, lhs);
            nu__ast_append_child(parser->ast, op_node, rhs);
            lhs = op_node;
        }
        else
        {
            break;
        }
    }
    *node = lhs;
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_variable_declaration (nu__parser_t      *parser,
                                nu__block_id_t     block,
                                nu__ast_node_id_t *node)
{
    nu_compiler_error_t error;
    nu__token_t         tok, ident;
    nu_type_t           primitive;
    nu_bool_t           found;
    nu__ast_node_id_t   expr;
    nu__symbol_id_t     symbol;
    nu__symbol_value_t  symbol_value;
    error = nu__parser_expect(parser, TOKEN_LOCAL, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__parser_expect(parser, TOKEN_IDENTIFIER, &ident);
    NU_COMPERR_CHECK(error);
    error = nu__try_parse_primitive(parser, &primitive, &found);
    NU_COMPERR_CHECK(error);
    error = nu__parser_expect(parser, TOKEN_ASSIGN, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__parse_expression(parser, 0, block, &expr);
    NU_COMPERR_CHECK(error);
    symbol_value.local.type = primitive;
    error                   = nu__define_symbol(parser->symbols,
                              SYMBOL_LOCAL,
                              symbol_value,
                              ident.value.identifier,
                              ident.span,
                              block,
                              &parser->error,
                              &symbol);
    NU_COMPERR_CHECK(error);
    error = nu__ast_add_node(parser->ast, node);
    NU_COMPERR_CHECK(error);
    parser->ast->nodes[*node].type         = AST_VARDECL;
    parser->ast->nodes[*node].value.symbol = symbol;
    parser->ast->nodes[*node].span         = ident.span;
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse_statement (nu__parser_t      *parser,
                     nu__block_id_t     block,
                     nu__ast_node_id_t *node)
{
    nu_compiler_error_t error;
    nu__token_t         tok;
    error = nu__parser_peek(parser, 0, &tok);
    NU_COMPERR_CHECK(error);
    switch (tok.type)
    {
        case TOKEN_LOCAL:
            error = nu__parse_variable_declaration(parser, block, node);
            NU_COMPERR_CHECK(error);
            break;
        case TOKEN_EXPORT:
            break;
        case TOKEN_CONST:
            break;
        case TOKEN_FUNCTION:
            break;
        case TOKEN_RETURN:
            break;
        case TOKEN_IF:
            break;
        case TOKEN_FOR:
            break;
        case TOKEN_WHILE:
            break;
        case TOKEN_BREAK:
            break;
        case TOKEN_CONTINUE:
            break;
        case TOKEN_IDENTIFIER:
            break;
        default:
            parser->error.non_statement_token.span = tok.span;
            parser->error.non_statement_token.got  = tok.type;
            return NU_COMPERR_NON_STATEMENT_TOKEN;
    }
    return NU_COMPERR_NONE;
}
static nu_compiler_error_t
nu__parse (nu__parser_t *parser)
{
    nu__block_id_t      global_block;
    nu_compiler_error_t error;
    nu__ast_node_id_t   stmt;
    nu__token_t         tok;
    error = nu__add_block(
        parser->symbols, BLOCK_GLOBAL, NU_BLOCK_NULL, &global_block);
    NU_COMPERR_CHECK(error);
    for (;;)
    {
        error = nu__parser_peek(parser, 0, &tok);
        NU_COMPERR_CHECK(error);
        if (tok.type == TOKEN_IMPORT)
        {
        }
        else if (tok.type == TOKEN_FROM)
        {
        }
        else
        {
            break;
        }
    }
    for (;;)
    {
        error = nu__parser_peek(parser, 0, &tok);
        NU_COMPERR_CHECK(error);
        if (tok.type == TOKEN_EOF)
        {
            break;
        }
        error = nu__parse_statement(parser, global_block, &stmt);
        NU_COMPERR_CHECK(error);
        nu__ast_append_child(parser->ast, parser->ast->root, stmt);
    }
    error = nu__parser_expect(parser, TOKEN_EOF, &tok);
    NU_COMPERR_CHECK(error);
    return NU_COMPERR_NONE;
}

#endif

#endif
