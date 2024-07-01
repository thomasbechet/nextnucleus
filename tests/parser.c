#define NU_IMPL
#define NULANG_IMPL
#include <nucleus/lang.h>

/* static const nu_char_t *source
    = "let abc: $hello = 'hello world' + 4 + 3.0 * 1.0 / (3/2)\n"
      "abc = 1.0\n"
      "if abc == 'cool' then abc = true end\n"
      "while abc > 0.12345 do\n"
      "let test: fix = 123\n"
      "let coucou = 0\n"
      "if (true == false) then print('hello') end\n"
      "aabc = 4\n"
      "$player.position = 3\n"
      "print($player)\n"
      "end\n" */

static nu_char_t *
load_file (const nu_char_t *file)
{
    nu_char_t *buffer = NULL;
    FILE      *f      = fopen(file, "rb");
    long       length;
    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        buffer[length] = '\0';
        fclose(f);
    }
    return buffer;
}

int
main (int argc, char *argv[])
{
    nulang_status_t        status;
    nulang_compiler_info_t info;
    nulang_compiler_t      compiler;
    nu_char_t             *source;

    NU_ASSERT(argc == 2);
    source = load_file(argv[1]);
    NU_ASSERT(source);

    /* nulang_print_tokens(source); */
    nulang_compiler_info_default(&info);
    status = nulang_compiler_init(&info, &compiler);
    NU_ASSERT(status == NULANG_SUCCESS);
    status = nulang_compile(&compiler, source);
    nulang_print_status(&compiler);
    NU_ASSERT(status == NULANG_SUCCESS);
    nulang_print_symbols(&compiler);
    nulang_print_ast(&compiler);

    nulang_compiler_free(&compiler);
    return 0;
}
