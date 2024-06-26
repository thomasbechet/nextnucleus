#define NU_IMPL
#define NULANG_IMPL
#include <nucleus/lang.h>

static const nu_char_t *source
    = "let abc: $hello = 'hello world' + 4 + 3.0 * 1.0 / (3/2)\n"
      "if abc == 'cool' then abc = true end\n"
      "while abc > 0.12345 do\n"
      "let test: fix = 123\n"
      "let coucou = 0\n"
      "if (true == false) then print('hello') end\n"
      "aabc = 4\n"
      "end\n";

int
main (void)
{
    nulang_compiler_info_t info;
    nulang_compiler_t      compiler;
    nulang_error_t         error;
    nulang_source_print_tokens(source);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    nulang_compiler_info_default(&info);
    error = nulang_compiler_init(&info, &compiler);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    error = nulang_compiler_load(&compiler, source);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    nulang_compiler_print_symbols(&compiler);
    nulang_compiler_print_types(&compiler);
    nulang_compiler_print_ast(&compiler);
    return 0;
}
