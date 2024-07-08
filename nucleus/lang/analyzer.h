#ifndef NULANG_ANALYZER_H
#define NULANG_ANALYZER_H

#include <nucleus/lang/ast.h>
#include <nucleus/lang/builtin.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/report.h>
#include <nucleus/lang/symbol.h>
#include <nucleus/vm/table.h>
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

typedef enum
{
    NULANG_MEMBER_PRIMITIVE_FUNCTION,
    NULANG_MEMBER_MODULE_FUNCTION,
    NULANG_MEMBER_FIELD_LOOKUP
} nulang__member_type_t;

typedef struct
{
    nulang__member_type_t    type;
    nulang__string_t         ident;
    nu_archetype_t           archetype;
    nu_field_t               field;
    const nulang__builtin_t *builtin;
} nulang__member_t;

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
        case BINOP_NEQUAL: {
            switch (a.primitive)
            {
                case NU_PRIMITIVE_INT:
                case NU_PRIMITIVE_FIX:
                case NU_PRIMITIVE_ENTITY:
                    break;
                default:
                    return NULANG_ERROR_ILLEGAL_COMPARISON;
            }
            out->primitive = NU_PRIMITIVE_BOOL;
        }
        break;
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

static nulang__error_t
nulang__analyze_member_lookup (nulang__analyzer_t *analyzer,
                               nulang__node_id_t   member_id,
                               nulang__member_t   *out)
{
    nulang__error_t   error;
    nulang__node_t   *member   = &analyzer->ast->nodes[member_id];
    nulang__node_id_t child_id = member->first_child;
    nulang__node_t   *child    = &analyzer->ast->nodes[child_id];
    switch (child->type)
    {
        case AST_SYMBOL: {
            nulang__symbol_t *sym
                = &analyzer->symbols->symbols[child->value.symbol];
            switch (sym->type)
            {
                case SYMBOL_VARIABLE: {
                    if (sym->value.variable.vartype.primitive
                        == NU_PRIMITIVE_ENTITY)
                    {
                        out->type = NULANG_MEMBER_FIELD_LOOKUP;
                    }
                }
                break;
                default:
                    analyzer->error_data->span = member->span;
                    return NULANG_ERROR_UNKNOWN_SYMBOL_TYPE;
            }
        }
        break;
        default:
            NU_UNREACHABLE;
            break;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze_call (nulang__analyzer_t *analyzer,
                      nulang__node_id_t   call,
                      nulang__vartype_t  *out)
{
    nulang__node_id_t callee;
    nulang__node_t   *pcallee;

    pcallee = nulang__first_child(analyzer->ast, call, &callee);

    switch (pcallee->type)
    {
        case AST_SYMBOL: {
            const nulang__symbol_t *sym
                = &analyzer->symbols->symbols[pcallee->value.symbol];
            switch (sym->type)
            {
                case SYMBOL_FUNCTION:
                    break;
                case SYMBOL_EXTERNAL:
                    /* TODO */
                    return NULANG_ERROR_NOT_CALLABLE_SYMBOL;
                    break;
                case SYMBOL_UNKNOWN:
                    analyzer->error_data->span = pcallee->span;
                    return NULANG_ERROR_UNRESOLVED_SYMBOL;
                    break;
                default:
                    analyzer->error_data->span = pcallee->span;
                    return NULANG_ERROR_NOT_CALLABLE_SYMBOL;
                    break;
            }
        }
        break;
        case AST_BUILTIN: {
            switch (pcallee->value.builtin.type)
            {
                case BUILTIN_CONSTRUCTOR:
                    out->primitive = pcallee->value.builtin.value.constructor;
                    break;
                case BUILTIN_CONSTANT:
                    /* TODO: INVALID */
                    break;
                case BUILTIN_FUNCTION:
                    out->primitive
                        = pcallee->value.builtin.value.function->return_type;
                    break;
            }
        }
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
                    type->archetype = NU_ARCHETYPE_NULL;
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
                    *type = sym->value.argument.vartype;
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
        case AST_CALL: {
            error = nulang__analyze_call(analyzer, expr, type);
            NULANG_ERROR_CHECK(error);
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
        if (!nulang__vartype_compatible(symbol->value.variable.vartype,
                                        vartype))
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
        return NULANG_ERROR_UNRESOLVED_SYMBOL;
    }
}
static nulang__error_t
nulang__analyze_assign (nulang__analyzer_t *analyzer, nulang__node_id_t assign)
{
    nulang__node_id_t var, expr;
    nulang__node_t   *pvar;
    nulang__symbol_t *symbol;
    nulang__vartype_t assign_vartype, expr_vartype;
    nulang__error_t   error;

    pvar = nulang__first_child(analyzer->ast, assign, &var);
    nulang__sibling(analyzer->ast, var, &expr);

    symbol = &analyzer->symbols->symbols[pvar->value.symbol];

    error = nulang__analyze_expr(analyzer, expr, &expr_vartype);
    NULANG_ERROR_CHECK(error);

    switch (symbol->type)
    {
        case SYMBOL_ARGUMENT:
            assign_vartype = symbol->value.argument.vartype;
            break;
        case SYMBOL_VARIABLE:
            assign_vartype = symbol->value.variable.vartype;
            break;
        default:
            NU_UNREACHABLE;
            break;
    }

    if (assign_vartype.primitive != NU_PRIMITIVE_UNKNOWN)
    {
        if (!nulang__vartype_compatible(assign_vartype, expr_vartype))
        {
            analyzer->error_data->vartype_got    = expr_vartype;
            analyzer->error_data->vartype_expect = assign_vartype;
            analyzer->error_data->span           = pvar->span;
            return NULANG_ERROR_INCOMPATIBLE_TYPE;
        }
    }

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze_return_statement (nulang__analyzer_t *analyzer,
                                  nulang__block_id_t  block,
                                  nulang__node_id_t   node)
{
    nulang__symbol_id_t function, expr;
    nu_bool_t           in_function;
    nulang__vartype_t   vartype;
    nulang__error_t     error;
    in_function
        = nulang__check_in_function(analyzer->symbols, block, &function);
    if (!in_function)
    {
        analyzer->error_data->span = analyzer->ast->nodes[node].span;
        return NULANG_ERROR_RETURN_OUTSIDE_FUNCTION;
    }
    nulang__first_child(analyzer->ast, node, &expr);
    error = nulang__analyze_expr(analyzer, expr, &vartype);
    NULANG_ERROR_CHECK(error);
    if (!nulang__vartype_compatible(
            analyzer->symbols->symbols[function].value.function.return_type,
            vartype))
    {
        analyzer->error_data->span = analyzer->ast->nodes[expr].span;
        analyzer->error_data->vartype_expect
            = analyzer->symbols->symbols[function].value.function.return_type;
        analyzer->error_data->vartype_got = vartype;
        return NULANG_ERROR_INCOMPATIBLE_TYPE;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__resolve_symbols (nulang__analyzer_t *analyzer)
{
    nu_size_t i;
    for (i = 0; i < analyzer->symbols->symbol_count; ++i)
    {
        if (analyzer->symbols->symbols[i].type == SYMBOL_UNKNOWN)
        {
            analyzer->error_data->span = analyzer->symbols->symbols[i].span;
            return NULANG_ERROR_UNRESOLVED_SYMBOL;
        }
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t nulang__analyze_statement(nulang__analyzer_t *analyzer,
                                                 nulang__block_id_t  block,
                                                 nulang__node_id_t   stmt);
static nulang__error_t
nulang__analyze_child_statements (nulang__analyzer_t *analyzer,
                                  nulang__block_id_t  block,
                                  nulang__node_id_t   parent)
{
    nulang__error_t   error;
    nulang__node_t   *node;
    nulang__node_id_t child;

    node = nulang__first_child(analyzer->ast, parent, &child);
    while (node)
    {
        error = nulang__analyze_statement(analyzer, block, child);
        NULANG_ERROR_CHECK(error);
        node = nulang__sibling(analyzer->ast, child, &child);
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze_statement (nulang__analyzer_t *analyzer,
                           nulang__block_id_t  block,
                           nulang__node_id_t   stmt)
{
    nulang__error_t   error;
    nulang__vartype_t type;
    nulang__node_t   *node = &analyzer->ast->nodes[stmt];
    NU_ASSERT(nulang__is_statement(node->type));
    switch (node->type)
    {
        case AST_RETURN:
            error = nulang__analyze_return_statement(analyzer, block, stmt);
            NULANG_ERROR_CHECK(error);
            break;
        case AST_IF: {
            nulang__node_id_t child;
            node = nulang__first_child(analyzer->ast, stmt, &child);
            while (node)
            {
                if (node->type == AST_IFBODY) /* detect else body */
                {
                    error = nulang__analyze_child_statements(
                        analyzer, node->value.if_block, child);
                    NULANG_ERROR_CHECK(error);
                    break;
                }
                else
                {
                    error = nulang__analyze_expr(analyzer, child, &type);
                    NULANG_ERROR_CHECK(error);
                    if (type.primitive != NU_PRIMITIVE_BOOL)
                    {
                        analyzer->error_data->span = node->span;
                        return NULANG_ERROR_NON_BOOLEAN_EXPRESSION;
                    }
                    node = nulang__sibling(analyzer->ast, child, &child);
                    NU_ASSERT(node);
                    error = nulang__analyze_child_statements(
                        analyzer, node->value.if_block, child);
                    NULANG_ERROR_CHECK(error);
                    node = nulang__sibling(analyzer->ast, child, &child);
                }
            }
        }
        break;
        case AST_FOR:
            break;
        case AST_WHILE: {
            nulang__node_id_t child;
            node  = nulang__first_child(analyzer->ast, stmt, &child);
            error = nulang__analyze_expr(analyzer, child, &type);
            NULANG_ERROR_CHECK(error);
            if (type.primitive != NU_PRIMITIVE_BOOL)
            {
                analyzer->error_data->span = node->span;
                return NULANG_ERROR_NON_BOOLEAN_EXPRESSION;
            }
            error = nulang__analyze_child_statements(
                analyzer, node->value.while_block, child);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_LOOP: {
            error = nulang__analyze_child_statements(
                analyzer, node->value.loop_block, stmt);
            NULANG_ERROR_CHECK(error);
        }
        case AST_FUNCTION: {
            nulang__block_id_t block
                = analyzer->symbols->symbols[node->value.symbol]
                      .value.function.block;
            error = nulang__analyze_child_statements(analyzer, block, stmt);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_CALL:
            error = nulang__analyze_call(analyzer, stmt, &type);
            NULANG_ERROR_CHECK(error);
            break;
        case AST_VARDECL:
            error = nulang__analyze_vardecl(analyzer, stmt);
            NULANG_ERROR_CHECK(error);
            break;
        case AST_ASSIGN:
            error = nulang__analyze_assign(analyzer, stmt);
            NULANG_ERROR_CHECK(error);
            break;
        default:
            break;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__analyze (nulang__analyzer_t *analyzer)
{
    nulang__error_t error;

    /* resolve symbols */
    error = nulang__resolve_symbols(analyzer);
    NULANG_ERROR_CHECK(error);

    /* check statements */
    error = nulang__analyze_child_statements(
        analyzer, analyzer->symbols->block_global, analyzer->ast->root);
    NULANG_ERROR_CHECK(error);

    return NULANG_ERROR_NONE;
}

#endif

#endif
