#ifndef NU_LUA_H
#define NU_LUA_H

#ifdef NU_IMPLEMENTATION

#include <nucleus/math.h>

/* prevent use of address of label */
#define LUA_USE_JUMPTABLE 0
/* force use C89 mode */
#define LUA_USE_C89
/* use float instead of double */
#define LUA_32bits

#define LUA_IMPL
#include <externals/lua/minilua.h>

#endif

#endif
