#include "stack.h"


error_id StackInitF( stack_t* stk, ssize_t init_capacity
                ON_DEBUG(, const char* file, const char* func, int line) )
{
    error_id error = StackInitChecks(stk, init_capacity);
    if (error != ALL_CORRECT)
        return error;

    #ifdef STACK_DEBUG
        stk->file = file;
        stk->func = func;
        stk->line = line;
    #endif

    stk->size = 0;

    if (ResizeStack(stk, init_capacity, sizeof(stack_el_t)) != ALL_CORRECT)
        return CANT_INIT;

    stk->status = ALL_CORRECT;

    stk->this_ptr = stk;

    #ifdef CANARY_DEFENSE
    InitCanaries(stk);
    #endif

    #ifdef HASH_DEFENSE
    RecalcHash(stk);
    #endif

    return ALL_CORRECT;
}

error_id StackInitByCopyF( stack_t* src, stack_t* dest
                 ON_DEBUG(, const char* file, const char* func, int line) )
{
    error_id error_src = StackAssert(src);
    if (error_src != ALL_CORRECT)
        return error_src;

    error_id error_dest = StackInitChecks(dest, src->capacity);
    if (error_dest != ALL_CORRECT)
        return error_dest;

    #ifdef STACK_DEBUG
        dest->file = file;
        dest->func = func;
        dest->line = line;
    #endif

    dest->size = 0;

    if (ResizeStack(dest, src->capacity, sizeof(stack_el_t)) != ALL_CORRECT)
        return CANT_INIT;

    for (ssize_t i = 0; i < src->size; i++)
        dest->data[i] = src->data[i];

    dest->status = ALL_CORRECT;

    dest->this_ptr = dest;

    #ifdef CANARY_DEFENSE
    InitCanaries(dest);
    #endif

    #ifdef HASH_DEFENSE
    RecalcHash(dest);
    #endif

    return ALL_CORRECT;
}


error_id StackPush( stack_t* stk, stack_el_t value )
{
    error_id error = StackAssert(stk);
    if (error != ALL_CORRECT)
        return error;

    if (stk->size == stk->capacity)
    {
        size_t new_capacity = (stk->capacity != 0 ? stk->capacity * 2 : 16); //for stk->capacity == 0, 16 is just a magic number

        if (ResizeStack(stk, new_capacity, sizeof(stack_el_t)) == STACK_OOM)
            return STACK_OOM;
    }

    stk->data[stk->size++] = value;


    #ifdef HASH_DEFENSE
    RecalcHash(stk);
    #endif

    return ALL_CORRECT;
}

error_id StackPop( stack_t* stk, stack_el_t* value )
{
    error_id error = StackAssert(stk);
    if (error != ALL_CORRECT)
        return error;

    if (stk->size == 0)
    {
        *value = POISON;
        stk->status = STACK_UNDERFLOW;
        StackAssert(stk);
        return STACK_UNDERFLOW;
    }

    if (4 * stk->size < stk->capacity)
    {
        ssize_t new_capacity = stk->capacity / 2;

        if (ResizeStack(stk, new_capacity, sizeof(stack_el_t)) == STACK_OOM)
            return STACK_OOM;
    }

    *value = stk->data[--stk->size];
    stk->data[stk->size] = POISON;

    #ifdef HASH_DEFENSE
    RecalcHash(stk);
    #endif

    return ALL_CORRECT;
}


error_id StackDtor( stack_t* stk )
{
    error_id error = StackAssert(stk);
    if (error != ALL_CORRECT)
        return error;

    DestroySecurity(stk);

    DestroyStackStruct(stk);

    return ALL_CORRECT;
}


void DestroyStackStruct( stack_t* stk )
{
    for (ssize_t i = 0; i < stk->capacity; i++)
        stk->data[i] = POISON;

    free(stk->alloc_ptr);
    stk->capacity = 0;
    stk->size = 0;
    stk->data = NULL;
    stk->alloc_ptr = NULL;

    return;
}

void DestroySecurity( stack_t* stk )
{
    #ifdef CANARY_DEFENSE
    stk->left_stack_canary = 0;
    stk->right_stack_canary = 0;
    stk->alloc_ptr[0] = POISON;
    stk->alloc_ptr[stk->capacity + 1] = POISON;
    #endif

    #ifdef HASH_DEFENSE
    stk->hash_buffer_value = 0;
    stk->hash_stack_value = 0;
    #endif

    return;
}

error_id ResizeStack( stack_t* stk, ssize_t new_capacity, size_t el_size )
{
    #ifdef CANARY_DEFENSE
    stack_el_t* new_alloc_ptr = (stack_el_t*)realloc(stk->alloc_ptr, (new_capacity + 2) * el_size);
    //                                                                               ^
    //                                                 2 more elements for canaries  |
    #else
    stack_el_t* new_alloc_ptr = (stack_el_t*)realloc(stk->alloc_ptr, new_capacity * el_size);
    #endif

    if (new_alloc_ptr == NULL)
    {
        if (stk->capacity == 0)
        {
            stk->status = CANT_INIT;
            StackAssert(stk);
            return CANT_INIT;
        }
        else
        {
            stk->status = STACK_OOM;
            StackAssert(stk);
            return STACK_OOM;
        }
    }
    stk->alloc_ptr = new_alloc_ptr;
    #ifdef CANARY_DEFENSE
    stk->data = new_alloc_ptr + 1;
    // |canary_left| |data[0]| .. |data[capacity - 1]| |canary_right|
    //  ^             ^
    //  | alloc_prt   | data
    #else
    stk->data = new_alloc_ptr;
    #endif

    stk->capacity = new_capacity;

    for (ssize_t i = stk->size; i < stk->capacity; i++)
    {
        stk->data[i] = POISON;
    }

    #ifdef CANARY_DEFENSE
    stk->data[stk->capacity] = RIGHT_BUFFER_CANARY_CORRECT_VALUE;   //left buffer canary was copied by realloc
    #endif

    return ALL_CORRECT;
}

error_id StackInitChecks( stack_t* stk, ssize_t init_capacity )
{
    if (stk == NULL)
    {
        StackAssert(stk);
        return FATAL_ERROR_NO_STACK;
    }

    if (stk->alloc_ptr != NULL || stk->capacity != 0 || stk->size != 0)
    {
        stk->status = ALREADY_INIT;
        StackAssert(stk);
        return ALREADY_INIT;
    }
    if (init_capacity < 0 || init_capacity > SSIZE_MAX / 2)
    {
        stk->capacity = init_capacity;
        stk->status = CAPACITY_ERROR;
        StackAssert(stk);
        return CAPACITY_ERROR;
    }
    return ALL_CORRECT;
}

#ifdef CANARY_DEFENSE
void InitCanaries( stack_t* stk )
{
    stk->alloc_ptr[0]                 =  LEFT_BUFFER_CANARY_CORRECT_VALUE;
    stk->alloc_ptr[stk->capacity + 1] = RIGHT_BUFFER_CANARY_CORRECT_VALUE;
    stk->left_stack_canary  =  LEFT_STACK_CANARY_CORRECT_VALUE;
    stk->right_stack_canary = RIGHT_STACK_CANARY_CORRECT_VALUE;
}
#endif
