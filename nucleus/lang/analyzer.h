#ifndef NULANG_ANALYZER_H
#define NULANG_ANALYZER_H

#include <nucleus/lang/ast.h>
#include <nucleus/lang/report.h>

#ifdef NULANG_IMPL

typedef struct
{
    nulang__ast_t          *ast;
    nulang__symbol_table_t *symbols;
    nulang__error_data_t   *error_data;
} nulang__analyzer_t;

static void
nulang__analyzer_init (nulang__analyzer_t     *analyzer,
                       nulang__ast_t          *ast,
                       nulang__symbol_table_t *symbols,
                       nulang__error_data_t   *error_data)
{
    analyzer->ast        = ast;
    analyzer->symbols    = symbols;
    analyzer->error_data = error_data;
}
static nulang__error_t
nulang__analyze_if (nulang__analyzer_t *analyzer, nulang__node_id_t expr)
{
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze_expr (nulang__analyzer_t *analyzer,
                      nulang__node_id_t   expr,
                      nulang__vartype_t  *ret)
{
    nulang__node_t *node;

    NU_ASSERT(expr != NULANG_NODE_NULL);
    node = &analyzer->ast->nodes[expr];
    NU_ASSERT(nulang__ast_is_expression(node->type));

    switch (node->type)
    {
        case AST_LITERAL:
            ret->type = VARTYPE_PRIMITIVE;
            switch (node->value.literal.type)
            {
                case LITERAL_NIL:
                    ret->value.primitive = NU_PRIMITIVE_ENTITY;
                    ret->type            = VARTYPE_ARCHETYPE;
                    break;
                case LITERAL_BOOL:
                    ret->value.primitive = NU_PRIMITIVE_BOOL;
                    break;
                case LITERAL_STRING:
                    /*TODO */
                    NU_UNREACHABLE;
                    break;
                case LITERAL_INT:
                    ret->value.primitive = NU_PRIMITIVE_INT;
                    break;
                case LITERAL_FIX:
                    ret->value.primitive = NU_PRIMITIVE_FIX;
                    break;
            }
            break;
        case AST_SYMBOL:
            break;
        case AST_PRIMITIVE:
            break;
        case AST_FIELDLOOKUP:
            break;
        case AST_IFBODY:
            break;
        case AST_CALL:
            break;
        case AST_BINOP:
            break;
        case AST_UNOP:
            break;
        default:
            NU_UNREACHABLE;
            break;
    }

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze_assign (nulang__analyzer_t *analyzer, nulang__node_id_t assign)
{
    nulang__node_id_t var, expr;
    nulang__node_t   *pvar;
    nulang__symbol_t *symbol;
    nulang__vartype_t vartype;
    nulang__error_t   error;

    pvar = nulang__first_child(analyzer->ast, assign, &var);
    nulang__sibling(analyzer->ast, var, &expr);

    NU_ASSERT(pvar->type == AST_SYMBOL);
    symbol = &analyzer->symbols->symbols[pvar->value.symbol];

    error = nulang__analyze_expr(analyzer, expr, &vartype);
    NULANG_ERROR_CHECK(error);

    if (symbol->value.variable.vartype.type != VARTYPE_UNKNOWN)
    {
        if (!nulang__vartype_equals(symbol->value.variable.vartype, vartype))
        {
            analyzer->error_data->vartype_got = vartype;
            analyzer->error_data->vartype_expect
                = symbol->value.variable.vartype;
            return NULANG_ERROR_INCOMPATIBLE_TYPE;
        }
    }
    else
    {
        symbol->value.variable.vartype = vartype;
    }

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze (nulang__analyzer_t *analyzer)
{
    nulang__error_t error;

    nulang__node_t   *node;
    nulang__node_id_t id;
    node = nulang__first_child(analyzer->ast, analyzer->ast->root, &id);
    while (node)
    {
        switch (node->type)
        {
            case AST_RETURN:
                break;
            case AST_IF:
                break;
            case AST_FOR:
                break;
            case AST_WHILE:
                break;
            case AST_LOOP:
                break;
            case AST_VARDECL:
                break;
            case AST_CONSTDECL:
                break;
            case AST_CALL:
                break;
            case AST_ASSIGN:
                error = nulang__analyze_assign(analyzer, id);
                NULANG_ERROR_CHECK(error);
                break;
            default:
                break;
        }
        node = nulang__sibling(analyzer->ast, id, &id);
    }

    return NULANG_ERROR_NONE;
}

#endif

#endif
