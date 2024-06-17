#include <stdio.h>
#define NU_IMPLEMENTATION
#include <nucleus/compiler.h>

static const nu_char_t *source
    = "local abc = 0\n"
      "if (abc == 2) then\n"
      "   abc = 3\n"
      "end";

int
main (void)
{
    nu__lexer_t          lexer;
    nu_char_t            c;
    nu_source_location_t loc;
    nu_bool_t            has;
    nu__token_t          token;
    nu_lexer_error_t     error;

    nu__lexer_init(source, &lexer);

    for (;;)
    {
        error = nu__lexer_next(&lexer, &token);
        NU_ASSERT(error == NU_LEXERR_NONE);
        if (token.type == TOKEN_EOF)
        {
            break;
        }
        printf("%s %d:%d %d:%d\n",
               NU_TOKEN_NAMES[token.type],
               token.span.start.line,
               token.span.start.column,
               token.span.stop.line,
               token.span.stop.column);
        if (token.type == TOKEN_LITERAL)
        {
            printf("%d\n", token.value.literal.value.i);
        }
        if (token.type == TOKEN_IDENTIFIER)
        {
            printf("%.*s\n",
                   (int)token.value.literal.value.s.n,
                   token.value.literal.value.s.p);
        }
    }

    return 0;
}
