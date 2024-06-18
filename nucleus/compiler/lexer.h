#ifndef NU_LEXER_H
#define NU_LEXER_H

#include "nucleus/compiler/error.h"
#include "nucleus/vm/string.h"
#include <nucleus/vm/math.h>
#include <nucleus/vm/types.h>

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_u16_t line;
    nu_u16_t column;
} nu_source_location_t;

#define NU_FOREACH_TOKEN(TOKEN) \
    TOKEN(TOKEN_IDENTIFIER)     \
    TOKEN(TOKEN_TYPE)           \
    TOKEN(TOKEN_LITERAL)        \
    TOKEN(TOKEN_IMPORT)         \
    TOKEN(TOKEN_FROM)           \
    TOKEN(TOKEN_EXPORT)         \
    TOKEN(TOKEN_COMMENT)        \
    TOKEN(TOKEN_SPACE)          \
    TOKEN(TOKEN_LPAREN)         \
    TOKEN(TOKEN_RPAREN)         \
    TOKEN(TOKEN_LBRACKET)       \
    TOKEN(TOKEN_RBRACKET)       \
    TOKEN(TOKEN_COMMA)          \
    TOKEN(TOKEN_DOT)            \
    TOKEN(TOKEN_COLON)          \
    TOKEN(TOKEN_ADD)            \
    TOKEN(TOKEN_SUB)            \
    TOKEN(TOKEN_MUL)            \
    TOKEN(TOKEN_DIV)            \
    TOKEN(TOKEN_ASSIGN)         \
    TOKEN(TOKEN_EQUAL)          \
    TOKEN(TOKEN_NEQUAL)         \
    TOKEN(TOKEN_LEQUAL)         \
    TOKEN(TOKEN_GEQUAL)         \
    TOKEN(TOKEN_LESS)           \
    TOKEN(TOKEN_GREATER)        \
    TOKEN(TOKEN_AND)            \
    TOKEN(TOKEN_OR)             \
    TOKEN(TOKEN_NOT)            \
    TOKEN(TOKEN_LOCAL)          \
    TOKEN(TOKEN_CONST)          \
    TOKEN(TOKEN_IF)             \
    TOKEN(TOKEN_THEN)           \
    TOKEN(TOKEN_ELSE)           \
    TOKEN(TOKEN_ELIF)           \
    TOKEN(TOKEN_END)            \
    TOKEN(TOKEN_DO)             \
    TOKEN(TOKEN_FOR)            \
    TOKEN(TOKEN_IN)             \
    TOKEN(TOKEN_WHILE)          \
    TOKEN(TOKEN_FUNCTION)       \
    TOKEN(TOKEN_BREAK)          \
    TOKEN(TOKEN_CONTINUE)       \
    TOKEN(TOKEN_RETURN)         \
    TOKEN(TOKEN_PRIMITIVE)      \
    TOKEN(TOKEN_EOF)
#define NU_GENERATE_ENUM(ENUM) ENUM,
#define NU_GENERATE_NAME(ENUM) #ENUM,

typedef enum
{
    NU_FOREACH_TOKEN(NU_GENERATE_ENUM)
} nu__token_type_t;

static const nu_char_t *NU_TOKEN_NAMES[]
    = { NU_FOREACH_TOKEN(NU_GENERATE_NAME) };

typedef struct
{
    nu_source_location_t start;
    nu_source_location_t stop;
} nu__source_span_t;

typedef struct
{
    const nu_char_t *p;
    nu_size_t        n;
} nu__lit_string_value_t;

typedef union
{
    nu_bool_t              b;
    nu_f32_t               f;
    nu_i32_t               i;
    nu__lit_string_value_t s;
} nu__lit_value_t;

typedef enum
{
    LITERAL_NIL,
    LITERAL_BOOL,
    LITERAL_STRING,
    LITERAL_INT,
    LITERAL_FLOAT
} nu__lit_type_t;

typedef struct
{
    nu__lit_value_t value;
    nu__lit_type_t  type;
} nu__lit_t;

typedef union
{
    nu_type_t              primitive;
    nu__lit_t              literal;
    nu__lit_string_value_t identifier;
} nu__token_value_t;

typedef struct
{
    nu__token_type_t  type;
    nu__source_span_t span;
    nu__token_value_t value;
} nu__token_t;

#define NU_TOKEN_EOF                         \
    (nu__token_t)                            \
    {                                        \
        TOKEN_EOF, { { 0, 0 }, { 0, 0 } }, 0 \
    }
#define NU_TOKEN_SINGLE(t, loc)  \
    (nu__token_t)                \
    {                            \
        (t), { (loc), (loc) }, 0 \
    }
#define NU_TOKEN_DOUBLE(t, loc)                         \
    (nu__token_t)                                       \
    {                                                   \
        (t), { (loc), { loc.line, loc.column + 1 } }, 0 \
    }

#define NU_LEXER_MAX_PEEKS 5

typedef struct
{
    /* source streaming info */
    const nu_char_t     *source;
    const nu_char_t     *ptr;
    nu_source_location_t next_loc;
    /* character peek */
    nu_char_t            peek_char;
    nu_source_location_t peek_loc;
    nu_bool_t            has_peek;
    /* token peeks */
    nu__token_t peeks[NU_LEXER_MAX_PEEKS];
    nu_size_t   peek_count;
} nu__lexer_t;

static void
nu__lexer_init (const nu_char_t *source, nu__lexer_t *lexer)
{
    lexer->source          = source;
    lexer->ptr             = source;
    lexer->next_loc.line   = 1;
    lexer->next_loc.column = 1;

    lexer->has_peek = NU_FALSE;

    lexer->peek_count = 0;
}

static nu_bool_t
nu__source_stream_next (nu__lexer_t          *lexer,
                        nu_char_t            *c,
                        nu_source_location_t *loc)
{
    if (!lexer->ptr)
    {
        /* end reached */
        return NU_FALSE;
    }
    *loc = lexer->next_loc;
    *c   = *(lexer->ptr++);
    if (*c == '\0')
    {
        lexer->ptr = NU_NULL;
        return NU_FALSE;
    }
    if (*c == '\n')
    {
        *c = ' ';
        lexer->next_loc.line += 1;
        lexer->next_loc.column = 1;
    }
    else
    {
        lexer->next_loc.column += 1;
    }
    return NU_TRUE;
}

static nu_bool_t
nu__lexer_next_char (nu__lexer_t          *lexer,
                     nu_char_t            *c,
                     nu_source_location_t *loc)
{
    if (lexer->has_peek)
    {
        lexer->has_peek = NU_FALSE;
        *c              = lexer->peek_char;
        *loc            = lexer->peek_loc;
        return NU_TRUE;
    }
    else
    {
        return nu__source_stream_next(lexer, c, loc);
    }
}
static nu_bool_t
nu__lexer_peek_char (nu__lexer_t          *lexer,
                     nu_char_t            *c,
                     nu_source_location_t *loc)
{
    if (lexer->has_peek)
    {
        *c   = lexer->peek_char;
        *loc = lexer->peek_loc;
        return NU_TRUE;
    }
    else
    {
        lexer->has_peek = nu__source_stream_next(
            lexer, &lexer->peek_char, &lexer->peek_loc);
        *c   = lexer->peek_char;
        *loc = lexer->peek_loc;
        return lexer->has_peek;
    }
}

static void
nu__consume_char (nu__lexer_t *lexer)
{
    nu_char_t            c;
    nu_source_location_t dloc;
    nu__lexer_next_char(lexer, &c, &dloc);
}
static nu__compiler_error_t
nu__consume_single_char_token (nu__lexer_t         *lexer,
                               nu__token_type_t     type,
                               nu_source_location_t loc,
                               nu__token_t         *token)
{
    nu__consume_char(lexer);
    *token = NU_TOKEN_SINGLE(type, loc);
    return NU_COMPERR_NONE;
}
static void
nu__consume_comment (nu__lexer_t *lexer)
{
    nu_source_location_t start_loc, loc;
    nu_char_t            c;
    nu__lexer_peek_char(lexer, &c, &start_loc);
    while (nu__lexer_peek_char(lexer, &c, &loc))
    {
        if (loc.line != start_loc.line)
        {
            break;
        }
        nu__consume_char(lexer);
    }
}
static void
nu__consume_spaces (nu__lexer_t *lexer)
{
    nu_char_t            c;
    nu_source_location_t loc;
    while (nu__lexer_peek_char(lexer, &c, &loc))
    {
        if (c != ' ')
        {
            break;
        }
        nu__consume_char(lexer);
    }
}

#define NU_NUMERIC_CHAR(c) (c >= '0' && c <= '9')
#define NU_IDENTIFIER_CHAR(c) \
    (NU_NUMERIC_CHAR(c) || (c >= 'a' && c <= 'z') || c == '_')
#define NU_MATCH_TOKEN(s, t, n) (n == nu_strlen(t) && nu_strncmp(s, t, n) == 0)

static nu__compiler_error_t
nu__consume_string (nu__lexer_t *lexer, nu__token_t *token)
{
    nu_source_location_t start_loc, stop_loc, loc;
    nu_char_t            c;
    const nu_char_t     *s;
    nu_size_t            n;

    nu__lexer_peek_char(lexer, &c, &start_loc);
    stop_loc = start_loc;
    while (nu__lexer_peek_char(lexer, &c, &loc))
    {
        if (c == '\'')
        {
            stop_loc                       = loc;
            token->span.start              = start_loc;
            token->span.stop               = stop_loc;
            token->type                    = TOKEN_LITERAL;
            token->value.literal.type      = LITERAL_STRING;
            token->value.literal.value.s.p = s;
            token->value.literal.value.s.n = n;
            return NU_COMPERR_NONE;
        }
        n++;
        nu__consume_char(lexer);
    }
    return NU_COMPERR_UNTERMINATED_STRING;
}
static nu__compiler_error_t
nu__consume_number (nu__lexer_t *lexer, nu__token_t *token)
{
    nu_source_location_t start_loc, stop_loc, loc;
    nu_char_t            c;
    const nu_char_t     *s;
    nu_size_t            n;
    nu_bool_t            has_dot = NU_FALSE;

    s = lexer->ptr - 1; /* small hack */
    n = 0;
    nu__lexer_peek_char(lexer, &c, &start_loc);
    stop_loc = start_loc;
    while (nu__lexer_peek_char(lexer, &c, &loc))
    {
        if (NU_NUMERIC_CHAR(c) || c == '.')
        {
            stop_loc = loc;
            if (c == '.')
            {
                if (has_dot)
                {
                    return NU_COMPERR_ILLEGAL_CHARACTER;
                }
                has_dot = NU_TRUE;
            }
            n++;
            nu__consume_char(lexer);
        }
        else if (NU_IDENTIFIER_CHAR(c))
        {
            return NU_COMPERR_ILLEGAL_CHARACTER;
        }
        else
        {
            break;
        }
    }
    token->span.start = start_loc;
    token->span.stop  = stop_loc;
    if (has_dot)
    {
        nu_f32_t v;
        if (nu_fparse(s, n, &v) != NU_ERROR_NONE)
        {
            return NU_COMPERR_ILLEGAL_CHARACTER;
        }
        token->type                  = TOKEN_LITERAL;
        token->value.literal.type    = LITERAL_FLOAT;
        token->value.literal.value.f = v;
    }
    else
    {
        nu_i32_t v;
        if (nu_iparse(s, n, &v) != NU_ERROR_NONE)
        {
            return NU_COMPERR_ILLEGAL_CHARACTER;
        }
        token->type                  = TOKEN_LITERAL;
        token->value.literal.type    = LITERAL_INT;
        token->value.literal.value.i = v;
    }
    return NU_COMPERR_NONE;
}
static nu__compiler_error_t
nu__consume_identifier (nu__lexer_t *lexer, nu__token_t *token)
{
    nu_source_location_t start_loc, stop_loc, loc;
    nu_char_t            c;
    const nu_char_t     *s;
    nu_size_t            n;

    s = lexer->ptr - 1; /* small hack */
    n = 0;
    nu__lexer_peek_char(lexer, &c, &start_loc);
    stop_loc = start_loc;
    while (nu__lexer_peek_char(lexer, &c, &loc))
    {
        if (!NU_IDENTIFIER_CHAR(c))
        {
            break;
        }
        stop_loc = loc;
        n++;
        nu__consume_char(lexer);
    }
    token->span.start = start_loc;
    token->span.stop  = stop_loc;

    /* check keyword */
    token->type = TOKEN_EOF;
    if (NU_MATCH_TOKEN(s, "import", n))
    {
        token->type = TOKEN_IMPORT;
    }
    else if (NU_MATCH_TOKEN(s, "from", n))
    {
        token->type = TOKEN_FROM;
    }
    else if (NU_MATCH_TOKEN(s, "export", n))
    {
        token->type = TOKEN_EXPORT;
    }
    else if (NU_MATCH_TOKEN("and", s, n))
    {
        token->type = TOKEN_AND;
    }
    else if (NU_MATCH_TOKEN(s, "or", n))
    {
        token->type = TOKEN_OR;
    }
    else if (NU_MATCH_TOKEN(s, "not", n))
    {
        token->type = TOKEN_NOT;
    }
    else if (NU_MATCH_TOKEN(s, "local", n))
    {
        token->type = TOKEN_LOCAL;
    }
    else if (NU_MATCH_TOKEN(s, "const", n))
    {
        token->type = TOKEN_CONST;
    }
    else if (NU_MATCH_TOKEN(s, "if", n))
    {
        token->type = TOKEN_IF;
    }
    else if (NU_MATCH_TOKEN(s, "then", n))
    {
        token->type = TOKEN_THEN;
    }
    else if (NU_MATCH_TOKEN(s, "else", n))
    {
        token->type = TOKEN_ELSE;
    }
    else if (NU_MATCH_TOKEN(s, "elif", n))
    {
        token->type = TOKEN_ELIF;
    }
    else if (NU_MATCH_TOKEN(s, "end", n))
    {
        token->type = TOKEN_END;
    }
    else if (NU_MATCH_TOKEN(s, "do", n))
    {
        token->type = TOKEN_DO;
    }
    else if (NU_MATCH_TOKEN(s, "for", n))
    {
        token->type = TOKEN_FOR;
    }
    else if (NU_MATCH_TOKEN(s, "in", n))
    {
        token->type = TOKEN_IN;
    }
    else if (NU_MATCH_TOKEN(s, "while", n))
    {
        token->type = TOKEN_WHILE;
    }
    else if (NU_MATCH_TOKEN(s, "function", n))
    {
        token->type = TOKEN_FUNCTION;
    }
    else if (NU_MATCH_TOKEN(s, "break", n))
    {
        token->type = TOKEN_BREAK;
    }
    else if (NU_MATCH_TOKEN(s, "continue", n))
    {
        token->type = TOKEN_CONTINUE;
    }
    else if (NU_MATCH_TOKEN(s, "return", n))
    {
        token->type = TOKEN_RETURN;
    }
    if (token->type != TOKEN_EOF)
    {
        return NU_COMPERR_NONE;
    }

    /* check if token is a literal */
    if (NU_MATCH_TOKEN(s, "true", n))
    {
        token->type                  = TOKEN_LITERAL;
        token->value.literal.value.b = NU_TRUE;
        token->value.literal.type    = LITERAL_BOOL;
    }
    else if (NU_MATCH_TOKEN(s, "false", n))
    {
        token->type                  = TOKEN_LITERAL;
        token->value.literal.value.b = NU_FALSE;
        token->value.literal.type    = LITERAL_BOOL;
    }
    else if (NU_MATCH_TOKEN(s, "nil", n))
    {
        token->type               = TOKEN_LITERAL;
        token->value.literal.type = LITERAL_NIL;
    }
    if (token->type == TOKEN_LITERAL)
    {
        return NU_COMPERR_NONE;
    }

    /* check primitive */
    if (NU_MATCH_TOKEN(s, "int", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_TYPE_INT;
    }
    else if (NU_MATCH_TOKEN(s, "float", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_TYPE_FLOAT;
    }
    else if (NU_MATCH_TOKEN(s, "string", n))
    {
        token->type = TOKEN_PRIMITIVE;
        /* TODO */
        token->value.primitive = NU_TYPE_INT;
    }
    else if (NU_MATCH_TOKEN(s, "vec2", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_TYPE_FV2;
    }
    else if (NU_MATCH_TOKEN(s, "vec3", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_TYPE_FV3;
    }
    if (token->type == TOKEN_PRIMITIVE)
    {
        return NU_COMPERR_NONE;
    }

    /* check identifier */
    token->type               = TOKEN_IDENTIFIER;
    token->value.identifier.p = s;
    token->value.identifier.n = n;

    return NU_COMPERR_NONE;
}

static nu__compiler_error_t
nu__lexer_parse_token (nu__lexer_t *lexer, nu__token_t *token)
{
    nu_char_t            c, next;
    nu_source_location_t loc, loc2;
    while (nu__lexer_peek_char(lexer, &c, &loc))
    {
        switch (c)
        {
            case '+':
                return nu__consume_single_char_token(
                    lexer, TOKEN_ADD, loc, token);
            case '-':
                nu__consume_char(lexer);
                if (nu__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '-')
                    {
                        nu__consume_char(lexer);
                        nu__consume_comment(lexer);
                    }
                    else
                    {
                        *token = NU_TOKEN_SINGLE(TOKEN_SUB, loc);
                        return NU_COMPERR_NONE;
                    }
                }
                else
                {
                    *token = NU_TOKEN_SINGLE(TOKEN_SUB, loc);
                    return NU_COMPERR_NONE;
                }
                break;
            case '*':
                return nu__consume_single_char_token(
                    lexer, TOKEN_MUL, loc, token);
            case '/':
                return nu__consume_single_char_token(
                    lexer, TOKEN_DIV, loc, token);
            case '(':
                return nu__consume_single_char_token(
                    lexer, TOKEN_LPAREN, loc, token);
            case ')':
                return nu__consume_single_char_token(
                    lexer, TOKEN_RPAREN, loc, token);
            case ']':
                return nu__consume_single_char_token(
                    lexer, TOKEN_LBRACKET, loc, token);
            case '[':
                return nu__consume_single_char_token(
                    lexer, TOKEN_RBRACKET, loc, token);
            case ',':
                return nu__consume_single_char_token(
                    lexer, TOKEN_COMMA, loc, token);
            case '.':
                return nu__consume_single_char_token(
                    lexer, TOKEN_DOT, loc, token);
            case ':':
                return nu__consume_single_char_token(
                    lexer, TOKEN_COLON, loc, token);
            case '=':
                nu__consume_char(lexer);
                if (nu__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nu__consume_char(lexer);
                        *token = NU_TOKEN_DOUBLE(TOKEN_EQUAL, loc);
                        return NU_COMPERR_NONE;
                    }
                }
                *token = NU_TOKEN_SINGLE(TOKEN_ASSIGN, loc);
                return NU_COMPERR_NONE;
            case '<':
                nu__consume_char(lexer);
                if (nu__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nu__consume_char(lexer);
                        *token = NU_TOKEN_DOUBLE(TOKEN_LEQUAL, loc);
                        return NU_COMPERR_NONE;
                    }
                }
                *token = NU_TOKEN_SINGLE(TOKEN_LESS, loc);
                return NU_COMPERR_NONE;
            case '>':
                nu__consume_char(lexer);
                if (nu__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nu__consume_char(lexer);
                        *token = NU_TOKEN_DOUBLE(TOKEN_GEQUAL, loc);
                        return NU_COMPERR_NONE;
                    }
                }
                *token = NU_TOKEN_SINGLE(TOKEN_GREATER, loc);
                return NU_COMPERR_NONE;
            case '!':
                nu__consume_char(lexer);
                if (nu__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nu__consume_char(lexer);
                        *token = NU_TOKEN_DOUBLE(TOKEN_NEQUAL, loc);
                        return NU_COMPERR_NONE;
                    }
                }
                return NU_COMPERR_ILLEGAL_CHARACTER;
            case ' ':
                nu__consume_spaces(lexer);
                break;
            case '\'':
                return nu__consume_string(lexer, token);
            default:
                if (NU_NUMERIC_CHAR(c))
                {
                    return nu__consume_number(lexer, token);
                }
                else if (NU_IDENTIFIER_CHAR(c))
                {
                    return nu__consume_identifier(lexer, token);
                }
                else
                {
                    nu__consume_char(lexer);
                }
                break;
        }
    }
    *token = NU_TOKEN_EOF;
    return NU_COMPERR_NONE;
}

static nu__compiler_error_t
nu__lexer_next (nu__lexer_t *lexer, nu__token_t *token)
{
    if (lexer->peek_count)
    {
        nu_size_t i = 0;
        *token      = lexer->peeks[0];
        for (i = 1; i < lexer->peek_count; ++i)
        {
            lexer->peeks[i - 1] = lexer->peeks[i];
        }
        return NU_COMPERR_NONE;
    }
    else
    {
        return nu__lexer_parse_token(lexer, token);
    }
}

static nu__compiler_error_t
nu__lexer_peek (nu__lexer_t *lexer, nu_size_t lookahead, nu__token_t *token)
{
    nu__compiler_error_t error;
    while (lexer->peek_count <= lookahead)
    {
        error = nu__lexer_parse_token(lexer, token);
        if (error != NU_COMPERR_NONE)
        {
            return error;
        }
        lexer->peeks[lexer->peek_count++] = *token;
    }
    *token = lexer->peeks[lookahead];
    return NU_COMPERR_NONE;
}

#endif

#endif
