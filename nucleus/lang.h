#ifndef NULANG_H
#define NULANG_H

#include <nucleus/lang/lexer.h>
#include <nucleus/lang/mir.h>
#include <nucleus/lang/parser.h>

typedef struct
{
    nulang__allocator_t    allocator;
    nulang__symbol_table_t symbols;
    nulang__ast_t          ast;
} nulang_compiler_t;

nulang_error_t nulang_compiler_init(nu_size_t symbol_capacity,
                                    nu_size_t type_capacity,
                                    nu_size_t block_capacity,
                                    nu_size_t node_capacity,
                                    const nulang_allocator_info_t *alloc_info,
                                    nulang_compiler_t             *compiler);
nulang_error_t nulang_compiler_evaluate(nulang_compiler_t *compiler,
                                        const nu_char_t   *source);

#ifdef NULANG_IMPL

nulang_error_t
nulang_compiler_init (nu_size_t                      symbol_capacity,
                      nu_size_t                      type_capacity,
                      nu_size_t                      block_capacity,
                      nu_size_t                      node_capacity,
                      const nulang_allocator_info_t *alloc_info,
                      nulang_compiler_t             *compiler)
{
    nulang_error_t error;

    compiler->allocator.callback = alloc_info->callback;
    compiler->allocator.userdata = alloc_info->userdata;

    error = nulang__symbol_table_init(symbol_capacity,
                                      type_capacity,
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

#include <nucleus/lang/print.h>

void
nulang_compiler_print_ast (const nulang_compiler_t *compiler)
{
    printf("==== AST ====\n");
    nulang__print_node(
        &compiler->symbols, &compiler->ast, 0, compiler->ast.root);
}
void
nulang_compiler_print_symtab (const nulang_compiler_t *compiler)
{
    printf("==== SYMTAB ====\n");
    nulang__print_symbol_table(&compiler->symbols);
}
nulang_error_t
nulang_compiler_print_tokens (const nu_char_t *source)
{
    nulang__lexer_t lexer;
    nulang__lexer_init(source, &lexer);
    printf("==== TOKENS ====\n");
    return nulang__lexer_print_tokens(&lexer);
}

#endif

#endif

#endif
