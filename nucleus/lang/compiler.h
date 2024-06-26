#ifndef NULANG_COMPILER_H
#define NULANG_COMPILER_H

#include <nucleus/lang/lexer.h>
#include <nucleus/lang/mir.h>
#include <nucleus/lang/parser.h>

typedef struct
{
    nu_size_t               symbol_capacity;
    nu_size_t               type_capacity;
    nu_size_t               block_capacity;
    nu_size_t               node_capacity;
    nulang_allocator_info_t allocator;
} nulang_compiler_info_t;

typedef struct
{
    nulang__allocator_t    allocator;
    nulang__symbol_table_t symbols;
    nulang__type_table_t   types;
    nulang__ast_t          ast;
} nulang_compiler_t;

NU_API void nulang_compiler_info_default(nulang_compiler_info_t *info);
NU_API nulang_error_t nulang_compiler_init(const nulang_compiler_info_t *info,
                                           nulang_compiler_t *compiler);
NU_API nulang_error_t nulang_compiler_free(nulang_compiler_t *compiler);
NU_API nulang_error_t nulang_compiler_load(nulang_compiler_t *compiler,
                                           const nu_char_t   *source);

#ifdef NULANG_IMPL

#ifdef NU_STDLIB

#include <stdlib.h>

static void *
nulang__default_allocator (nu_size_t s, void *userdata)
{
    (void)userdata;
    return malloc(s);
}

#endif

void
nulang_compiler_info_default (nulang_compiler_info_t *info)
{
    info->symbol_capacity = 256;
    info->type_capacity   = 256;
    info->block_capacity  = 256;
    info->node_capacity   = 256;
#ifdef NU_STDLIB
    info->allocator.callback = nulang__default_allocator;
    info->allocator.userdata = NU_NULL;
#else
    info->allocator.callback = NU_NULL;
    info->allocator.userdata = NU_NULL;
#endif
}

nulang_error_t
nulang_compiler_init (const nulang_compiler_info_t *info,
                      nulang_compiler_t            *compiler)
{
    nulang_error_t error;

    compiler->allocator.callback = info->allocator.callback;
    compiler->allocator.userdata = info->allocator.userdata;

    error = nulang__symbol_table_init(info->symbol_capacity,
                                      info->block_capacity,
                                      &compiler->allocator,
                                      &compiler->symbols);
    NULANG_ERROR_CHECK(error);
    error = nulang__type_table_init(
        info->type_capacity, &compiler->allocator, &compiler->types);
    NULANG_ERROR_CHECK(error);

    error = nulang__ast_init(
        info->node_capacity, &compiler->allocator, &compiler->ast);
    NULANG_ERROR_CHECK(error);

    return NULANG_ERROR_NONE;
}
nulang_error_t
nulang_compiler_free (nulang_compiler_t *compiler)
{
    (void)compiler;
    /* TODO: free resources */
    return NULANG_ERROR_NONE;
}
static void
nulang__compiler_prepare (nulang_compiler_t *compiler)
{
    nulang__symbol_table_clear(&compiler->symbols);
    nulang__type_table_clear(&compiler->types);
    nulang__ast_clear(&compiler->ast);
}
nulang_error_t
nulang_compiler_load (nulang_compiler_t *compiler, const nu_char_t *source)
{
    nulang__lexer_t      lexer;
    nulang__parser_t     parser;
    nulang_error_t       error;
    nulang__error_data_t error_data;

    nulang__compiler_prepare(compiler);
    nulang__lexer_init(source, &lexer);
    nulang__parser_init(&lexer,
                        &compiler->ast,
                        &compiler->symbols,
                        &compiler->types,
                        &error_data,
                        &parser);
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

#endif

#endif