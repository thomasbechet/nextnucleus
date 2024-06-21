#ifndef NU_COMPILER_H
#define NU_COMPILER_H

#include "nucleus/compiler/error.h"
#include <nucleus/compiler/allocator.h>
#include <nucleus/compiler/parser.h>

typedef struct
{
    nu__compiler_allocator_t allocator;
    nu__symbol_table_t       symbols;
    nu__ast_t                ast;
    nu__lexer_t              lexer;
    nu__parser_t             parser;
} nu_compiler_t;

nu_compiler_error_t nu_compiler_init(
    nu_size_t                           symbol_capacity,
    nu_size_t                           block_capacity,
    nu_size_t                           node_capacity,
    const nu_compiler_allocator_info_t *alloc_info,
    nu_compiler_t                      *compiler);
nu_compiler_error_t nu_compiler_evaluate(nu_compiler_t   *compiler,
                                         const nu_char_t *source);

#ifdef NU_IMPLEMENTATION

nu_compiler_error_t
nu_compiler_init (nu_size_t                           symbol_capacity,
                  nu_size_t                           block_capacity,
                  nu_size_t                           node_capacity,
                  const nu_compiler_allocator_info_t *alloc_info,
                  nu_compiler_t                      *compiler)
{
    nu_compiler_error_t error;

    compiler->allocator.callback = alloc_info->callback;
    compiler->allocator.userdata = alloc_info->userdata;

    error = nu__symbol_table_init(symbol_capacity,
                                  block_capacity,
                                  &compiler->allocator,
                                  &compiler->symbols);
    NU_COMPERR_CHECK(error);

    error = nu__ast_init(node_capacity, &compiler->allocator, &compiler->ast);
    NU_COMPERR_CHECK(error);

    return NU_COMPERR_NONE;
}
nu_compiler_error_t
nu_compiler_evaluate (nu_compiler_t *compiler, const nu_char_t *source)
{
    return NU_COMPERR_NONE;
}

#endif

#endif
