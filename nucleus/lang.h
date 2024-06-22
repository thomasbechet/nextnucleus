#ifndef NULANG_H
#define NULANG_H

#include <nucleus/lang/parser.h>

typedef struct
{
    nulang__allocator_t    allocator;
    nulang__symbol_table_t symbols;
    nulang__ast_t          ast;
} nulang_compiler_t;

nulang_error_t nulang_compiler_init(nu_size_t symbol_capacity,
                                    nu_size_t block_capacity,
                                    nu_size_t node_capacity,
                                    const nulang_allocator_info_t *alloc_info,
                                    nulang_compiler_t             *compiler);
nulang_error_t nulang_compiler_evaluate(nulang_compiler_t *compiler,
                                        const nu_char_t   *source);

#ifdef NULANG_IMPLEMENTATION

nulang_error_t
nulang_compiler_init (nu_size_t                      symbol_capacity,
                      nu_size_t                      block_capacity,
                      nu_size_t                      node_capacity,
                      const nulang_allocator_info_t *alloc_info,
                      nulang_compiler_t             *compiler)
{
    nulang_error_t error;

    compiler->allocator.callback = alloc_info->callback;
    compiler->allocator.userdata = alloc_info->userdata;

    error = nulang__symbol_table_init(symbol_capacity,
                                      block_capacity,
                                      &compiler->allocator,
                                      &compiler->symbols);
    NULANG_ERROR_CHECK(error);

    error
        = nulang__ast_init(node_capacity, &compiler->allocator, &compiler->ast);
    NULANG_ERROR_CHECK(error);

    return NULANG_ERROR_NONE;
}
static void
nulang__compiler_prepare (nulang_compiler_t *compiler)
{
    nulang__symbol_table_clear(&compiler->symbols);
    nulang__ast_clear(&compiler->ast);
}
nulang_error_t
nulang_compiler_evaluate (nulang_compiler_t *compiler, const nu_char_t *source)
{
    nulang__lexer_t      lexer;
    nulang__parser_t     parser;
    nulang_error_t       error;
    nulang__error_data_t error_data;

    nulang__compiler_prepare(compiler);
    nulang__lexer_init(source, &lexer);
    nulang__parser_init(
        &lexer, &compiler->ast, &compiler->symbols, &error_data, &parser);
    error = nulang__parse(&parser);

#ifdef NU_STDLIB
    if (error != NULANG_ERROR_NONE)
    {
        nulang__error_print(error, &error_data, source);
        return error;
    }
#endif

    return NULANG_ERROR_NONE;
}

#ifdef NU_STDLIB

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
nulang__print_symbol (const nulang__symbol_table_t *table,
                      nulang__symbol_id_t           symbol)
{
    printf("%.*s",
           (int)table->symbols[symbol].ident.n,
           table->symbols[symbol].ident.p);
    switch (table->symbols[symbol].type)
    {
        case SYMBOL_FUNCTION:
        case SYMBOL_ARGUMENT:
        case SYMBOL_CONSTANT:
        case SYMBOL_VARIABLE:
            printf(" %s",
                   NU_TYPE_NAMES[table->symbols[symbol].value.variable.type]);
            break;
        case SYMBOL_MODULE:
        case SYMBOL_EXTERNAL:
        case SYMBOL_UNKNOWN:
            break;
    }
}
static void
nulang__print_symbol_table (const nulang__symbol_table_t *table)
{
    nu_size_t i;
    for (i = 0; i < table->symbol_count; ++i)
    {
        nulang__print_symbol(table, i);
        printf("\n");
    }
}
static void
nulang__print_node (const nulang_compiler_t *compiler,
                    nu_u16_t                 depth,
                    nulang__node_id_t        id)
{
    nulang__node_t *node = &compiler->ast.nodes[id];
    nulang__print_depth(depth);
    printf("%s ", NULANG_AST_NAMES[node->type]);
    switch (node->type)
    {
        case AST_VARDECL:
            nulang__print_symbol(&compiler->symbols, node->value.symbol);
            break;
        case AST_IDENTIFIER:
            nulang__print_symbol(&compiler->symbols, node->value.symbol);
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
        nulang__print_node(compiler, depth + 1, node->first_child);
    }
    if (node->next_sibling != NULANG_NODE_NULL)
    {
        nulang__print_node(compiler, depth, node->next_sibling);
    }
}
void
nulang_compiler_print (const nulang_compiler_t *compiler)
{
    printf("==== AST ====\n");
    nulang__print_node(compiler, 0, compiler->ast.root);
    printf("==== SYMTAB ====\n");
    nulang__print_symbol_table(&compiler->symbols);
}

#endif

#endif

#endif
