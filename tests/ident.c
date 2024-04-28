#include <assert.h>
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

int main(void)
{
    nu_ident_t ident;

    nu_ident_empty(ident);
    assert(nu_ident_len(ident) == 0);
    assert(nu_strncmp(nu_ident_str(ident), "", NU_IDENT_MAX) == 0);
    nu_ident_set_str(ident, "foo");
    assert(nu_ident_len(ident) == 3);
    assert(nu_strncmp(nu_ident_str(ident), "foo", NU_IDENT_MAX) == 0);
    nu_ident_set_str(ident, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    assert(nu_ident_len(ident) == NU_IDENT_MAX);
    assert(nu_strncmp(nu_ident_str(ident), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", NU_IDENT_MAX) == 0);

    return 0;
}
