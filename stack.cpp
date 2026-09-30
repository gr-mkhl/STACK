#include "stack.h"
#include "tools.h"

enum error_id StackInit( stack_t* stk, size_t init_capacity
               ON_DEBUG(, const char* file, const char* func, int line))
{
    if (stk == NULL)
        StackOk(stk);
    if (stk->alloc_ptr != NULL || stk->capacity > 0 || stk->size > 0)
    {
        ON_DEBUG(stk->status = ALREADY_INIT_STACK;
                 StackOk(stk);)
        return ALREADY_INIT_STACK;
    }

    #ifdef STACK_DEBUG
    stk->file = file;
    stk->func = func;
    stk->line = line;
    #endif

    if (init_capacity < 0)
    {
        ON_DEBUG(stk->status = CAPACITY_ERROR;
                 StackOk(stk);)
        return CAPACITY_ERROR;
    }

    stk->alloc_ptr = (stack_el_t*)calloc(init_capacity + 2, sizeof(stack_el_t)); //TODO - init_alloc_ptr?
    //                                                   ^
    //                                                   | 2 more elements for canaries
    if (stk->alloc_ptr == NULL)
    {
        ON_DEBUG(stk->status = STACK_OOM;
                 StackOk(stk);)
        return STACK_OOM;
    }

    stk->data = stk->alloc_ptr + 1;
    // |canary_left| |data[0]| .. |data[capacity - 1]| |canary_right|
    //  ^             ^
    //  | alloc_prt   | data

    stk->capacity = init_capacity;
    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = POISON;

    stk->alloc_ptr[0] = LEFT_BUFFER_CANARY_CORRECT_VALUE;
    stk->alloc_ptr[stk->capacity + 1] = RIGHT_BUFFER_CANARY_CORRECT_VALUE;
    stk->left_stack_canary = LEFT_STACK_CANARY_CORRECT_VALUE;
    stk->right_stack_canary = RIGHT_STACK_CANARY_CORRECT_VALUE;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

enum error_id StackPush( stack_t* stk, stack_el_t value )
{
    StackOk(stk);

    if (stk->size >= stk->capacity) // >= is added just in case.
    {
        size_t new_capacity = stk->capacity != 0 ? stk->capacity * 2 : 1; //it works not bad, if stk->capacity == 0

        stack_el_t* new_alloc_ptr = (stack_el_t*)realloc(stk->alloc_ptr, (new_capacity + 2) * sizeof(stack_el_t));
        //                                                                                ^
        //                                                   2 more elements for canaries |
        if (new_alloc_ptr == NULL)
        {
            ON_DEBUG(stk->status = STACK_OOM;
                     StackOk(stk);)
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
    StackOk(stk);

    if (stk->size == 0)
    {
        *value = POISON;
        ON_DEBUG(stk->status = STACK_UNDERFLOW;
                 StackOk(stk);)
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
                     StackOk(stk);)
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
    //StackOk(stk);  do we need this?

    if (stk->alloc_ptr == NULL)
    {
        ON_DEBUG(stk->status = ALREADY_DESTROYED_STACK;
                 StackOk(stk);)
        return ALREADY_DESTROYED_STACK;
    }

    size_t need_to_clear = stk->capacity + 2;
    //                                     ^
    //        don't forget about canaries  |
    for (size_t i = 0; i < need_to_clear; i++)
    {
        stk->alloc_ptr[i] = POISON;
    }

    stk->capacity = 0;
    stk->size = 0;
    stk->left_stack_canary = 0;
    stk->right_stack_canary = 0;

    free(stk->alloc_ptr);
    stk->data = NULL;
    stk->alloc_ptr = NULL;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

enum error_id StackVerifier( const stack_t* stk )
{
    if (stk == NULL)
        return FATAL_ERROR_NO_STACK;

    if (stk->alloc_ptr == NULL)
        return UNINIT_STACK;

    if (stk->capacity < 0)                          //TODO - size_t > 0, we need to check infinite value
        return CAPACITY_ERROR;

    if (stk->size < 0 || stk->size > stk->capacity) //TODO - size_t > 0, we need to check infinite value
        return SIZE_ERROR;

    if (AreNotCanariesAlive(stk))
        return DIED_CANARY;

    return ALL_CORRECT;
}

int AreNotCanariesAlive( const stack_t* stk )
{
    if (stk->left_stack_canary            != LEFT_STACK_CANARY_CORRECT_VALUE  ||
        stk->right_stack_canary           != RIGHT_STACK_CANARY_CORRECT_VALUE ||
        stk->alloc_ptr[0]                 != LEFT_BUFFER_CANARY_CORRECT_VALUE ||
        stk->alloc_ptr[stk->capacity + 1] != RIGHT_BUFFER_CANARY_CORRECT_VALUE)
    {
        return 1;
    }
    return 0;
}

//   ATTENTION!!!
//
//    StackPrint doesn't check stack
//   you need to check it before calling it
//
//   ATTENTION!!!

enum error_id StackPrint( const stack_t* stk, FILE* stream )
{
    assert(stream);
    size_t tabs = 0;

    fprintf(stream, "__stack[0x%p]__"
            ON_DEBUG(" created at %s:%s():%i")
            "\n", stk
            ON_DEBUG(, stk->file, stk->func, stk->line));

    fopen_bracket(&tabs, stream);
    ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%llX (correct value = 0x%llX)\n",
                             stk->left_stack_canary, LEFT_STACK_CANARY_CORRECT_VALUE))
    FPRINTFWITHTABS(tabs, stream, "capacity = %zd\n", stk->capacity)
    FPRINTFWITHTABS(tabs, stream, "size = %zd\n", stk->size)
    FPRINTFWITHTABS(tabs, stream, "data[0x%p]\n", &stk->data)
    fopen_bracket(&tabs, stream);

    ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%X (correct value = 0x%X)\n",
                             stk->alloc_ptr[0], LEFT_BUFFER_CANARY_CORRECT_VALUE))
    size_t stack_size = stk->size;
    size_t stack_capacity = stk->capacity;

    for (size_t i = 0; i < stack_capacity; i++)
    {
        if (i < stack_size)
            FPRINTFWITHTABS(tabs, stream, "(+)")
        else
            FPRINTFWITHTABS(tabs, stream, "(-)")
        fprintf(stream, "[%zd] = " OUTPUT_SPECIFIER "%s\n", i, stk->data[i],
            (CompareDoubles(stk->data[i], POISON) == EQUAL ? " (POISON_VALUE)" : ""));  //TODO: - CompareDoubles?
    }
    ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%X (correct value = 0x%X)\n",
                             stk->alloc_ptr[stack_capacity + 1], RIGHT_BUFFER_CANARY_CORRECT_VALUE))
    fclose_bracket(&tabs, stream);
    ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%llX (correct value = 0x%llX)\n",
                             stk->right_stack_canary, RIGHT_STACK_CANARY_CORRECT_VALUE))
    fclose_bracket(&tabs, stream);

    return ALL_CORRECT;
}

enum error_id StackOk( stack_t* stk )
{
    if (stk == NULL)
    {
        StackDump(stk, FATAL_ERROR_NO_STACK
                  ON_DEBUG(, __FILE__, __func__, __LINE__));
        return FATAL_ERROR_NO_STACK;
    }
    ON_DEBUG(
    if (stk->status != ALL_CORRECT)
    {
        StackDump(stk, stk->status, __FILE__, __func__, __LINE__);
        return stk->status;
    })

    enum error_id is_error = StackVerifier(stk);

    if (is_error != ALL_CORRECT)
    {
        StackDump(stk, is_error
                  ON_DEBUG(, __FILE__, __func__, __LINE__));
        return ON_DEBUG(stk->status = )is_error;
    }

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}

void StackDump( const stack_t* stk, enum error_id error
                ON_DEBUG(, const char* file, const char* func, int line) )
{

    FILE* error_file = fopen(ERROR_FILE_NAME, "a");
    if (error_file == NULL)
        return;

    if (error == FATAL_ERROR_NO_STACK)
    {
        fprintf(error_file, "FATAL_ERROR:" ON_DEBUG("in file \"%s\" function \"%s()\" line %d") "\n\n" ON_DEBUG(, file, func, line));
        fprintf(error_file, "-> NULL-pointer was received instead of pointer to the stack\n");
        fclose(error_file);
        abort();
    }
    else if (error == DIED_CANARY)
    {
        fprintf(error_file, "FATAL_ERROR:" ON_DEBUG("in file \"%s\" function \"%s()\" line %d") "\n\n" ON_DEBUG(, file, func, line));
        fprintf(error_file, "-> Canary died. Press F to pay respect\n\n");
        StackPrint(stk, error_file);
        fclose(error_file);
        abort();
    }
    else
    {
        fprintf(error_file, "ERROR:" ON_DEBUG("in file \"%s\" function \"%s()\" line %d") "\n\n" ON_DEBUG(, file, func, line));
        switch (error)
        {
            case SIZE_ERROR:
                fprintf(error_file, "-> Stack size error\n\n");
                StackPrint(stk, error_file);
                break;
            case CAPACITY_ERROR:
                fprintf(error_file, "-> Stack capacity error\n\n");
                StackPrint(stk, error_file);
                break;
            case STACK_OOM:
                fprintf(error_file, "-> Stack allocation error\n\n");
                StackPrint(stk, error_file);
                break;
            case UNINIT_STACK:
                fprintf(error_file, "-> Stack is not initialized\n\n");
                StackPrint(stk, error_file);
                break;
            case ALREADY_INIT_STACK:
                fprintf(error_file, "-> Stack is already initialized\n\n");
                StackPrint(stk, error_file);
                break;
            case STACK_UNDERFLOW:
                fprintf(error_file, "-> Stack underflow\n\n");
                StackPrint(stk, error_file);
                break;

            default:
                fprintf(error_file, "-> UNDEFINED ERROR?!\n\n"
                                    "ERROR CODE:%d\n\n", error);
                StackPrint(stk, error_file);
                break;
        }
    }
    fclose(error_file);

    return;

}
