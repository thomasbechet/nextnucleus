#ifndef NULANG_PRINT_H
#define NULANG_PRINT_H

#include <nucleus/lang/compiler.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/ast.h>
#include <nucleus/lang/token.h>

#ifdef NU_STDLIB

NU_API void nulang_print_tokens(const nu_char_t *source);
NU_API void nulang_print_symbols(const nulang_compiler_t *compiler);
NU_API void nulang_print_ast(const nulang_compiler_t *compiler);
NU_API void nulang_print_status(const nulang_compiler_t *compiler);

#endif

#ifdef NULANG_IMPL

#ifdef NU_STDLIB

#include <stdio.h>

static void
nulang__print_string (nulang__string_t s)
{
    printf("%.*s", (int)s.n, s.p);
}
static void
nulang__print_location (nulang_location_t loc)
{
    printf("%d:%d", loc.line, loc.column);
}
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
            printf("str('%.*s')", (int)lit->value.s.n, lit->value.s.p);
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
nulang__print_token (const nulang__token_t *tok)
{
    printf("%s ", NULANG_TOKEN_NAMES[tok->type]);
    nulang__print_location(tok->span.start);
    printf(" ");
    switch (tok->type)
    {
        case TOKEN_IDENTIFIER:
            nulang__print_string(tok->value.identifier);
            break;
        case TOKEN_ARCHETYPE:
            nulang__print_string(tok->value.identifier);
            break;
        case TOKEN_LITERAL:
            nulang__print_literal(&tok->value.literal);
            break;
        case TOKEN_PRIMITIVE:
            printf("%s ", NU_PRIMITIVE_NAMES[tok->value.primitive]);
        default:
            break;
    }
}
static nulang__error_t
nulang__lexer_print_tokens (nulang__lexer_t *lexer)
{
    nulang__token_t tok;
    nulang__error_t error;
    for (;;)
    {
        error = nulang__lexer_next(lexer, &tok);
        NULANG_ERROR_CHECK(error);
        nulang__print_token(&tok);
        printf("\n");
        if (tok.type == TOKEN_EOF)
        {
            break;
        }
    }
    return NULANG_ERROR_NONE;
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
nulang__print_symbol (const nulang__symbol_table_t *symbols,
                      nulang__symbol_id_t           symbol)
{
    const nulang__symbol_t *sym = &symbols->symbols[symbol];
    printf("'%.*s' type=%s",
           (int)sym->ident.n,
           sym->ident.p,
           NULANG_SYMBOL_NAMES[sym->type]);
    switch (symbols->symbols[symbol].type)
    {
        case SYMBOL_FUNCTION:
            break;
        case SYMBOL_CONSTANT:
            break;
        case SYMBOL_VARIABLE:
            printf(" block=%d", sym->block);
            switch (sym->value.variable.vartype.type)
            {
                case VARTYPE_ARCHETYPE:
                    printf(" vartype=ARCHETYPE(%d)",
                           sym->value.variable.vartype.value.archetype);
                    break;
                case VARTYPE_PRIMITIVE:
                    printf(" vartype=%s",
                           NU_PRIMITIVE_NAMES[sym->value.variable.vartype.value
                                                  .primitive]);
                    break;
                case VARTYPE_UNKNOWN:
                    printf(" vartype=UNKNOWN");
                    break;
            }
            if (sym->value.variable.vartype.type == VARTYPE_ARCHETYPE)
            {
                break;
            }
            break;
        case SYMBOL_ARCHETYPE:
            break;
        case SYMBOL_ARGUMENT:
            break;
        case SYMBOL_MODULE:
            break;
        case SYMBOL_EXTERNAL:
            break;
        case SYMBOL_UNKNOWN:
            break;
    }
}

void
nulang_print_status (const nulang_compiler_t *compiler)
{
    nulang__error_data_t data = compiler->error_data;
    if (compiler->error == NULANG_ERROR_NONE)
    {
        printf("SUCCESS\n");
        return;
    }
    printf("ERROR: ");
    switch (compiler->error)
    {
        case NULANG_ERROR_OUT_OF_NODE:
            printf("out of nodes (capacity: %d)", compiler->ast.node_capacity);
            break;
        case NULANG_ERROR_OUT_OF_SYMBOL:
            printf("out of symbols (capacity: %d)",
                   compiler->symbols.symbol_capacity);
            break;
        case NULANG_ERROR_OUT_OF_BLOCK:
            printf("out of blocks (capacity: %d)",
                   compiler->symbols.block_capacity);
            break;
        case NULANG_ERROR_OUT_OF_MEMORY:
            printf("out of memory");
            break;

        case NULANG_ERROR_ILLEGAL_CHARACTER:
            printf("illegal character");
            break;
        case NULANG_ERROR_UNTERMINATED_STRING:
            printf("unterminated string");
            break;
        case NULANG_ERROR_UNEXPECTED_TOKEN:
            printf("unexpected token (expect: %s, got: %s)",
                   NULANG_TOKEN_NAMES[data.token_expect],
                   NULANG_TOKEN_NAMES[data.token_got]);
            break;

        case NULANG_ERROR_SYMBOL_ALREADY_DEFINED:
            printf("symbol already defined");
            break;
        case NULANG_ERROR_INVALID_ATOM_EXPRESSION:
            printf("invalid atom expression (got: %s)",
                   NULANG_TOKEN_NAMES[data.token_got]);
            break;
        case NULANG_ERROR_UNEXPECTED_BINOP:
            printf("unpexected binop");
            break;
        case NULANG_ERROR_NON_STATEMENT_TOKEN:
            printf("non statement token");
            break;
        case NULANG_ERROR_IDENTIFIER_AS_STATEMENT:
            printf("identifier as statement");
            break;
        case NULANG_ERROR_INVALID_VARTYPE:
            printf("invalid vartype");
            break;

        case NULANG_ERROR_INCOMPATIBLE_TYPE:
            printf("incompatible type");

        case NULANG_ERROR_NONE:
            break;
    }
    printf(" at line %d column %d\n",
           data.span.start.line,
           data.span.start.column);
    {
        nu_u32_t start, stop, n;
        start = stop = compiler->error_data.span.start.index;
        while (start)
        {
            if (*(compiler->source + start) == '\n')
            {
                start++;
                break;
            }
            start--;
        }
        while (*(compiler->source + stop) && *(compiler->source + stop) != '\n')
        {
            stop++;
        }
        printf("ERROR: ");
        printf("%.*s\n", (nu_u32_t)(stop - start), compiler->source + start);
        printf("ERROR: ");
        n = compiler->error_data.span.start.index - start;
        while (n--)
        {
            printf(" ");
        }
        printf("^\n");
    }
}

static void
nulang__print_symbol_table (const nulang__symbol_table_t *table)
{
    nu_size_t i;
    for (i = 0; i < table->symbol_count; ++i)
    {
        printf("[%ld] ", i);
        nulang__print_symbol(table, i);
        printf("\n");
    }
}
static void
nulang__print_node (const nulang__symbol_table_t *symbols,
                    const nulang__ast_t          *ast,
                    nu_u16_t                      depth,
                    nulang__node_id_t             id)
{
    nulang__node_t *node = &ast->nodes[id];
    nulang__print_depth(depth);
    printf("%s ", NULANG_AST_NAMES[node->type]);
    switch (node->type)
    {
        case AST_VARDECL:
            printf("symbol(%d) ", node->value.symbol);
            break;
        case AST_SYMBOL:
            printf("symbol(%d) ", node->value.symbol);
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
        case AST_FIELDLOOKUP:
            nulang__print_string(node->value.fieldlookup);
        default:
            break;
    }
    printf("\n");
    if (node->first_child != NULANG_NODE_NULL)
    {
        nulang__print_node(symbols, ast, depth + 1, node->first_child);
    }
    if (node->next_sibling != NULANG_NODE_NULL)
    {
        nulang__print_node(symbols, ast, depth, node->next_sibling);
    }
}

void
nulang_print_tokens (const nu_char_t *source)
{
    nulang__lexer_t lexer;
    nulang__lexer_init(source, &lexer);
    printf("==== TOKENS ====\n");
    nulang__lexer_print_tokens(&lexer);
}
void
nulang_print_symbols (const nulang_compiler_t *compiler)
{
    printf("==== SYMBOLS ====\n");
    nulang__print_symbol_table(&compiler->symbols);
}
void
nulang_print_ast (const nulang_compiler_t *compiler)
{
    printf("==== NODES ====\n");
    nulang__print_node(
        &compiler->symbols, &compiler->ast, 0, compiler->ast.root);
}

#endif

#endif

#endif
