#ifndef NU_PARSER_H
#define NU_PARSER_H

#include "nucleus/vm/types.h"
#include <nucleus/compiler/error.h>
#include <nucleus/compiler/allocator.h>
#include <nucleus/vm/error.h>
#include <nucleus/compiler/lexer.h>

#ifdef NU_IMPLEMENTATION

#define NU_FOREACH_BINOP(BINOP) \
    BINOP(BINOP_ADD)            \
    BINOP(BINOP_SUB)            \
    BINOP(BINOP_MUL)            \
    BINOP(BINOP_DIV)            \
    BINOP(BINOP_EQ)             \
    BINOP(BINOP_NEQ)            \
    BINOP(BINOP_LEQ)            \
    BINOP(BINOP_GEQ)            \
    BINOP(BINOP_LESS)           \
    BINOP(BINOP_GREATER)        \
    BINOP(BINOP_AND)            \
    BINOP(BINOP_NOT)

typedef enum
{
    NU_FOREACH_BINOP(NU_GENERATE_ENUM)
} nu__binop_type_t;
static const nu_char_t *NU_BINOP_NAMES[]
    = { NU_FOREACH_BINOP(NU_GENERATE_NAME) };

#define NU_FOREACH_UNOP(UNOP) \
    UNOP(UNOP_MINUS)          \
    UNOP(UNOP_NOT)
typedef enum
{
    NU_FOREACH_UNOP(NU_GENERATE_ENUM)
} nu__unop_type_t;
static const nu_char_t *NU_UNOP_NAMES[] = { NU_FOREACH_UNOP(NU_GENERATE_NAME) };

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

typedef union
{
    nu__lit_value_t  literal;
    nu__binop_type_t binop;
    nu__unop_type_t  unop;
    nu_type_t        primitive;
} nu__ast_value_t;

typedef nu_u32_t nu__ast_node_id_t;
#define NU_AST_NODE_NULL 0xffffffff

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
    SYMBOL(SYMBOL_EXTERNAL)
typedef enum
{
    NU_FOREACH_SYMBOL(NU_GENERATE_ENUM)
} nu__symbol_type_t;
static const nu_char_t *NU_SYMBOL_NAMES[]
    = { NU_FOREACH_SYMBOL(NU_GENERATE_NAME) };

typedef nu_u32_t nu__symbol_id_t;
#define NU_SYMBOL_NULL 0xffffffff

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
    nu__symbol_function_t function;
    nu__symbol_argument_t argument;
    nu__symbol_constant_t constant;
    nu__symbol_local_t    local;
    nu__symbol_external_t external;
} nu__symbol_value_t;

typedef struct
{
    nu__symbol_type_t  type;
    nu__symbol_value_t value;
} nu__symbol_t;

typedef struct
{

} nu__symbol_table_t;

typedef struct
{
    nu__source_span_t span;
    nu__token_type_t  expect;
    nu__token_type_t  got;
} nu__parser_unexpected_token_t;

typedef struct
{
    nu__parser_unexpected_token_t unexpected_token;
} nu__parser_error_t;

typedef struct
{
    nu__parser_error_t error;
    nu__lexer_t       *lexer;
    nu__ast_t         *ast;
} nu__parser_t;

static nu__compiler_error_t
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
    for (i = 0; i < sizeof(statements) / sizeof(statements[0]); ++i)
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
    for (i = 0; i < sizeof(expressions) / sizeof(expressions[0]); ++i)
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
    for (i = 0; i < sizeof(loops) / sizeof(loops[0]); ++i)
    {
        if (t == loops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}

static nu__compiler_error_t
nu__parser_peek (nu__parser_t *parser, nu_size_t lookahead, nu__token_t *token)
{
    return nu__lexer_peek(parser->lexer, lookahead, token);
}
static nu__compiler_error_t
nu__parser_consume (nu__parser_t *parser, nu__token_t *token)
{
    return nu__lexer_next(parser->lexer, token);
}
static nu__compiler_error_t
nu__parser_expect (nu__parser_t    *parser,
                   nu__token_type_t type,
                   nu__token_t     *token)
{
    nu__token_t          tok;
    nu__compiler_error_t error;
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
static nu__compiler_error_t
nu__parser_accept (nu__parser_t    *parser,
                   nu__token_type_t type,
                   nu__token_t     *token,
                   nu_bool_t       *found)
{
    nu__token_t          tok;
    nu__compiler_error_t error;
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
static nu__compiler_error_t
nu__try_parse_primitive (nu__parser_t *parser,
                         nu_type_t    *primitive,
                         nu_bool_t    *found)
{
    nu__compiler_error_t error;
    nu__token_t          tok;
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
typedef nu__compiler_error_t (*nu__parse_identifier_list_pfn_t)(
    const nu__token_t *, void *);
static nu__compiler_error_t
nu__parse_identifier_list (nu__parser_t                   *parser,
                           nu__token_type_t                separator,
                           nu_size_t                      *count,
                           nu__parse_identifier_list_pfn_t callback,
                           void                           *userdata)
{
    nu__compiler_error_t error;
    nu__token_t          tok;
    nu_bool_t            found;
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
typedef nu__compiler_error_t (*nu__parse_function_argument_list_pfn_t)(
    const nu__token_t *, nu_type_t, void *);
static nu__compiler_error_t
nu__parse_function_argument_list (
    nu__parser_t                          *parser,
    nu__token_type_t                       separator,
    nu_size_t                             *count,
    nu__parse_function_argument_list_pfn_t callback,
    void                                  *userdata)
{
    nu__compiler_error_t error;
    nu__token_t          tok;
    nu_bool_t            found;
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
static nu__compiler_error_t
nu__parse_field_lookup_chain (nu__parser_t      *parser,
                              nu__ast_node_id_t  child,
                              nu__ast_node_id_t *parent)
{
    nu__compiler_error_t error;
    nu__ast_node_id_t    node;
    nu__token_t          tok;
    error = nu__parser_expect(parser, TOKEN_DOT, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NU_COMPERR_CHECK(error);
    error = nu__ast_add_node(parser->ast, &node);
    NU_COMPERR_CHECK(error);
    parser->ast->nodes[node].type          = AST_FIELDLOOKUP;
    parser->ast->nodes[node].span          = tok.span;
    parser->ast->nodes[node].value.literal = tok.value.literal.value;
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
static nu__compiler_error_t
nu__try_parse_field_lookup_chain (nu__parser_t      *parser,
                                  nu__ast_node_id_t  node,
                                  nu__ast_node_id_t *parent)
{
    nu__compiler_error_t error;
    nu__token_t          tok;
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
static nu__compiler_error_t
nu__parse_identifier (nu__parser_t      *parser,
                      nu__ast_node_id_t  block,
                      nu__ast_node_id_t *ident)
{
    nu__compiler_error_t error;
    nu__ast_node_id_t    node;
    nu__token_t          tok;
    error = nu__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NU_COMPERR_CHECK(error);
    return NU_COMPERR_NONE;
}
static nu__compiler_error_t
nu__parse_call (nu__parser_t      *parser,
                nu__ast_node_id_t  ident,
                nu__ast_node_id_t  block,
                nu__ast_node_id_t *call)
{
    nu__compiler_error_t error;
    nu__ast_node_id_t    node;
    nu__token_t          tok;
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
        (void)ident;
        (void)block;
        (void)call;
        /* TODO */
    }
    return NU_COMPERR_NONE;
}

static nu__compiler_error_t
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

#endif

#endif
