#ifndef NULANG_ANALYZER_H
#define NULANG_ANALYZER_H

#include <nucleus/lang/ast.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/report.h>
#include <nucleus/vm/types.h>

#ifdef NU_IMPL

typedef struct
{
    nulang__vartype_t type;
    nulang__value_t   value;
} nulang__variable_t;

typedef struct
{
    nulang__ast_t          *ast;
    nulang__symbol_table_t *symbols;
    nulang__error_data_t   *error_data;
} nulang__analyzer_t;

static nulang__error_t
nulang__check_binop (nulang__binop_t       op,
                     nulang__vartype_t     a,
                     nulang__vartype_t     b,
                     nulang__vartype_t    *out,
                     nulang__error_data_t *error)
{
    if (a.primitive != b.primitive)
    {
        /* TODO: try to promote one type to another */
        error->vartype_expect.primitive = a.primitive;
        return NULANG_ERROR_INCOMPATIBLE_TYPE;
    }
    switch (op)
    {
        case BINOP_ADD:
        case BINOP_SUB:
        case BINOP_MUL:
        case BINOP_DIV:
            switch (a.primitive)
            {
                case NU_PRIMITIVE_INT:
                case NU_PRIMITIVE_FIX:
                case NU_PRIMITIVE_IVEC2:
                case NU_PRIMITIVE_IVEC3:
                case NU_PRIMITIVE_IVEC4:
                case NU_PRIMITIVE_VEC2:
                case NU_PRIMITIVE_VEC3:
                case NU_PRIMITIVE_VEC4:
                case NU_PRIMITIVE_QUAT:
                    break;
                default:
                    return NULANG_ERROR_ILLEGAL_ARITHMETIC;
            }
            out->primitive = a.primitive;
            break;
        case BINOP_EQUAL:
        case BINOP_NEQUAL:
        case BINOP_LEQUAL:
        case BINOP_GEQUAL:
        case BINOP_LESS:
        case BINOP_GREATER:
            switch (a.primitive)
            {
                case NU_PRIMITIVE_INT:
                case NU_PRIMITIVE_FIX:
                    break;
                default:
                    return NULANG_ERROR_ILLEGAL_COMPARISON;
            }
            out->primitive = NU_PRIMITIVE_BOOL;
            break;
        case BINOP_AND:
        case BINOP_OR:
            if (a.primitive != NU_PRIMITIVE_BOOL)
            {
                return NULANG_ERROR_ILLEGAL_LOGICAL;
            }
            out->primitive = NU_PRIMITIVE_BOOL;
            break;
    }
    return NULANG_ERROR_NONE;
}
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
typedef union
{
    nulang__symbol_id_t function;
    int                 constructor;
    int                 builtin;
} nulang__callable_t;
static nulang__error_t
nulang__find_callable_recurs (nulang__analyzer_t *analyzer,
                              nulang__node_id_t   callee,
                              nulang__callable_t *out)
{
}
static nulang__error_t
nulang__find_callable (nulang__analyzer_t *analyzer,
                       nulang__node_id_t   callee,
                       nulang__callable_t *out)
{
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyzer_call (nulang__analyzer_t *analyzer,
                       nulang__node_id_t   call,
                       nulang__vartype_t  *out)
{
    nulang__node_t   *node, *pcallee;
    nulang__node_id_t callee;
    nulang__symbol_t *symbol;
    nulang__vartype_t vartype;
    nulang__error_t   error;

    node    = &analyzer->ast->nodes[call];
    pcallee = nulang__first_child(analyzer->ast, call, &callee);

    switch (pcallee->type)
    {
        case AST_SYMBOL:
        case AST_PRIMITIVE:
        case AST_FIELDLOOKUP:
            break;
        default:
            break;
    }

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze_expr (nulang__analyzer_t *analyzer,
                      nulang__node_id_t   expr,
                      nulang__vartype_t  *type)
{
    nulang__node_t *node;
    nulang__error_t error;

    NU_ASSERT(expr != NULANG_NODE_NULL);
    node = &analyzer->ast->nodes[expr];
    NU_ASSERT(nulang__is_expression(node->type));

    type->primitive = NU_PRIMITIVE_UNKNOWN;

    switch (node->type)
    {
        case AST_LITERAL: {
            switch (node->value.literal.type)
            {
                case LITERAL_NIL:
                    type->primitive = NU_PRIMITIVE_ENTITY;
                    type->archetype = NU_NULL;
                    break;
                case LITERAL_BOOL:
                    type->primitive = NU_PRIMITIVE_BOOL;
                    break;
                case LITERAL_STRING:
                    /*TODO */
                    NU_UNREACHABLE;
                    break;
                case LITERAL_INT:
                    type->primitive = NU_PRIMITIVE_INT;
                    break;
                case LITERAL_FIX:
                    type->primitive = NU_PRIMITIVE_FIX;
                    break;
            }
        }
        break;
        case AST_SYMBOL: {
            nulang__symbol_t *sym
                = &analyzer->symbols->symbols[node->value.symbol];
            switch (sym->type)
            {
                case SYMBOL_ARGUMENT:
                    break;
                case SYMBOL_VARIABLE:
                    *type = sym->value.variable.vartype;
                    break;
                case SYMBOL_EXTERNAL:
                    break;
                case SYMBOL_UNKNOWN:
                    analyzer->error_data->span = node->span;
                    return NULANG_ERROR_UNKNOWN_SYMBOL_TYPE;
                default:
                    break;
            }
        }
        break;
        case AST_FIELDLOOKUP: {
        }
        break;
        case AST_CALL: {
        }
        break;
        case AST_SINGLETON:
        case AST_INSERT: {
            type->primitive = NU_PRIMITIVE_ENTITY;
            type->archetype = node->value.archetype;
        }
        break;
        case AST_BINOP: {
            nulang__node_id_t rhs, lhs;
            nulang__vartype_t rhs_vartype, lhs_vartype;
            nulang__first_child(analyzer->ast, expr, &rhs);
            nulang__sibling(analyzer->ast, rhs, &lhs);
            error = nulang__analyze_expr(analyzer, rhs, &rhs_vartype);
            NULANG_ERROR_CHECK(error);
            error = nulang__analyze_expr(analyzer, lhs, &lhs_vartype);
            NULANG_ERROR_CHECK(error);
            error                      = nulang__check_binop(node->value.binop,
                                        rhs_vartype,
                                        lhs_vartype,
                                        type,
                                        analyzer->error_data);
            analyzer->error_data->span = node->span;
            NULANG_ERROR_CHECK(error);
        }
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
nulang__analyze_vardecl (nulang__analyzer_t *analyzer,
                         nulang__node_id_t   vardecl)

{
    nulang__node_id_t expr;
    nulang__node_t   *node;
    nulang__symbol_t *symbol;
    nulang__vartype_t vartype;
    nulang__error_t   error;

    node = &analyzer->ast->nodes[vardecl];
    nulang__first_child(analyzer->ast, vardecl, &expr);

    symbol = &analyzer->symbols->symbols[node->value.symbol];

    error = nulang__analyze_expr(analyzer, expr, &vartype);
    NULANG_ERROR_CHECK(error);

    if (symbol->value.variable.vartype.primitive != NU_PRIMITIVE_UNKNOWN)
    {
        if (!nulang__vartype_equals(symbol->value.variable.vartype, vartype))
        {
            analyzer->error_data->vartype_got = vartype;
            analyzer->error_data->vartype_expect
                = symbol->value.variable.vartype;
            analyzer->error_data->span = node->span;
            return NULANG_ERROR_INCOMPATIBLE_TYPE;
        }
        return NULANG_ERROR_NONE;
    }
    else if (vartype.primitive != NU_PRIMITIVE_UNKNOWN)
    {
        symbol->value.variable.vartype = vartype;
        return NULANG_ERROR_NONE;
    }
    else
    {
        analyzer->error_data->span = node->span;
        return NULANG_ERROR_UNKNOWN_SYMBOL_TYPE;
    }
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

    symbol = &analyzer->symbols->symbols[pvar->value.symbol];

    error = nulang__analyze_expr(analyzer, expr, &vartype);
    NULANG_ERROR_CHECK(error);

    if (symbol->value.variable.vartype.primitive != NU_PRIMITIVE_UNKNOWN)
    {
        if (!nulang__vartype_equals(symbol->value.variable.vartype, vartype))
        {
            analyzer->error_data->vartype_got = vartype;
            analyzer->error_data->vartype_expect
                = symbol->value.variable.vartype;
            analyzer->error_data->span = pvar->span;
            return NULANG_ERROR_INCOMPATIBLE_TYPE;
        }
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
            case AST_CALL:
                break;
            case AST_VARDECL:
                error = nulang__analyze_vardecl(analyzer, id);
                NULANG_ERROR_CHECK(error);
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
