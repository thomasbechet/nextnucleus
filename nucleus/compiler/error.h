#ifndef NU_COMPILER_ERROR_H
#define NU_COMPILER_ERROR_H

typedef enum
{
    NU_COMPERR_NONE,
    NU_COMPERR_OUT_OF_NODE,
    NU_COMPERR_OUT_OF_MEMORY,
    NU_COMPERR_INVALID,
    NU_COMPERR_ILLEGAL_CHARACTER,
    NU_COMPERR_UNTERMINATED_STRING,
    NU_COMPERR_UNEXPECTED_TOKEN
} nu__compiler_error_t;

#define NU_COMPERR_CHECK(error)   \
    if (error != NU_COMPERR_NONE) \
    {                             \
        return error;             \
    }

#endif
