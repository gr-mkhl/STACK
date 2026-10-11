#ifndef _STACK_H_
#define _STACK_H_

//#define STACK_DEBUG
//#define CANARY_DEFENSE
//#define HASH_DEFENSE

#include "stack_struct.h"

#include <stdio.h>
#include <stdlib.h>

#define RED      "\033[31m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define RET_COL  "\033[0m"



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


#endif
