#ifndef NULANG_PARSER_H
#define NULANG_PARSER_H

#include <nucleus/lang/ast.h>
#include <nucleus/lang/token.h>
#include <nucleus/lang/report.h>
#include <nucleus/vm/error.h>
#include <nucleus/vm/types.h>

#ifdef NULANG_IMPL

typedef struct
{
    nulang__error_data_t   *error;
    nulang__lexer_t        *lexer;
    nulang__ast_t          *ast;
    nulang__symbol_table_t *symbols;
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
nulang__try_parse_vartype (nulang__parser_t       *parser,
                           nulang__symbol_table_t *symbols,
                           nulang__block_id_t      block,
                           nulang__vartype_t      *vartype,
                           nu_bool_t              *found)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_accept(parser, TOKEN_COLON, &tok, found);
    NULANG_ERROR_CHECK(error);
    if (*found)
    {
        error = nulang__parser_accept(parser, TOKEN_ARCHETYPE, &tok, found);
        NULANG_ERROR_CHECK(error);
        if (*found)
        {
            nulang__symbol_id_t arch;
            error = nulang__lookup_archetype(
                symbols, tok.value.identifier, tok.span, &arch);
            NULANG_ERROR_CHECK(error);
            vartype->primitive       = NU_PRIMITIVE_ENTITY;
            vartype->value.archetype = arch;
            return NULANG_ERROR_NONE;
        }
        error = nulang__parser_accept(parser, TOKEN_PRIMITIVE, &tok, found);
        NULANG_ERROR_CHECK(error);
        if (*found)
        {
            vartype->primitive = tok.value.primitive;
            return NULANG_ERROR_NONE;
        }
        error = nulang__parser_consume(parser, &tok);
        NULANG_ERROR_CHECK(error);
        parser->error->span = tok.span;
        return NULANG_ERROR_INVALID_VARTYPE;
    }
    vartype->primitive = NU_PRIMITIVE_UNKNOWN;
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
typedef nulang__error_t (*nu__parse_function_argument_list_pfn_t)(
    const nulang__token_t *, nulang__vartype_t, void *);
static nulang__error_t
nulang__parse_function_argument_list (
    nulang__parser_t                      *parser,
    nulang__token_type_t                   separator,
    nulang__block_id_t                     function_block,
    nu_size_t                             *count,
    nu__parse_function_argument_list_pfn_t callback,
    void                                  *userdata)
{
    nulang__error_t error;
    nulang__token_t tok;
    nu_bool_t       found;
    *count = 0;
    error  = nulang__parser_accept(parser, TOKEN_IDENTIFIER, &tok, &found);
    NULANG_ERROR_CHECK(error);
    if (found)
    {
        nulang__vartype_t vartype;
        error = nulang__try_parse_vartype(
            parser, parser->symbols, function_block, &vartype, &found);
        NULANG_ERROR_CHECK(error);
        error = callback(&tok, vartype, userdata);
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
            error = nulang__try_parse_vartype(
                parser, parser->symbols, function_block, &vartype, &found);
            NULANG_ERROR_CHECK(error);
            error = callback(&tok, vartype, userdata);
            NULANG_ERROR_CHECK(error);
            (*count)++;
        }
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__parse_field_lookup_chain (nulang__parser_t  *parser,
                                  nulang__node_id_t  child,
                                  nulang__node_id_t *parent)
{
    nulang__error_t   error;
    nulang__node_id_t node;
    nulang__token_t   tok;
    error = nulang__parser_expect(parser, TOKEN_DOT, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_IDENTIFIER, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, &node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[node].type              = AST_FIELDLOOKUP;
    parser->ast->nodes[node].span              = tok.span;
    parser->ast->nodes[node].value.fieldlookup = tok.value.identifier;
    nulang__ast_append_child(parser->ast, node, child);
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_DOT)
    {
        return nulang__parse_field_lookup_chain(parser, node, parent);
    }
    else
    {
        *parent = node;
        return NULANG_ERROR_NONE;
    }
}
static nulang__error_t
nulang__try_parse_field_lookup_chain (nulang__parser_t  *parser,
                                      nulang__node_id_t  node,
                                      nulang__node_id_t *parent)
{
    nulang__error_t error;
    nulang__token_t tok;
    error = nulang__parser_peek(parser, 0, &tok);
    NULANG_ERROR_CHECK(error);
    if (tok.type == TOKEN_DOT)
    {
        return nulang__parse_field_lookup_chain(parser, node, parent);
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

    if (tok.type == TOKEN_ARCHETYPE)
    {
        error = nulang__lookup_archetype(
            parser->symbols, tok.value.identifier, tok.span, &symbol);
        NULANG_ERROR_CHECK(error);
    }
    else if (tok.type == TOKEN_IDENTIFIER)
    {
        error = nulang__lookup_symbol(
            parser->symbols, tok.value.identifier, tok.span, block, &symbol);
        NULANG_ERROR_CHECK(error);
    }
    else
    {
        parser->error->token_got = tok.type;
        return NULANG_ERROR_UNEXPECTED_TOKEN;
    }

    error = nulang__ast_add_node(parser->ast, node);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[*node].type         = AST_SYMBOL;
    parser->ast->nodes[*node].value.symbol = symbol;
    parser->ast->nodes[*node].span         = tok.span;

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
        case TOKEN_ARCHETYPE:
            error = nulang__parse_symbol(parser, block, node);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_field_lookup_chain(parser, *node, node);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_call(parser, *node, block, node);
            NULANG_ERROR_CHECK(error);
            break;
        case TOKEN_PRIMITIVE:
            error = nulang__parser_expect(parser, TOKEN_PRIMITIVE, &tok);
            NULANG_ERROR_CHECK(error);
            error = nulang__ast_add_node(parser->ast, node);
            NULANG_ERROR_CHECK(error);
            parser->ast->nodes[*node].type            = AST_PRIMITIVE;
            parser->ast->nodes[*node].span            = tok.span;
            parser->ast->nodes[*node].value.primitive = tok.value.primitive;
            error = nulang__try_parse_field_lookup_chain(parser, *node, node);
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
    else if (nulang__ast_is_unop(tok.type))
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
    else if (nulang__ast_is_binop(tok.type))
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
        if (nulang__ast_is_binop(tok.type))
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
    error = nulang__try_parse_vartype(
        parser, parser->symbols, block, &vartype, &found);
    NULANG_ERROR_CHECK(error);
    error = nulang__parser_expect(parser, TOKEN_ASSIGN, &tok);
    NULANG_ERROR_CHECK(error);
    error = nulang__parse_expression(parser, 0, block, &expr);
    NULANG_ERROR_CHECK(error);
    symbol_value.variable.vartype = vartype;
    error                         = nulang__define_symbol(parser->symbols,
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
    parser->ast->nodes[*node].type         = AST_VARDECL;
    parser->ast->nodes[*node].value.symbol = symbol;
    parser->ast->nodes[*node].span         = ident.span;
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
    error = nulang__add_block(parser->symbols, BLOCK_IF, block, &if_block);
    NULANG_ERROR_CHECK(error);
    error = nulang__ast_add_node(parser->ast, &body);
    NULANG_ERROR_CHECK(error);
    parser->ast->nodes[body].type = AST_IFBODY;
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
    error
        = nulang__add_block(parser->symbols, BLOCK_WHILE, block, &while_block);
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
        case TOKEN_CONST:
            break;
        case TOKEN_FUNCTION:
            break;
        case TOKEN_RETURN:
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
        case TOKEN_ARCHETYPE:
        case TOKEN_IDENTIFIER:
            error = nulang__parse_symbol(parser, block, &ident);
            NULANG_ERROR_CHECK(error);
            error = nulang__try_parse_field_lookup_chain(parser, ident, &ident);
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
    nulang__block_id_t global_block;
    nulang__error_t    error;
    nulang__node_id_t  stmt;
    nulang__token_t    tok;
    error = nulang__add_block(
        parser->symbols, BLOCK_GLOBAL, NULANG_BLOCK_NULL, &global_block);
    NULANG_ERROR_CHECK(error);
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
        error = nulang__parse_statement(parser, global_block, &stmt);
        NULANG_ERROR_CHECK(error);
        nulang__ast_append_child(parser->ast, parser->ast->root, stmt);
    }
    error = nulang__parser_expect(parser, TOKEN_EOF, &tok);
    NULANG_ERROR_CHECK(error);
    return NULANG_ERROR_NONE;
}

static void
nulang__parser_init (nulang__lexer_t        *lexer,
                     nulang__ast_t          *ast,
                     nulang__symbol_table_t *symbols,
                     nulang__error_data_t   *error,
                     nulang__parser_t       *parser)
{
    parser->error   = error;
    parser->lexer   = lexer;
    parser->ast     = ast;
    parser->symbols = symbols;
}

#endif

#endif
