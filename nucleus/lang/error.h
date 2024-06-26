#ifndef NULANG_ERROR_H
#define NULANG_ERROR_H

#include <nucleus/lang/token.h>

typedef enum
{
    NULANG_ERROR_NONE,
    /* allocation error */
    NULANG_ERROR_OUT_OF_NODE,
    NULANG_ERROR_OUT_OF_SYMBOL,
    NULANG_ERROR_OUT_OF_TYPE,
    NULANG_ERROR_OUT_OF_BLOCK,
    NULANG_ERROR_OUT_OF_MEMORY,
    /* lexer error */
    NULANG_ERROR_ILLEGAL_CHARACTER,
    NULANG_ERROR_UNTERMINATED_STRING,
    NULANG_ERROR_UNEXPECTED_TOKEN,
    /* parser error */
    NULANG_ERROR_SYMBOL_ALREADY_DEFINED,
    NULANG_ERROR_INVALID_ATOM_EXPRESSION,
    NULANG_ERROR_UNEXPECTED_BINOP,
    NULANG_ERROR_NON_STATEMENT_TOKEN,
    NULANG_ERROR_IDENTIFIER_AS_STATEMENT,
    NULANG_ERROR_EMPTY_TYPE
} nulang_error_t;

#define NULANG_ERROR_CHECK(error)   \
    if (error != NULANG_ERROR_NONE) \
    {                               \
        return error;               \
    }

typedef struct
{
    nulang__span_t       span;
    nulang__token_type_t expect;
    nulang__token_type_t got;
} nulang__error_data_t;

#endif
