#ifndef NU_PARSER_H
#define NU_PARSER_H

#include <nucleus/compiler/lexer.h>

#ifdef NU_IMPLEMENTATION

#define NU_FOREACH_BINOP(BINOP) \
    BINOP(BINOP_ADD)            \
    BINOP(BINOP_SUB)            \
    BINOP(BINOP_MUL)            \
    BINOP(BINOP_DIV)            \
    BINOP(BINOP_EQ)             \
    BINOP(BINOP_NEQ)            \
    BINOP(BINOP_LEQ)            \
    BINOP(BINOP_GEQ)            \
    BINOP(BINOP_LESS)           \
    BINOP(BINOP_GREATER)        \
    BINOP(BINOP_AND)            \
    BINOP(BINOP_NOT)

typedef enum
{
    NU_FOREACH_BINOP(NU_GENERATE_ENUM)
} nu__binop_type_t;
static const nu_char_t *NU_BINOP_NAMES[]
    = { NU_FOREACH_BINOP(NU_GENERATE_NAME) };

#define NU_FOREACH_UNOP(UNOP) \
    UNOP(UNOP_MINUS)          \
    UNOP(UNOP_NOT)
typedef enum
{
    NU_FOREACH_UNOP(NU_GENERATE_ENUM)
} nu__unop_type_t;
static const nu_char_t *NU_UNOP_NAMES[] = { NU_FOREACH_UNOP(NU_GENERATE_NAME) };

#define NU_FOREACH_AST(AST) \
    AST(AST_PROGRAM)        \
    AST(AST_COMPOUND)       \
    AST(AST_LITERAL)        \
    AST(AST_IDENTIFIER)     \
    AST(AST_PRIMITIVE)      \
    AST(AST_FIELDLOOKUP)    \
    AST(AST_BREAK)          \
    AST(AST_CONTINUE)       \
    AST(AST_RETURN)         \
    AST(AST_IF)             \
    AST(AST_IFBODY)         \
    AST(AST_FOR)            \
    AST(AST_WHILE)          \
    AST(AST_LOOP)           \
    AST(AST_VARDECL)        \
    AST(AST_CONSTDECL)      \
    AST(AST_CALL)           \
    AST(AST_ASSIGN)         \
    AST(AST_BINOP)          \
    AST(AST_UNOP)
typedef enum
{
    NU_FOREACH_AST(NU_GENERATE_ENUM)
} nu__ast_type_t;
static const nu_char_t *NU_AST_NAMES[] = { NU_FOREACH_AST(NU_GENERATE_NAME) };

typedef union
{
    nu__lit_value_t  literal;
    nu__binop_type_t binop;
    nu__unop_type_t  unop;
    nu_type_t        primitive;
} nu__ast_value_t;

typedef nu_u32_t nu__ast_node_id_t;

typedef struct
{
    nu__ast_type_t    type;
    nu__ast_value_t   value;
    nu__source_span_t span;
    nu__ast_node_id_t parent;
    nu__ast_node_id_t first_child;
    nu__ast_node_id_t last_child;
    nu__ast_node_id_t next_sibling;
} nu__ast_node_t;

typedef struct
{
    nu__ast_node_id_t root;
    nu__ast_node_t   *nodes;
} nu__ast_t;

#endif

#endif
