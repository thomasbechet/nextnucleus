#ifndef NULANG_PARSER_H
#define NULANG_PARSER_H

#include <nucleus/lang/ast.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/symbol.h>
#include <nucleus/lang/token.h>
#include <nucleus/lang/report.h>
#include <nucleus/vm.h>
#include <nucleus/vm/string.h>

#ifdef NU_IMPL

typedef struct
{
    const struct nu__vm    *vm;
    nulang__error_data_t   *error;
    nulang__lexer_t        *lexer;
    nulang__ast_t          *ast;
    nulang__symbol_table_t *symtab;
} nulang__parser_t;

static nulang__error_t nulang__parse_expression(nulang__parser_t *parser,
                                                nu_u16_t min_precedence,
                                                nulang__block_id_t block,
                                                nulang__node_id_t *node);
static nulang__error_t nulang__parse_statement(nulang__parser_t  *parser,
                                               nulang__block_id_t block,
                                               nulang__node_id_t *node);
static nulang__error_t
nulang__parser_peek (nulang__parser_t *parser,
                     nu_size_t         lookahead,
                     nulang__token_t  *token)
{
    return nulang__lexer_peek(parser->lexer, lookahead, token);
}
static nulang__error_t
nulang__parser_consume (nulang__parser_t *parser, nulang__token_t *token)
{
    return nulang__lexer_next(parser->lexer, token);
}
static nulang__error_t
nulang__parser_expect (nulang__parser_t    *parser,
                       nulang__token_type_t type,
                       nulang__token_t     *token)
{
    nulang__error_t error;
    error = nulang__parser_consume(parser, token);
    NULANG_ERROR_CHECK(error);
    if (token->type != type)
    {
        parser->error->token_got    = token->type;
        parser->error->token_expect = type;
        parser->error->span         = token->span;
        return NULANG_ERROR_UNEXPECTED_TOKEN;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parser_accept (nulang__parser_t    *parser,
                       nulang__token_type_t type,
                       nulang__token_t     *token,
                       nu_bool_t           *found)
{
    nulang__token_t tok;
    nulang__error_t error;
    error = nulang__lexer_peek(parser->lexer, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == type)
    {
        *found = NU_TRUE;
        return nulang__lexer_next(parser->lexer, token);
    }
    *found = NU_FALSE;
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_vartype (nulang__parser_t *parser, nulang__vartype_t *vartype)
{
    nulang__error_t error;
    nulang__token_t tok;
    nu_bool_t       found;
    error = nulang__parser_expect(parser, TOKEN_COLON, &tok);
    NULANG_ERROR_CHECK(error);
    /* primitive */
    error = nulang__parser_accept(parser, TOKEN_PRIMITIVE, &tok, &found);
    NULANG_ERROR_CHECK(error);
    if (found)
    {
        vartype->primitive = tok.value.primitive;
        return NULANG_ERROR_NONE;
    }
    /* archetype */
    error = nulang__parser_accept(parser, TOKEN_IDENTIFIER, &tok, &found);
    NULANG_ERROR_CHECK(error);
    if (found)
    {
        vartype->primitive = NU_PRIMITIVE_ENTITY;
        vartype->archetype = NU_ARCHETYPE_NULL;
        return NULANG_ERROR_NONE;
    }
    error = nulang__parser_consume(parser, &tok);
    NULANG_ERROR_CHECK(error);
    parser->error->span = tok.span;
    return NULANG_ERROR_INVALID_VARTYPE;
}
static nulang__error_t
nulang__try_parse_vartype (nulang__parser_t  *parser,
                           nulang__vartype_t *vartype,
                           nu_bool_t         *found)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_COLON)
    {
        *found = NU_TRUE;
        error  = nulang__parse_vartype(parser, vartype);
        NULANG_ERROR_CHECK(error);
    }
    else
    {
        *found             = NU_FALSE;
        vartype->primitive = NU_PRIMITIVE_UNKNOWN;
    }
    return NULANG_ERROR_NONE;
}
typedef nulang__error_t (*nu__parse_identifier_list_pfn_t)(
    const nulang__token_t *, void *);
static nulang__error_t
nulang__parse_identifier_list (nulang__parser_t               *parser,
                               nulang__token_type_t            separator,
                               nu_size_t                      *count,
                               nu__parse_identifier_list_pfn_t callback,
                               void                           *userdata)
{
    nulang__error_t error;
    nulang__token_t tok;
    nu_bool_t       found;
    *count = 0;
    error  = nulang__parser_accept(parser, TOKEN_IDENTIFIER, &tok, &found);
    NULANG_ERROR_CHECK(error);
    if (found)
    {
        error = callback(&tok, userdata);
        NULANG_ERROR_CHECK(error);
        (*count)++;
        for (;;)
        {
            error = nulang__parser_accept(parser, separator, &tok, &found);
            NULANG_ERROR_CHECK(error);
            if (!found)
            {
                break;
            }
            error = nulang__parser_expect(parser, TOKEN_LITERAL, &tok);
            NULANG_ERROR_CHECK(error);
            error = callback(&tok, userdata);
            NULANG_ERROR_CHECK(error);
            (*count)++;
        }
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_member_chain (nulang__parser_t  *parser,
                            nulang__node_id_t  child,
                            nulang__node_id_t *parent)
{
    nulang__error_t   error;
    nulang__token_t   tok;
    nulang__node_id_t node;
    error = nulang__parser_expect(parser, TOKEN_DOT, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NULANG_ERROR_CHECK(error);

    {
        nulang__node_t *pchild = &parser->ast->nodes[child];
        switch (pchild->type)
        {
            case AST_MEMBER:
            case AST_SYMREF: {
                error = nulang__ast_add_node(parser->ast, &node);
                NULANG_ERROR_CHECK(error);
                parser->ast->nodes[node].span         = tok.span;
                parser->ast->nodes[node].type         = AST_MEMBER;
                parser->ast->nodes[node].value.member = tok.value.identifier;
                nulang__ast_append_child(parser->ast, node, child);
            }
            break;
            case AST_BUILTIN: {
                switch (pchild->value.builtin.type)
                {
                    case BUILTIN_CONSTRUCTOR: {
                        /* promote to primitive builtin function */
                        const nulang__builtin_function_t *function
                            = nulang__find_builtin_function(
                                pchild->value.builtin.value.constructor,
                                tok.value.identifier);
                        if (!function)
                        {
                            parser->error->span = tok.span;
                            return NULANG_ERROR_UNKNOWN_SYMBOL_TYPE;
                        }
                        parser->ast->nodes[child].value.builtin.type
                            = BUILTIN_FUNCTION;
                        parser->ast->nodes[child].value.builtin.value.function
                            = function;
                        node = child;
                    }
                    break;
                    default:
                        parser->error->span = tok.span;
                        return NULANG_ERROR_UNKNOWN_SYMBOL_TYPE;
                }
            }
            default:
                break;
        }
    }

    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_DOT)
    {
        return nulang__parse_member_chain(parser, node, parent);
    }
    else
    {
        *parent = node;
        return NULANG_ERROR_NONE;
    }
}
static nulang__error_t
nulang__try_parse_member_chain (nulang__parser_t  *parser,
                                nulang__node_id_t  node,
                                nulang__node_id_t *parent)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_DOT)
    {
        return nulang__parse_member_chain(parser, node, parent);
    }
    else
    {
        *parent = node;
        return NULANG_ERROR_NONE;
    }
}
static nulang__error_t
nulang__parse_symbol (nulang__parser_t  *parser,
                      nulang__node_id_t  block,
                      nulang__node_id_t *node)
{
    nulang__error_t     error;
    nulang__symbol_id_t symbol;
    nulang__token_t     tok;

    error = nulang__parser_consume(parser, &tok);
    NULANG_ERROR_CHECK(error);

    if (tok.type == TOKEN_IDENTIFIER)
    {
        error = nulang__lookup_symbol(
            parser->symtab, tok.value.identifier, tok.span, block, &symbol);
        NULANG_ERROR_CHECK(error);
    }
    else
    {
        parser->error->token_got = tok.type;
        return NULANG_ERROR_UNEXPECTED_TOKEN;
    }

    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type                = AST_SYMREF;
    parser->ast->nodes[*node].value.symref.symbol = symbol;
    parser->ast->nodes[*node].span                = tok.span;

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_call (nulang__parser_t  *parser,
                    nulang__node_id_t  ident,
                    nulang__node_id_t  block,
                    nulang__node_id_t *call)
{
    nulang__error_t   error;
    nulang__node_id_t node, expr;
    nulang__token_t   tok;
    nu_bool_t         found;
    error = nulang__ast_add_node(parser->ast, &node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[node].type = AST_CALL;
    nulang__ast_append_child(parser->ast, node, ident);
    error = nulang__parser_expect(parser, TOKEN_LPAREN, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__lexer_peek(parser->lexer, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type != TOKEN_RPAREN)
    {
        error = nulang__parse_expression(parser, 0, block, &expr);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, node, expr);
    }
    for (;;)
    {
        error = nulang__parser_accept(parser, TOKEN_COMMA, &tok, &found);
        NULANG_ERROR_CHECK(error);
        if (!found)
        {
            break;
        }
        error = nulang__parse_expression(parser, 0, block, &expr);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, node, expr);
    }
    error = nulang__parser_expect(parser, TOKEN_RPAREN, &tok);
    NULANG_ERROR_CHECK(error);
    *call = node;
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__try_parse_call (nulang__parser_t  *parser,
                        nulang__node_id_t  node,
                        nulang__block_id_t block,
                        nulang__node_id_t *id)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_LPAREN)
    {
        error = nulang__parse_call(parser, node, block, id);
        NULANG_ERROR_CHECK(error);
    }
    else
    {
        *id = node;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_insert_or_singleton (nulang__parser_t   *parser,
                                   nulang__node_type_t type,
                                   nulang__node_id_t  *id)
{
    nulang__error_t error;
    nulang__token_t tok;
    nu_archetype_t  archetype;
    error = nulang__parser_consume(parser, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_LPAREN, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NULANG_ERROR_CHECK(error);
    archetype = nu__archetype_find(
        &parser->vm->tables,
        nu_uidn(tok.value.identifier.p, tok.value.identifier.n));
    if (!archetype)
    {
        parser->error->span = tok.span;
        return NULANG_ERROR_ARCHETYPE_NOT_FOUND;
    }
    error = nulang__ast_add_node(parser->ast, id);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*id].type = type;
    parser->ast->nodes[*id].span = tok.span;
    if (type == AST_INSERT)
    {
        parser->ast->nodes[*id].value.insert.archetype = archetype;
    }
    else
    {
        parser->ast->nodes[*id].value.singleton.archetype = archetype;
    }
    error = nulang__parser_expect(parser, TOKEN_RPAREN, &tok);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_atom (nulang__parser_t  *parser,
                    nulang__block_id_t block,
                    nulang__node_id_t *node)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    switch (tok.type)
    {
        case TOKEN_IDENTIFIER:
            /* TODO: lookup for builtin functions */
            error = nulang__parse_symbol(parser, block, node);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_member_chain(parser, *node, node);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_call(parser, *node, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_INSERT:
            error = nulang__parse_insert_or_singleton(parser, AST_INSERT, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_SINGLETON:
            error = nulang__parse_insert_or_singleton(
                parser, AST_SINGLETON, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_PRIMITIVE:
            error = nulang__parser_expect(parser, TOKEN_PRIMITIVE, &tok);
            NULANG_ERROR_CHECK(error);
            error = nulang__ast_add_node(parser->ast, node);
            NULANG_ERROR_CHECK(error);
            /* by default, we expect a builtin constructor */
            parser->ast->nodes[*node].type               = AST_BUILTIN;
            parser->ast->nodes[*node].span               = tok.span;
            parser->ast->nodes[*node].value.builtin.type = BUILTIN_CONSTRUCTOR;
            parser->ast->nodes[*node].value.builtin.value.constructor
                = tok.value.primitive;
            error = nulang__try_parse_member_chain(parser, *node, node);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_call(parser, *node, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_LITERAL:
            error = nulang__parser_consume(parser, &tok);
            NULANG_ERROR_CHECK(error);
            error = nulang__ast_add_node(parser->ast, node);
            NULANG_ERROR_CHECK(error);
            parser->ast->nodes[*node].type          = AST_LITERAL;
            parser->ast->nodes[*node].span          = tok.span;
            parser->ast->nodes[*node].value.literal = tok.value.literal;
            break;
        default:
            parser->error->span      = tok.span;
            parser->error->token_got = tok.type;
            return NULANG_ERROR_INVALID_ATOM_EXPRESSION;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_primary (nulang__parser_t  *parser,
                       nulang__block_id_t block,
                       nulang__node_id_t *node)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_LPAREN)
    {
        error = nulang__parser_consume(parser, &tok);
        NULANG_ERROR_CHECK(error);
        error = nulang__parse_expression(parser, 1, block, node);
        NULANG_ERROR_CHECK(error);
        error = nulang__parser_expect(parser, TOKEN_RPAREN, &tok);
        NULANG_ERROR_CHECK(error);
    }
    else if (nulang__is_unop(tok.type))
    {
        nulang__node_id_t expr;
        error = nulang__parser_consume(parser, &tok);
        NULANG_ERROR_CHECK(error);
        error = nulang__parse_primary(parser, block, &expr);
        NULANG_ERROR_CHECK(error);
        error = nulang__ast_add_node(parser->ast, node);
        NULANG_ERROR_CHECK(error);
        parser->ast->nodes[*node].type = AST_UNOP;
        parser->ast->nodes[*node].value.unop
            = nulang__unop_from_token(tok.type);
        parser->ast->nodes[*node].span = tok.span;
        nulang__ast_append_child(parser->ast, *node, expr);
    }
    else if (nulang__is_binop(tok.type))
    {
        parser->error->span = tok.span;
        return NULANG_ERROR_UNEXPECTED_BINOP;
    }
    else
    {
        error = nulang__parse_atom(parser, block, node);
        NULANG_ERROR_CHECK(error);
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_expression (nulang__parser_t  *parser,
                          nu_u16_t           min_precedence,
                          nulang__block_id_t block,
                          nulang__node_id_t *node)
{
    nulang__node_id_t lhs;
    nulang__token_t   tok;
    nulang__error_t   error;

    error = nulang__parse_primary(parser, block, &lhs);
    NULANG_ERROR_CHECK(error);
    for (;;)
    {
        error = nulang__parser_peek(parser, 0, &tok);
        NULANG_ERROR_CHECK(error);
        if (nulang__is_binop(tok.type))
        {
            nu_u16_t          next_min_assoc;
            nulang__node_id_t op_node, rhs;
            nu_u16_t          prec = nulang__binop_precedence(tok.type);
            nu_bool_t         is_left_associative
                = nulang__binop_is_left_associative(tok.type);
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

            error = nulang__parser_consume(parser, &tok);
            NULANG_ERROR_CHECK(error);
            error
                = nulang__parse_expression(parser, next_min_assoc, block, &rhs);
            NULANG_ERROR_CHECK(error);

            error = nulang__ast_add_node(parser->ast, &op_node);
            NULANG_ERROR_CHECK(error);
            parser->ast->nodes[op_node].type = AST_BINOP;
            parser->ast->nodes[op_node].value.binop
                = nulang__binop_from_token(tok.type);
            parser->ast->nodes[op_node].span = tok.span;
            nulang__ast_append_child(parser->ast, op_node, lhs);
            nulang__ast_append_child(parser->ast, op_node, rhs);
            lhs = op_node;
        }
        else
        {
            break;
        }
    }
    *node = lhs;
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_variable_declaration (nulang__parser_t  *parser,
                                    nulang__block_id_t block,
                                    nulang__node_id_t *node)
{
    nulang__error_t        error;
    nulang__token_t        tok, ident;
    nulang__vartype_t      vartype;
    nu_bool_t              found;
    nulang__node_id_t      expr;
    nulang__symbol_id_t    symbol;
    nulang__symbol_value_t symbol_value;
    error = nulang__parser_expect(parser, TOKEN_LET, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_IDENTIFIER, &ident);
    NULANG_ERROR_CHECK(error);
    error = nulang__try_parse_vartype(parser, &vartype, &found);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_ASSIGN, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parse_expression(parser, 0, block, &expr);
    NULANG_ERROR_CHECK(error);
    symbol_value.variable.vartype = vartype;
    error                         = nulang__define_symbol(parser->symtab,
                                  SYMBOL_VARIABLE,
                                  symbol_value,
                                  ident.value.identifier,
                                  ident.span,
                                  block,
                                  &symbol);
    if (error != NULANG_ERROR_NONE)
    {
        parser->error->span = ident.span;
    }
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type                 = AST_VARDECL;
    parser->ast->nodes[*node].value.vardecl.symbol = symbol;
    parser->ast->nodes[*node].span                 = ident.span;
    nulang__ast_append_child(parser->ast, *node, expr);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_and_append_if_body (nulang__parser_t  *parser,
                                  nulang__node_id_t  if_node,
                                  nulang__block_id_t block)
{
    nulang__block_id_t if_block;
    nulang__error_t    error;
    nulang__token_t    tok;
    nulang__node_id_t  body;
    error = nulang__add_block(parser->symtab, BLOCK_IF, block, &if_block);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, &body);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[body].type = AST_COMPOUND;
    for (;;)
    {
        nulang__node_id_t stmt;
        error = nulang__parser_peek(parser, 0, &tok);
        NULANG_ERROR_CHECK(error);
        if (tok.type == TOKEN_END || tok.type == TOKEN_ELSE
            || tok.type == TOKEN_ELIF)
        {
            break;
        }
        error = nulang__parse_statement(parser, if_block, &stmt);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, body, stmt);
    }
    nulang__ast_append_child(parser->ast, if_node, body);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_if_statement (nulang__parser_t  *parser,
                            nulang__block_id_t block,
                            nulang__node_id_t *node)
{
    nulang__error_t   error;
    nulang__token_t   tok;
    nulang__node_id_t condition;
    nu_bool_t         found;
    error = nulang__parser_expect(parser, TOKEN_IF, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type = AST_IF;
    parser->ast->nodes[*node].span = tok.span;
    /* if condition */
    error = nulang__parse_expression(parser, 0, block, &condition);
    NULANG_ERROR_CHECK(error);
    nulang__ast_append_child(parser->ast, *node, condition);
    error = nulang__parser_expect(parser, TOKEN_THEN, &tok);
    NULANG_ERROR_CHECK(error);
    /* if body */
    error = nulang__parse_and_append_if_body(parser, *node, block);
    NULANG_ERROR_CHECK(error);
    for (;;)
    {
        error = nulang__parser_accept(parser, TOKEN_ELIF, &tok, &found);
        NULANG_ERROR_CHECK(error);
        if (!found)
        {
            break;
        }
        /* elif condition */
        error = nulang__parse_expression(parser, 0, block, &condition);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, *node, condition);
        error = nulang__parser_expect(parser, TOKEN_THEN, &tok);
        NULANG_ERROR_CHECK(error);
        /* elif body */
        error = nulang__parse_and_append_if_body(parser, *node, block);
        NULANG_ERROR_CHECK(error);
    }
    error = nulang__parser_accept(parser, TOKEN_ELSE, &tok, &found);
    NULANG_ERROR_CHECK(error);
    if (found)
    {
        /* else body */
        error = nulang__parse_and_append_if_body(parser, *node, block);
        NULANG_ERROR_CHECK(error);
    }
    error = nulang__parser_expect(parser, TOKEN_END, &tok);
    NULANG_ERROR_CHECK(error);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_assignment_statement (nulang__parser_t  *parser,
                                    nulang__block_id_t block,
                                    nulang__node_id_t  ident,
                                    nulang__node_id_t *node)
{
    nulang__error_t   error;
    nulang__token_t   tok;
    nulang__node_id_t expr;
    error = nulang__parser_expect(parser, TOKEN_ASSIGN, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parse_expression(parser, 0, block, &expr);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type = AST_ASSIGN;
    parser->ast->nodes[*node].span = tok.span;
    nulang__ast_append_child(parser->ast, *node, ident);
    nulang__ast_append_child(parser->ast, *node, expr);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_while_statement (nulang__parser_t  *parser,
                               nulang__block_id_t block,
                               nulang__node_id_t *node)
{
    nulang__error_t    error;
    nulang__token_t    tok;
    nulang__node_id_t  expr;
    nulang__block_id_t while_block;
    error = nulang__parser_expect(parser, TOKEN_WHILE, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__add_block(parser->symtab, BLOCK_WHILE, block, &while_block);
    NULANG_ERROR_CHECK(error);
    error = nulang__parse_expression(parser, 0, block, &expr);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type = AST_WHILE;
    parser->ast->nodes[*node].span = tok.span;
    nulang__ast_append_child(parser->ast, *node, expr);
    error = nulang__parser_expect(parser, TOKEN_DO, &tok);
    NULANG_ERROR_CHECK(error);
    for (;;)
    {
        nulang__node_id_t stmt;
        error = nulang__parser_peek(parser, 0, &tok);
        NULANG_ERROR_CHECK(error);
        if (tok.type == TOKEN_END)
        {
            break;
        }
        error = nulang__parse_statement(parser, while_block, &stmt);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, *node, stmt);
    }
    error = nulang__parser_expect(parser, TOKEN_END, &tok);
    NULANG_ERROR_CHECK(error);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_function_declaration (nulang__parser_t  *parser,
                                    nulang__block_id_t block,
                                    nu_bool_t          exported,
                                    nulang__node_id_t *node)
{
    nulang__error_t        error;
    nulang__token_t        tok;
    nulang__node_id_t      ident;
    nulang__symbol_value_t symbol_value;
    nulang__symbol_id_t    function_symbol;
    nulang__block_id_t     function_block;
    nulang__vartype_t      type;
    nu_bool_t              found;

    error = nulang__parser_expect(parser, TOKEN_FUNCTION, &tok);
    NULANG_ERROR_CHECK(error);
    if (parser->symtab->blocks[block].type != BLOCK_GLOBAL)
    {
        parser->error->span = tok.span;
        return NULANG_ERROR_FUNCTION_OUTSIDE_GLOBAL_SCOPE;
    }

    error = nulang__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NULANG_ERROR_CHECK(error);

    error = nulang__add_block(
        parser->symtab, BLOCK_FUNCTION, block, &function_block);
    NULANG_ERROR_CHECK(error);
    symbol_value.function.return_type.primitive = NU_PRIMITIVE_UNKNOWN;
    symbol_value.function.exported              = exported;
    symbol_value.function.first_arg             = NULANG_NODE_NULL;
    symbol_value.function.block                 = function_block;
    error = nulang__define_symbol(parser->symtab,
                                  SYMBOL_FUNCTION,
                                  symbol_value,
                                  tok.value.identifier,
                                  tok.span,
                                  block,
                                  &function_symbol);
    NULANG_ERROR_CHECK(error);
    parser->symtab->blocks[function_block].value.function = function_symbol;
    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type                 = AST_FUNDECL;
    parser->ast->nodes[*node].span                 = tok.span;
    parser->ast->nodes[*node].value.fundecl.symbol = function_symbol;

    /* parse arguments */
    error = nulang__parser_expect(parser, TOKEN_LPAREN, &tok);
    NULANG_ERROR_CHECK(error);

    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type != TOKEN_RPAREN)
    {
        for (;;)
        {
            nulang__symbol_id_t argument;

            error = nulang__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
            NULANG_ERROR_CHECK(error);

            /* check duplicated argument */
            if (nulang__find_symbol_in_block(parser->symtab,
                                             function_block,
                                             tok.value.identifier,
                                             &argument))
            {
                parser->error->span = tok.span;
                return NULANG_ERROR_DUPLICATED_FUNCTION_ARGUMENT;
            }

            /* parse argument type */
            error = nulang__parse_vartype(parser, &type);
            NULANG_ERROR_CHECK(error);

            /* define symbol */
            symbol_value.argument.vartype  = type;
            symbol_value.argument.function = function_symbol;
            symbol_value.argument.next
                = parser->symtab->symbols[function_symbol]
                      .value.function.first_arg;
            error = nulang__define_symbol(parser->symtab,
                                          SYMBOL_ARGUMENT,
                                          symbol_value,
                                          tok.value.identifier,
                                          tok.span,
                                          function_block,
                                          &argument);
            NULANG_ERROR_CHECK(error);
            parser->symtab->symbols[function_symbol].value.function.first_arg
                = argument;

            error = nulang__parser_accept(parser, TOKEN_COMMA, &tok, &found);
            NULANG_ERROR_CHECK(error);
            if (!found)
            {
                break;
            }
        }
    }
    error = nulang__parser_expect(parser, TOKEN_RPAREN, &tok);
    NULANG_ERROR_CHECK(error);

    /* parse return type */
    error = nulang__try_parse_vartype(parser, &type, &found);
    NULANG_ERROR_CHECK(error);
    if (found)
    {
        parser->symtab->symbols[function_symbol].value.function.return_type
            = type;
    }

    /* parse function body */
    for (;;)
    {
        nulang__node_id_t stmt;
        error = nulang__parser_peek(parser, 0, &tok);
        NULANG_ERROR_CHECK(error);
        if (tok.type == TOKEN_END)
        {
            break;
        }
        error = nulang__parse_statement(parser, function_block, &stmt);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, *node, stmt);
    }
    error = nulang__parser_expect(parser, TOKEN_END, &tok);
    NULANG_ERROR_CHECK(error);

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_return_statement (nulang__parser_t  *parser,
                                nulang__block_id_t block,
                                nulang__node_id_t *node)
{
    nulang__error_t   error;
    nulang__token_t   tok;
    nulang__node_id_t expr;
    error = nulang__parser_expect(parser, TOKEN_RETURN, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parse_expression(parser, 0, block, &expr);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type = AST_RETURN;
    parser->ast->nodes[*node].span = tok.span;
    nulang__ast_append_child(parser->ast, *node, expr);
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_statement (nulang__parser_t  *parser,
                         nulang__block_id_t block,
                         nulang__node_id_t *node)
{
    nulang__error_t   error;
    nulang__token_t   tok;
    nulang__node_id_t ident;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    switch (tok.type)
    {
        case TOKEN_LET:
            error = nulang__parse_variable_declaration(parser, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_EXPORT:
            break;
        case TOKEN_FUNCTION:
            error = nulang__parse_function_declaration(
                parser, block, NU_FALSE, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_RETURN:
            error = nulang__parse_return_statement(parser, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_IF:
            error = nulang__parse_if_statement(parser, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_FOR:
            break;
        case TOKEN_WHILE:
            error = nulang__parse_while_statement(parser, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_BREAK:
            break;
        case TOKEN_CONTINUE:
            break;
        case TOKEN_IDENTIFIER:
            error = nulang__parse_symbol(parser, block, &ident);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_member_chain(parser, ident, &ident);
            NULANG_ERROR_CHECK(error);
            error = nulang__parser_peek(parser, 0, &tok);
            NULANG_ERROR_CHECK(error);
            if (tok.type == TOKEN_ASSIGN)
            {
                error = nulang__parse_assignment_statement(
                    parser, block, ident, node);
                NULANG_ERROR_CHECK(error);
            }
            else if (tok.type == TOKEN_LPAREN)
            {
                error = nulang__parse_call(parser, ident, block, node);
                NULANG_ERROR_CHECK(error);
            }
            else
            {
                parser->error->span = tok.span;
                return NULANG_ERROR_IDENTIFIER_AS_STATEMENT;
            }
            break;
        default:
            parser->error->span      = tok.span;
            parser->error->token_got = tok.type;
            return NULANG_ERROR_NON_STATEMENT_TOKEN;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse (nulang__parser_t *parser)
{
    nulang__error_t   error;
    nulang__node_id_t stmt;
    nulang__token_t   tok;
    for (;;)
    {
        error = nulang__parser_peek(parser, 0, &tok);
        NULANG_ERROR_CHECK(error);
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
        error = nulang__parser_peek(parser, 0, &tok);
        NULANG_ERROR_CHECK(error);
        if (tok.type == TOKEN_EOF)
        {
            break;
        }
        error = nulang__parse_statement(
            parser, parser->symtab->block_global, &stmt);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, parser->ast->root, stmt);
    }
    error = nulang__parser_expect(parser, TOKEN_EOF, &tok);
    NULANG_ERROR_CHECK(error);
    return NULANG_ERROR_NONE;
}

static void
nulang__parser_init (const struct nu__vm    *vm,
                     nulang__lexer_t        *lexer,
                     nulang__ast_t          *ast,
                     nulang__symbol_table_t *symtab,
                     nulang__error_data_t   *error,
                     nulang__parser_t       *parser)
{
    parser->vm     = vm;
    parser->error  = error;
    parser->lexer  = lexer;
    parser->ast    = ast;
    parser->symtab = symtab;
}

#endif

#endif
