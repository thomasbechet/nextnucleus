#ifndef NULANG_PRINT_H
#define NULANG_PRINT_H

#include <nucleus/lang/compiler.h>

#ifdef NU_STDLIB

NU_API nulang_error_t nulang_source_print_tokens(const nu_char_t *source);
NU_API void nulang_compiler_print_symbols(const nulang_compiler_t *compiler);
NU_API void nulang_compiler_print_types(const nulang_compiler_t *compiler);
NU_API void nulang_compiler_print_ast(const nulang_compiler_t *compiler);

#endif

#ifdef NULANG_IMPL

#ifdef NU_STDLIB

#include <stdio.h>

static void
nulang__print_string (nulang__string_t s)
{
    printf("%.*s", (int)s.n, s.p);
}
static void
nulang__print_span (nulang__span_t span)
{
    printf("%d:%d %d:%d ",
           span.start.line,
           span.start.column,
           span.stop.line,
           span.stop.column);
}
static void
nulang__print_literal (const nulang__lit_t *lit)
{
    switch (lit->type)
    {
        case LITERAL_NIL:
            printf("nil");
            break;
        case LITERAL_BOOL:
            if (lit->value.b)
            {
                printf("bool(true)");
            }
            else
            {
                printf("bool(false)");
            }
            break;
        case LITERAL_STRING:
            printf("str(%.*s)", (int)lit->value.s.n, lit->value.s.p);
            break;
        case LITERAL_INT:
            printf("int(%d)", lit->value.i);
            break;
        case LITERAL_FIX:
            printf("fix(%f)", nu_ftofloat(lit->value.f));
            break;
    }
}
static void
nulang__print_token (const nulang__token_t *tok)
{
    printf("%s ", NULANG_TOKEN_NAMES[tok->type]);
    nulang__print_span(tok->span);
    switch (tok->type)
    {
        case TOKEN_IDENTIFIER:
            nulang__print_string(tok->value.identifier);
            break;
        case TOKEN_TYPE:
            nulang__print_string(tok->value.identifier);
            break;
        case TOKEN_LITERAL:
            nulang__print_literal(&tok->value.literal);
            break;
        case TOKEN_PRIMITIVE:
            printf("%s ", NU_PRIMITIVE_NAMES[tok->value.primitive]);
        default:
            break;
    }
}
static nulang_error_t
nulang__lexer_print_tokens (nulang__lexer_t *lexer)
{
    nulang__token_t tok;
    nulang_error_t  error;
    for (;;)
    {
        error = nulang__lexer_next(lexer, &tok);
        NULANG_ERROR_CHECK(error);
        nulang__print_token(&tok);
        printf("\n");
        if (tok.type == TOKEN_EOF)
        {
            break;
        }
    }
    return NULANG_ERROR_NONE;
}
static void
nulang__print_depth (nu_u16_t depth)
{
    nu_u16_t d = depth;
    while (d)
    {
        printf("  ");
        d--;
    }
}
static void
nulang__print_type (const nulang__type_table_t *table, nulang__type_id_t type)
{
    nulang__type_t *t = &table->types[type];
    printf("%.*s", (int)t->ident.n, t->ident.p);
}
static void
nulang__print_symbol (const nulang__symbol_table_t *symbols,
                      nulang__symbol_id_t           symbol)
{
    const nulang__symbol_t *sym = &symbols->symbols[symbol];
    printf("name: %.*s block: %d", (int)sym->ident.n, sym->ident.p, sym->block);
    switch (symbols->symbols[symbol].type)
    {
        case SYMBOL_FUNCTION:
            break;
        case SYMBOL_CONSTANT:
            break;
        case SYMBOL_VARIABLE:
            switch (sym->value.variable.vartype.type)
            {
                case VARTYPE_ENTITY:
                    printf(" type: %d", sym->value.variable.vartype.value.type);
                    break;
                case VARTYPE_PRIMITIVE:
                    printf(" type: %s",
                           NU_PRIMITIVE_NAMES[sym->value.variable.vartype.value
                                                  .primitive]);
                    break;
                case VARTYPE_UNKNOWN:
                    printf(" type: UNKNOWN");
                    break;
            }
            if (sym->value.variable.vartype.type == VARTYPE_ENTITY)
            {
                break;
            }
            break;
        case SYMBOL_MODULE:
            break;
        case SYMBOL_EXTERNAL:
            break;
        case SYMBOL_UNKNOWN:
            printf(" UNKNOWN");
            break;
        default:
            break;
    }
}
static void
nulang__print_symbol_table (const nulang__symbol_table_t *table)
{
    nu_size_t i;
    for (i = 0; i < table->symbol_count; ++i)
    {
        printf("[%ld] ", i);
        nulang__print_symbol(table, i);
        printf("\n");
    }
}
static void
nulang__print_type_table (const nulang__type_table_t *table)
{
    nu_size_t i;
    for (i = 0; i < table->type_count; ++i)
    {
        printf("[%ld] ", i);
        nulang__print_type(table, i);
        printf("\n");
    }
}
static void
nulang__print_node (const nulang__symbol_table_t *symbols,
                    const nulang__ast_t          *ast,

                    nu_u16_t          depth,
                    nulang__node_id_t id)
{
    nulang__node_t *node = &ast->nodes[id];
    nulang__print_depth(depth);
    printf("%s ", NULANG_AST_NAMES[node->type]);
    switch (node->type)
    {
        case AST_VARDECL:
            printf("symbol: %d ", node->value.symbol);
            break;
        case AST_SYMBOL:
            printf("symbol: %d ", node->value.symbol);
            break;
        case AST_LITERAL:
            nulang__print_literal(&node->value.literal);
            break;
        case AST_BINOP:
            printf("%s", NULANG_BINOP_NAMES[node->value.binop]);
            break;
        case AST_UNOP:
            printf("%s", NULANG_UNOP_NAMES[node->value.unop]);
            break;
        default:
            break;
    }
    printf("\n");
    if (node->first_child != NULANG_NODE_NULL)
    {
        nulang__print_node(symbols, ast, depth + 1, node->first_child);
    }
    if (node->next_sibling != NULANG_NODE_NULL)
    {
        nulang__print_node(symbols, ast, depth, node->next_sibling);
    }
}

nulang_error_t
nulang_source_print_tokens (const nu_char_t *source)
{
    nulang__lexer_t lexer;
    nulang__lexer_init(source, &lexer);
    printf("==== TOKENS ====\n");
    return nulang__lexer_print_tokens(&lexer);
}
void
nulang_compiler_print_symbols (const nulang_compiler_t *compiler)
{
    printf("==== SYMBOLS ====\n");
    nulang__print_symbol_table(&compiler->symbols);
}
void
nulang_compiler_print_types (const nulang_compiler_t *compiler)
{
    printf("==== TYPES ====\n");
    nulang__print_type_table(&compiler->types);
}
void
nulang_compiler_print_ast (const nulang_compiler_t *compiler)
{
    printf("==== NODES ====\n");
    nulang__print_node(
        &compiler->symbols, &compiler->ast, 0, compiler->ast.root);
}

#endif

#endif

#endif
