#ifndef NULANG_TOKEN_H
#define NULANG_TOKEN_H

#include <nucleus/vm/math.h>
#include <nucleus/vm/types.h>

typedef struct
{
    nu_u16_t line;
    nu_u16_t column;
} nulang_location_t;

#define NULANG_FOREACH_TOKEN(TOKEN) \
    TOKEN(TOKEN_IDENTIFIER)         \
    TOKEN(TOKEN_PRIMITIVE)          \
    TOKEN(TOKEN_TYPE)               \
    TOKEN(TOKEN_LITERAL)            \
    TOKEN(TOKEN_IMPORT)             \
    TOKEN(TOKEN_FROM)               \
    TOKEN(TOKEN_EXPORT)             \
    TOKEN(TOKEN_COMMENT)            \
    TOKEN(TOKEN_SPACE)              \
    TOKEN(TOKEN_LPAREN)             \
    TOKEN(TOKEN_RPAREN)             \
    TOKEN(TOKEN_LBRACKET)           \
    TOKEN(TOKEN_RBRACKET)           \
    TOKEN(TOKEN_COMMA)              \
    TOKEN(TOKEN_DOT)                \
    TOKEN(TOKEN_COLON)              \
    TOKEN(TOKEN_ADD)                \
    TOKEN(TOKEN_SUB)                \
    TOKEN(TOKEN_MUL)                \
    TOKEN(TOKEN_DIV)                \
    TOKEN(TOKEN_EQUAL)              \
    TOKEN(TOKEN_ASSIGN)             \
    TOKEN(TOKEN_NEQUAL)             \
    TOKEN(TOKEN_LEQUAL)             \
    TOKEN(TOKEN_GEQUAL)             \
    TOKEN(TOKEN_LESS)               \
    TOKEN(TOKEN_GREATER)            \
    TOKEN(TOKEN_AND)                \
    TOKEN(TOKEN_OR)                 \
    TOKEN(TOKEN_NOT)                \
    TOKEN(TOKEN_LET)                \
    TOKEN(TOKEN_CONST)              \
    TOKEN(TOKEN_IF)                 \
    TOKEN(TOKEN_THEN)               \
    TOKEN(TOKEN_ELSE)               \
    TOKEN(TOKEN_ELIF)               \
    TOKEN(TOKEN_END)                \
    TOKEN(TOKEN_DO)                 \
    TOKEN(TOKEN_FOR)                \
    TOKEN(TOKEN_IN)                 \
    TOKEN(TOKEN_WHILE)              \
    TOKEN(TOKEN_FUNCTION)           \
    TOKEN(TOKEN_BREAK)              \
    TOKEN(TOKEN_CONTINUE)           \
    TOKEN(TOKEN_RETURN)             \
    TOKEN(TOKEN_EOF)
#define NULANG_GENERATE_ENUM(ENUM) ENUM,
#define NULANG_GENERATE_NAME(ENUM) #ENUM,

typedef enum
{
    NULANG_FOREACH_TOKEN(NULANG_GENERATE_ENUM)
} nulang__token_type_t;

static const nu_char_t *NULANG_TOKEN_NAMES[]
    = { NULANG_FOREACH_TOKEN(NULANG_GENERATE_NAME) };

typedef struct
{
    nulang_location_t start;
    nulang_location_t stop;
} nulang__span_t;

typedef struct
{
    const nu_char_t *p;
    nu_size_t        n;
} nulang__string_t;

typedef union
{
    nu_bool_t        b;
    nu_f32_t         f;
    nu_i32_t         i;
    nulang__string_t s;
} nulang__lit_value_t;

typedef enum
{
    LITERAL_NIL,
    LITERAL_BOOL,
    LITERAL_STRING,
    LITERAL_INT,
    LITERAL_FIX
} nulang__lit_type_t;

typedef struct
{
    nulang__lit_value_t value;
    nulang__lit_type_t  type;
} nulang__lit_t;

typedef union
{
    nu_primitive_t   primitive;
    nulang__lit_t    literal;
    nulang__string_t identifier;
} nulang__token_value_t;

typedef struct
{
    nulang__token_type_t  type;
    nulang__span_t        span;
    nulang__token_value_t value;
} nulang__token_t;

#define NULANG_TOKEN_EOF                     \
    (nulang__token_t)                        \
    {                                        \
        TOKEN_EOF, { { 0, 0 }, { 0, 0 } }, 0 \
    }
#define NULANG_TOKEN_SINGLE(t, loc) \
    (nulang__token_t)               \
    {                               \
        (t), { (loc), (loc) }, 0    \
    }
#define NULANG_TOKEN_DOUBLE(t, loc)                     \
    (nulang__token_t)                                   \
    {                                                   \
        (t), { (loc), { loc.line, loc.column + 1 } }, 0 \
    }

#endif
