#ifndef NU_INTERPRETER_H
#define NU_INTERPRETER_H

#include <nucleus/types.h>
#include <nucleus/math.h>

typedef enum
{
    OP_ADD  = 0,
    OP_SUB  = 1,
    OP_MULI = 2,
    OP_MULF = 3,
    OP_DIVI = 4,
    OP_DIVF = 5
} nu_opcode_t;

#ifdef NU_IMPLEMENTATION

#define NU_STACK_SIZE 256

typedef nu_u32_t nu__word_t;
typedef nu_u16_t nu__instruction_t;

#define NU_GET_OPCODE(i) (i & 0xff)

typedef struct
{
    nu_u32_t           sp; /* stack pointer */
    nu_u32_t           lp; /* locals pointer */
    nu_u32_t           pc; /* program counter */
    nu__word_t         stack[NU_STACK_SIZE];
    nu__word_t         local[NU_STACK_SIZE];
    nu__instruction_t *instructions;
    nu__word_t        *constants;
} nu__interpreter_t;

static void
nu__interpreter_init (nu__interpreter_t *inte)
{
    nu_size_t i;
    inte->sp = 0;
    inte->lp = 0;
    inte->pc = 0;
    for (i = 0; i < NU_STACK_SIZE; ++i)
    {
        inte->stack[i] = 0;
        inte->local[i] = 0;
    }
    inte->instructions = NU_NULL;
    inte->constants    = NU_NULL;
}

static void
nu__interpreter_exec (nu__interpreter_t *inte)
{
    nu_i32_t          a, b;
    nu__instruction_t instruction = inte->instructions[inte->pc++];
    switch (NU_GET_OPCODE(instruction))
    {
        case OP_ADD:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a + b;
            break;
        case OP_SUB:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a - b;
            break;
        case OP_MULI:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a * b;
            break;
        case OP_MULF:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = nu_fmul(a, b);
            break;
        case OP_DIVI:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a / b;
            break;
        case OP_DIVF:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = nu_fdiv(a, b);
            break;
        default:
            break;
    }
}

#endif

#endif
