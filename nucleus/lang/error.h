#ifndef NULANG_ERROR_H
#define NULANG_ERROR_H

#include <nucleus/lang/token.h>

typedef enum
{
    NULANG_ERROR_NONE,
    NULANG_ERROR_OUT_OF_NODE,
    NULANG_ERROR_OUT_OF_SYMBOL,
    NULANG_ERROR_OUT_OF_BLOCK,
    NULANG_ERROR_OUT_OF_MEMORY,
    NULANG_ERROR_INVALID,
    NULANG_ERROR_ILLEGAL_CHARACTER,
    NULANG_ERROR_UNTERMINATED_STRING,
    NULANG_ERROR_UNEXPECTED_TOKEN,
    NULANG_ERROR_SYMBOL_ALREADY_DEFINED,
    NULANG_ERROR_INVALID_ATOM_EXPRESSION,
    NULANG_ERROR_UNEXPECTED_BINOP,
    NULANG_ERROR_NON_STATEMENT_TOKEN,
    NULANG_ERROR_IDENTIFIER_AS_STATEMENT
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

#ifdef NULANG_IMPLEMENTATION

#ifdef NU_STDLIB

static void
nulang__error_print (nulang_error_t        code,
                     nulang__error_data_t *data,
                     const nu_char_t      *source)
{
    switch (code)
    {
        case NULANG_ERROR_NONE:
            break;
        case NULANG_ERROR_OUT_OF_NODE:
            printf("Out of node error");
        case NULANG_ERROR_OUT_OF_SYMBOL:
        case NULANG_ERROR_OUT_OF_BLOCK:
        case NULANG_ERROR_OUT_OF_MEMORY:
        case NULANG_ERROR_INVALID:
        case NULANG_ERROR_ILLEGAL_CHARACTER:
        case NULANG_ERROR_UNTERMINATED_STRING:
        case NULANG_ERROR_UNEXPECTED_TOKEN:
        case NULANG_ERROR_SYMBOL_ALREADY_DEFINED:
        case NULANG_ERROR_INVALID_ATOM_EXPRESSION:
        case NULANG_ERROR_UNEXPECTED_BINOP:
        case NULANG_ERROR_NON_STATEMENT_TOKEN:
        case NULANG_ERROR_IDENTIFIER_AS_STATEMENT:
            break;
    }
}

#endif

#endif

#endif
