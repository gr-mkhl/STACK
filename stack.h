#ifndef _STACK_H_
#define _STACK_H_

#define STACK_DEBUG

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef int stack_el_t;
#define OUTPUT_SPECIFIER "%d"
#define POISON -67
#define ERROR_FILE_NAME "STACK_ERRORS.txt"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define PURPLE      "\033[35m"
#define LIGHT_BLUE  "\033[36m"
#define WHITE       "\033[37m"
#define RETURN_COL  "\033[0m"


const unsigned long long LEFT_STACK_CANARY_CORRECT_VALUE = 0xAB0BA;
const unsigned long long RIGHT_STACK_CANARY_CORRECT_VALUE = 0xBA0BAB;

const stack_el_t LEFT_BUFFER_CANARY_CORRECT_VALUE= 0xEDA;
const stack_el_t RIGHT_BUFFER_CANARY_CORRECT_VALUE= 0xDED;

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


enum error_id {ALL_CORRECT = 0,
               UNINIT_STACK = -1,
               STACK_OOM = -2,
               CAPACITY_ERROR = -3,
               SIZE_ERROR = -4,
               ALREADY_INIT_STACK = -5,
               ALREADY_DESTROYED_STACK = -6,
               FATAL_ERROR_NO_STACK = -7,
               DIED_CANARY = -8,
               STACK_UNDERFLOW = -9};

struct stack_t
{
    unsigned long long left_stack_canary;
    stack_el_t* alloc_ptr;
    stack_el_t* data;

    size_t size;
    size_t capacity;


    ON_DEBUG(const char* file;
             const char* func;
             int line;
             enum error_id status;)

    unsigned long long right_stack_canary;
};
#define STACKINIT(PTR, SIZE) StackInit(PTR, SIZE ON_DEBUG(, __FILE__, __func__, __LINE__))
enum error_id StackInit( stack_t* stk, size_t capacity
               ON_DEBUG(, const char* file, const char* func, int line));
enum error_id StackPrint( const stack_t* stk, FILE* stream );
enum error_id StackPush( stack_t* stk, stack_el_t value );
enum error_id StackPop( stack_t* stk, stack_el_t* value );
enum error_id StackDestroy( stack_t* stk );

enum error_id StackOk( stack_t* stk );

void StackDump( const stack_t* stk, enum error_id error
                ON_DEBUG(, const char* file, const char* func, int line));

enum error_id StackVerifier( const stack_t* stk );

int AreNotCanariesAlive( const stack_t* stk );

#endif



