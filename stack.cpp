#include "stack.h"

enum error_id StackInitF( stack_t* stk, size_t init_capacity
                ON_DEBUG(, const char* file, const char* func, int line))
{
    if (stk == NULL)
    {
        ON_DEBUG(StackAssert(stk);)
        return FATAL_ERROR_NO_STACK;
    }

    if (stk->alloc_ptr != NULL || stk->capacity > 0 || stk->size > 0)
    {
        ON_DEBUG(stk->status = ALREADY_INIT;
                 StackAssert(stk);)
        return ALREADY_INIT;
    }

    #ifdef STACK_DEBUG
    stk->file = file;
    stk->func = func;
    stk->line = line;
    #endif

    if (init_capacity > SIZE_MAX - EPS)         // <=> (init_capacity < 0)
    {                                           //size_t  is unsigned and always > 0,
        stk->capacity = init_capacity;          //so we need to check "infinite" value
        ON_DEBUG(stk->status = CAPACITY_ERROR;
                 StackAssert(stk);)
        return CAPACITY_ERROR;
    }
    stack_el_t* init_alloc_ptr = (stack_el_t*)calloc(init_capacity + 2, sizeof(stack_el_t));
    //                                                   ^
    //                                                   | 2 more elements for canaries
    if (init_alloc_ptr == NULL)
    {
        ON_DEBUG(stk->status = STACK_OOM;
                 StackAssert(stk);)
        return STACK_OOM;
    }

    stk->alloc_ptr = init_alloc_ptr;
    stk->data = init_alloc_ptr + 1;

    // |canary_left| |data[0]| .. |data[capacity - 1]| |canary_right|
    //  ^             ^
    //  | alloc_prt   | data

    stk->capacity = init_capacity;
    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = POISON;

    stk->alloc_ptr[0]                 =  LEFT_BUFFER_CANARY_CORRECT_VALUE;
    stk->alloc_ptr[stk->capacity + 1] = RIGHT_BUFFER_CANARY_CORRECT_VALUE;
    stk->left_stack_canary  =  LEFT_STACK_CANARY_CORRECT_VALUE;
    stk->right_stack_canary = RIGHT_STACK_CANARY_CORRECT_VALUE;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

enum error_id StackPush( stack_t* stk, stack_el_t value )
{
    enum error_id is_error = StackAssert(stk);
    if (is_error != ALL_CORRECT)
    {
        return is_error;
    }
    if (stk->size == stk->capacity)
    {
        size_t new_capacity = stk->capacity != 0 ? stk->capacity * 2 : 1; //for stk->capacity == 0

        stack_el_t* new_alloc_ptr = (stack_el_t*)realloc(stk->alloc_ptr, (new_capacity + 2) * sizeof(stack_el_t));
        //                                                                               ^
        //                                                  2 more elements for canaries |
        if (new_alloc_ptr == NULL)
        {
            ON_DEBUG(stk->status = STACK_OOM;
                     StackAssert(stk);)
            return STACK_OOM;
        }
        stk->alloc_ptr = new_alloc_ptr;
        stk->data = new_alloc_ptr + 1;
        stk->capacity = new_capacity;

        for (size_t i = stk->size; i < stk->capacity; i++)
            stk->data[i] = POISON;

        stk->data[stk->capacity] = RIGHT_BUFFER_CANARY_CORRECT_VALUE;   //left buffer canary was copied by realloc
    }

    stk->data[stk->size++] = value;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

enum error_id StackPop( stack_t* stk, stack_el_t* value )
{
    enum error_id is_error = StackAssert(stk);
    if (is_error != ALL_CORRECT)
        return is_error;

    if (stk->size == 0)
    {
        *value = POISON;
        ON_DEBUG(stk->status = STACK_UNDERFLOW;
                 StackAssert(stk);)
        return STACK_UNDERFLOW;
    }
    if (4 * stk->size < stk->capacity)
    {
        size_t new_capacity = stk->capacity / 2; // I think that new_capacity and new_alloc_ptr
                                                 // are not necessary, all just for beauty.
        stack_el_t* new_alloc_ptr = (stack_el_t*)realloc(stk->alloc_ptr, (new_capacity + 2) * sizeof(stack_el_t));
        //                                                                               ^
        //                                                 2 more elements for canaries  |
        if (new_alloc_ptr == NULL)
        {
            ON_DEBUG(stk->status = STACK_OOM;
                     StackAssert(stk);)
            return STACK_OOM;
        }

        stk->alloc_ptr = new_alloc_ptr;
        stk->data = new_alloc_ptr + 1;
        stk->capacity = new_capacity;

        stk->data[stk->capacity] = RIGHT_BUFFER_CANARY_CORRECT_VALUE;   //left buffer canary was copied by realloc
    }

    *value = stk->data[--stk->size];
    stk->data[stk->size] = POISON;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

enum error_id StackDestroy( stack_t* stk )
{
    enum error_id is_error = StackAssert(stk);
    if (is_error != ALL_CORRECT)
        return is_error;

    /*
    if (stk == NULL)
    {
        ON_DEBUG(StackAssert(stk);)
        return FATAL_ERROR_NO_STACK;
    }

    if (stk->alloc_ptr == NULL)
    {
        ON_DEBUG(stk->status = UNINIT_STACK;
                 StackAssert(stk);)
        return UNINIT_STACK;
    }
    */

    size_t need_to_clear = stk->capacity + 2;
    //                                     ^
    //        don't forget about canaries  |
    for (size_t i = 0; i < need_to_clear; i++)
    {
        stk->alloc_ptr[i] = POISON;
    }

    stk->capacity = 0;
    stk->size = 0;
    stk->left_stack_canary  = POISON;
    stk->right_stack_canary = POISON;

    free(stk->alloc_ptr);
    stk->data = NULL;
    stk->alloc_ptr = NULL;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

