#ifndef _STACK_H_
#define _STACK_H_

//#define STACK_DEBUG

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//#include <windows.h>


#ifdef STACK_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#define CANARY_DEFENSE
#define HASH_DEFENSE
#else
#define ON_DEBUG(...)
#endif

#ifdef CANARY_DEFENSE
#define CANARY_ON(...) __VA_ARGS__
#else
#define CANARY_ON(...)
#endif

#ifdef HASH_DEFENSE
#define HASH_ON(...) __VA_ARGS__
#else
#define HASH_ON(...)
#endif




typedef int stack_el_t;
#define OUTPUT_SPECIFIER "%d"
#define POISON -67


#define ERROR_FILE_NAME "STACK_ERRORS.log"

#define RED         "\033[31m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define RET_COL  "\033[0m"

#ifdef CANARY_DEFENSE
const unsigned long long LEFT_STACK_CANARY_CORRECT_VALUE = 0xDEADBEEF;
const unsigned long long RIGHT_STACK_CANARY_CORRECT_VALUE = 0xFEEDDED;
const stack_el_t LEFT_BUFFER_CANARY_CORRECT_VALUE= 0xEDADEDA;
const stack_el_t RIGHT_BUFFER_CANARY_CORRECT_VALUE= 0xBEDADEDA;
#endif


enum error_id {ALL_CORRECT = 0,
               UNINIT_STACK = -1,
               STACK_OOM = -2,
               CAPACITY_ERROR = -3,
               SIZE_ERROR = -4,
               ALREADY_INIT = -5,
               FATAL_ERROR_NO_STACK = -6,
               CANARY_ON(DEAD_CANARY = -7,)
               STACK_UNDERFLOW = -8,
               HASH_ON(BAD_HASH = -9,)
               CANT_INIT = -10,
               INCORRECT_COPY = -11};

struct stack_t
{
    CANARY_ON(unsigned long long left_stack_canary;)

    stack_el_t* alloc_ptr;
    stack_el_t* data;
    size_t size;
    size_t capacity;

    HASH_ON(uint64_t hash_buffer_value;)
    HASH_ON(uint64_t hash_stack_value;)

    stack_t* this_ptr;

    ON_DEBUG(const char* file;
             const char* func;
             int line;)

    error_id status;

    CANARY_ON(unsigned long long right_stack_canary;)
};

error_id StackInitF( stack_t* stk, size_t capacity
           ON_DEBUG(, const char* file, const char* func, int line) );
#define StackInit(PTR, SIZE) StackInitF(PTR, SIZE ON_DEBUG(, __FILE__, __func__, __LINE__))
error_id StackInitByCopyF( stack_t* src, stack_t* dest
            ON_DEBUG(, const char* file, const char* func, int line) );
#define StackInitByCopy(SRC, DEST) StackInitByCopyF(SRC, DEST ON_DEBUG(, __FILE__, __func__, __LINE__))
error_id StackPush( stack_t* stk, stack_el_t value );
error_id StackPop( stack_t* stk, stack_el_t* value );
error_id StackDtor( stack_t* stk );

error_id StackInitChecks( stack_t* stk, size_t init_capacity );
error_id ResizeStack( stack_t* stk, size_t new_capacity, size_t el_size );
void DestroySecurity( stack_t* stk );
void DestroyStackStruct( stack_t* stk );



#ifdef CANARY_DEFENSE
void InitCanaries( stack_t* stk );
bool AreCanariesDead( const stack_t* stk );
#endif

#ifdef HASH_DEFENSE
uint64_t Hash_djb2( const void* beg, const void* end, const void* except, size_t except_el_size );
void RecalcHash( stack_t* stk );
#endif


#define fprintf_with_tabs(TABS, STREAM, ...) do                \
                            {                                  \
                                FprintNTabs(STREAM, TABS);     \
                                fprintf(STREAM, __VA_ARGS__);  \
                            } while(0);
void StackPrint( const stack_t* stk, FILE* stream );
void PrintStackHead( const stack_t* stk, FILE* stream, size_t* tabs );
void PrintStackBody( const stack_t* stk, FILE* stream, size_t* tabs );
void PrintStackTail( const stack_t* stk, FILE* stream, size_t* tabs );





error_id StackVerifier( stack_t* stk );
#define StackAssert(STK) StackAssertF( STK ON_DEBUG(, __FILE__, __func__, __LINE__) )
error_id StackAssertF( stack_t* stk
             ON_DEBUG(, const char* file, const char* func, int line));
#define StackDump(STK, ERR) StackDumpF( STK, ERR ON_DEBUG(, __FILE__, __func__, __LINE__) )
void StackDumpF( stack_t* stk,  error_id error
      ON_DEBUG(, const char* file, const char* func, int line ));


void fopen_bracket( size_t* tabs, FILE* stream );
void fclose_bracket( size_t* tabs, FILE* stream );
void FprintNTabs( FILE* stream, size_t tabs );


void StackUnitTest();

void ALL_CORRECT_Stack();
void ALREADY_INIT_Stack();
void UNINIT_Stack();
void INCORRECT_COPY_Stack();
void CAPACITY_ERROR_Stack();
void SIZE_ERROR_Stack();
void OOM_Stack();
void UNDERFLOW_Stack();
void DEAD_CANARY_Stack();
void BAD_HASH_Stack();




#endif
