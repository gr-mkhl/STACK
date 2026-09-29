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

#define STACK_OK(STK_POINTER, FUNC) do                                                                                  \
                                    {                                                                                   \
                                        enum ERRORS is_error = StackVerifier(STK_POINTER, FUNC);                        \
                                        if (is_error != ALL_CORRECT)                                                    \
                                        {                                                                               \
                                            StackDump(STK_POINTER, is_error ON_DEBUG(, __FILE__, __func__, __LINE__));  \
                                            return is_error;                                                            \
                                        }                                                                               \
                                    } while(0);

enum ERRORS {ALL_CORRECT = 0,
             UNINIT_STACK = -1,
             STACK_OOM = -2,
             CAPACITY_ERROR = -3,
             SIZE_ERROR = -4,
             STACK_OVERFLOW = -5,
             STACK_UNDERFLOW = -6,
             ALREADY_INIT_STACK = -7,
             ALREADY_DESTROYED_STACK = -8,
             FATAL_ERROR_NO_STACK = -9};

enum FUNC {INIT = 1,
           PUSH = 2,
           POP = 3,
           DESTROY = 4,
           IS_OK = 0};


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
enum ERRORS StackInit( stack_t* stk, size_t capacity
               ON_DEBUG(, const char* file, const char* func, int line));
void StackDump( const stack_t* stk, enum ERRORS error
                ON_DEBUG(, const char* file, const char* func, int line));
enum ERRORS StackPrint( const stack_t* stk, FILE* stream );
enum ERRORS StackPush( stack_t* stk, stack_el_t value );
enum ERRORS StackPop( stack_t* stk, stack_el_t* value );
enum ERRORS StackDestroy( stack_t* stk );
enum ERRORS StackVerifier( const stack_t* stk, enum FUNC func );
#endif



