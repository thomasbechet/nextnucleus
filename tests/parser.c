#include <stdio.h>
#include <stdlib.h>
#define NU_IMPLEMENTATION
#define NULANG_IMPLEMENTATION
#include <nucleus/lang.h>

static const nu_char_t *source
    = "local abc: fix = 'hello world' + 4 + 3.0 * 1.0 / (3/2)\n"
      "if abc == 'cool' then abc = true end\n"
      "while abc > 0.12345 do\n"
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
    alloc_info.callback = custom_alloc;
    alloc_info.userdata = NU_NULL;
    error = nulang_compiler_init(256, 256, 256, &alloc_info, &compiler);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    error = nulang_compiler_evaluate(&compiler, source);
    NU_ASSERT(error == NULANG_ERROR_NONE);
    nulang_compiler_print(&compiler);
    return 0;
}
