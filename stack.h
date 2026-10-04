#ifndef _STACK_H_
#define _STACK_H_

#define STACK_DEBUG

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <math.h>


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
#define RET_COL  "\033[0m"

#define EPS (SIZE_MAX / 10)

const unsigned long long LEFT_STACK_CANARY_CORRECT_VALUE = 0xDEADBEEF;
const unsigned long long RIGHT_STACK_CANARY_CORRECT_VALUE = 0xDEDADED;
const stack_el_t LEFT_BUFFER_CANARY_CORRECT_VALUE= 0xE1DADEDA;
const stack_el_t RIGHT_BUFFER_CANARY_CORRECT_VALUE= 0xBEDADEDA;

#ifdef STACK_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

enum error_id {ALL_CORRECT = 0,
               UNINIT_STACK = -1,
               STACK_OOM = -2,
               CAPACITY_ERROR = -3,
               SIZE_ERROR = -4,
               ALREADY_INIT = -5,
               FATAL_ERROR_NO_STACK = -6,
               DEAD_CANARY = -7,
               STACK_UNDERFLOW = -8,
               BAD_HASH = -9,
               CANT_INIT = -10};

struct stack_t
{
    unsigned long long left_stack_canary;

    stack_el_t* alloc_ptr;
    stack_el_t* data;
    ssize_t size;
    ssize_t capacity;

    uint64_t hash_buffer_value;
    uint64_t hash_stack_value;


    ON_DEBUG(const char* file;
             const char* func;
             int line;)
    error_id status;
    unsigned long long right_stack_canary;
};

#define StackInit(PTR, SIZE) StackInitF(PTR, SIZE ON_DEBUG(, __FILE__, __func__, __LINE__))

 error_id StackInitF( stack_t* stk, ssize_t capacity
                ON_DEBUG(, const char* file, const char* func, int line));

 error_id StackPush( stack_t* stk, stack_el_t value );
 error_id StackPop( stack_t* stk, stack_el_t* value );
 error_id StackDtor( stack_t* stk );

error_id StackInitChecks( stack_t* stk, ssize_t init_capacity );
void InitCanaries( stack_t* stk );
void DestroySecurity( stack_t* stk );
void DestroyStackStruct( stack_t* stk );
error_id ResizeStack( stack_t* stk, ssize_t new_capacity, size_t el_size );


#define fprintf_with_tabs(TABS, STREAM, ...) do                \
                            {                                  \
                                FprintNTabs(STREAM, TABS);     \
                                fprintf(STREAM, __VA_ARGS__);  \
                            } while(0);

void StackPrint( const stack_t* stk, FILE* stream );
void PrintStackHead( const stack_t* stk, FILE* stream, size_t* tabs );
void PrintStackBody( const stack_t* stk, FILE* stream, size_t* tabs );
void PrintStackTail( const stack_t* stk, FILE* stream, size_t* tabs );

#define StackAssert(STK) StackAssertF( STK ON_DEBUG(, __FILE__, __func__, __LINE__) )
 error_id StackAssertF( stack_t* stk
              ON_DEBUG(, const char* file, const char* func, int line));

void StackDump( stack_t* stk,  error_id error
      ON_DEBUG(, const char* file, const char* func, int line ));

error_id StackVerifier( stack_t* stk );

uint64_t Hash_djb2( const void* beg, const void* end, const void* except, size_t except_el_size );
void RecalcHash( stack_t* stk );

int AreNotCanariesAlive( const stack_t* stk );

void fopen_bracket( size_t* tabs, FILE* stream );
void fclose_bracket( size_t* tabs, FILE* stream );
void FprintNTabs( FILE* stream, size_t tabs );


void StackUnitTest();

void ALL_CORRECT_Stack();
void ALREADY_INIT_Stack();
void UNINIT_Stack();
void CAPACITY_ERROR_Stack();
void SIZE_ERROR_Stack();
void OOM_Stack();
void UNDERFLOW_Stack();
void DEAD_CANARY_Stack();
void BAD_HASH_Stack();




#endif
