#ifndef _STACK_H_
#define _STACK_H_

#define STACK_DEBUG

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef double stack_el_t;
#define OUTPUT_SPECIFIER "%lg"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RETURN_COL "\033[0m"

#define POISON NAN

#ifdef STACK_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

#define MENU BLUE                                 \
            "menu:\n"                             \
            "1 - init stack sizeof\n"             \
            "2 - push to stack\n"                 \
            "3 - pop from stack\n"                \
            "4 - print stack\n"                   \
            "q - exit program\n"                  \
            RETURN_COL


struct stack_t
{
    ON_DEBUG(const char* file;
             const char* func;
             int line;)

    stack_el_t* data;
    size_t size;
    size_t capacity;
};
#define STACKINIT(PTR, SIZE) StackInit(PTR, SIZE ON_DEBUG(, __FILE__, __func__, __LINE__))
int StackInit( stack_t* stk, size_t capacity
               ON_DEBUG(, const char* file, const char* func, int line));

int StackPrint( stack_t* stk, FILE* stream );
int StackPush( stack_t* stk, stack_el_t value );
stack_el_t StackPop( stack_t* stk );
int StackDestroy( stack_t* stk );

#endif
/*
enum ERRORS = {UNINIT_STACK = -1, ALREADY_INIT_STACK = -2};
enum FUNCTIONS = {INIT = 1, PUSH = 2, POP = 3, DESTROY = 4};


enum ERRORS StackVerifier( stack_t* stk, enum FUNC func );


*/
