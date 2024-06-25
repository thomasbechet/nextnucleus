#include <stdio.h>
#include <stdlib.h>
#define NU_IMPL
#define NULANG_IMPL
#include <nucleus/lang.h>

static const nu_char_t *source
    = "let abc: $hello = 'hello world' + 4 + 3.0 * 1.0 / (3/2)\n"
      "if abc == 'cool' then abc = true end\n"
      "while abc > 0.12345 do\n"
      "let coucou = 0\n"
      "if (true == false) then print('hello') end\n"
      "aabc = 4\n"
      "end\n";
static void *
custom_alloc (nu_size_t s, void *userdata)
{
    (void)userdata;
    return malloc(s);
}

int
main (void)
{
    nulang_compiler_t       compiler;
    nulang_allocator_info_t alloc_info;
    nulang_error_t          error;
    nulang_compiler_print_tokens(source);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    alloc_info.callback = custom_alloc;
    alloc_info.userdata = NU_NULL;
    error = nulang_compiler_init(256, 256, 256, 256, &alloc_info, &compiler);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    error = nulang_compiler_evaluate(&compiler, source);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    nulang_compiler_print_symtab(&compiler);
    nulang_compiler_print_ast(&compiler);
    return 0;
}
