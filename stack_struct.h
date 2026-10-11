#ifndef _STACK_STRUCT_H_
#define _STACK_STRUCT_H_


#ifdef __win32__
    #define WINDOWS_SYS
#endif

#ifdef STACK_DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
    #ifndef CANARY_DEFENSE
        #define CANARY_DEFENSE
    #endif
    #ifndef HASH_DEFENSE
        #define HASH_DEFENSE
    #endif
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

enum error_id
{
    ALL_CORRECT = 0,
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
    INCORRECT_COPY = -11,
    INVALID_POINTER = -12
};


#include <stddef.h>
#include <stdint.h>


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

#endif
