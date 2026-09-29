#include "stack.h"
#include "tools.h"

enum ERRORS StackInit( stack_t* stk, size_t capacity
               ON_DEBUG(, const char* file, const char* func, int line)) //NOTE capacity must be > 0
{
    STACK_OK(stk, INIT)

    #ifdef STACK_DEBUG
    stk->file = file;
    stk->func = func;
    stk->line = line;
    #endif

    stk->data = (stack_el_t*)calloc(capacity, sizeof(stack_el_t));
    assert(stk->data);
    stk->capacity = capacity;
    for (size_t i = 0; i < stk->capacity; i++)
        stk->data[i] = POISON;

    return ALL_CORRECT;
}

enum ERRORS StackPush( stack_t* stk, stack_el_t value )
{
    STACK_OK(stk, PUSH)


    if (stk->size + 1 == stk->capacity)
    {
        stk->capacity *= 2;
        stk->data = (stack_el_t*)realloc(stk->data, stk->capacity * sizeof(stack_el_t));
        assert(stk->data);
        for (size_t i = stk->size; i < stk->capacity; i++)
            stk->data[i] = POISON;
    }


    stk->data[stk->size++] = value;

    return ALL_CORRECT;
}

enum ERRORS StackPop( stack_t* stk, stack_el_t* value )
{
    STACK_OK(stk, POP)

    if (4 * stk->size  < stk->capacity)
    {
       stk->capacity /= 2;
       stk->data = (stack_el_t*)realloc(stk->data, stk->capacity * sizeof(stack_el_t));
       assert(stk->data);
    }
    STACK_OK(stk, IS_OK)
    *value = stk->data[--stk->size];
    stk->data[stk->size] = POISON;

    return ALL_CORRECT;
}

enum ERRORS StackDestroy( stack_t* stk )
{
    STACK_OK(stk, DESTROY)

    size_t capacity = stk->capacity;
    for (size_t i = 0; i < capacity; i++)
    {
        stk->data[i] = POISON;
    }

    stk->size = 0;
    stk->capacity = 0;
    free(stk->data);
    stk->data = NULL;

    return ALL_CORRECT;
}


enum ERRORS StackPrint( const stack_t* stk, FILE* stream )
{
    STACK_OK(stk, IS_OK)

    assert(stream);

    size_t tabs = 0;

    fprintf(stream, "__stack[0x%p]__"
            ON_DEBUG(" created at %s:%s():%i")
            "\n", stk
            ON_DEBUG(, stk->file, stk->func, stk->line));

    fopen_bracket(&tabs, stream);
    FPRINTFWITHTABS(tabs, stream, "capacity = %zd\n", stk->capacity)
    FPRINTFWITHTABS(tabs, stream, "size = %zd\n", stk->size)
    FPRINTFWITHTABS(tabs, stream, "data[0x%p]\n", &stk->data)
    fopen_bracket(&tabs, stream);

    size_t stack_size = stk->size;
    size_t stack_capacity = stk->capacity;

    for (size_t i = 0; i < stack_capacity; i++)
    {
        if (i < stack_size)
            FPRINTFWITHTABS(tabs, stream, "(+)")
        else
            FPRINTFWITHTABS(tabs, stream, "(-)")
        fprintf(stream, "[%zd] = " OUTPUT_SPECIFIER "%s\n", i, stk->data[i],
            (CompareDoubles(stk->data[i], POISON) == EQUAL ? " (POISON_VALUE)" : ""));
    }
    fclose_bracket(&tabs, stream);
    fclose_bracket(&tabs, stream);

    return ALL_CORRECT;
}

enum ERRORS StackVerifier( const stack_t* stk, enum FUNC func )
{
    if (stk == NULL)
        return FATAL_ERROR_NO_STACK;

    if (func == IS_OK)
    {
        if (stk->data == NULL && stk->capacity > 0)
            return STACK_OOM;
        if (stk->data == NULL)
            return UNINIT_STACK;

        if (stk->capacity <= 0)
            return CAPACITY_ERROR;
        if (stk->size < 0 || stk->size > stk->capacity)
            return SIZE_ERROR;
        return ALL_CORRECT;
    }

    if (func == INIT)
    {
        if (stk->data != NULL && stk->capacity > 0)
            return ALREADY_INIT_STACK;
        return ALL_CORRECT;
    }

    if (func == PUSH)
    {
        if (stk->data == NULL && stk->capacity > 0)
            return STACK_OOM;
        if (stk->data == NULL)
            return UNINIT_STACK;
        if (stk->capacity <= 0)
            return CAPACITY_ERROR;
        if (stk->size < 0 || stk->size > stk->capacity)
            return SIZE_ERROR;
        if (stk->size == stk->capacity)
            return STACK_OVERFLOW;
        return ALL_CORRECT;
    }

    if (func == POP)
    {
        if (stk->capacity <= 0)
            return CAPACITY_ERROR;
        if (stk->size < 0 || stk->size > stk->capacity)
            return SIZE_ERROR;
        if (stk->data == NULL && stk->capacity > 0)
            return STACK_OOM;
        if (stk->data == NULL)
            return UNINIT_STACK;
        if (stk->size == 0)
            return STACK_UNDERFLOW;
        return ALL_CORRECT;
    }

    if (func == DESTROY)
    {
        if (stk->data == NULL)
            return UNINIT_STACK;
        if (stk->data == NULL)
            return ALREADY_DESTROYED_STACK;
        return ALL_CORRECT;
    }

    return ALL_CORRECT;
}



void StackDump( const stack_t* stk, enum ERRORS error
                ON_DEBUG(, const char* file, const char* func, int line) )
{

    FILE* error_file = fopen("ERRORS.txt", "a");
    if (error_file == NULL)
        return;

    if (error == FATAL_ERROR_NO_STACK)
    {
        fprintf(error_file, "/nFATAL_ERROR: in file \"%s\" function \"%s()\" line %d\n"
                            "-> NULL-pointer was received instead of pointer to the stack\n", file, func, line);
        fclose(error_file);
        abort();
        return;
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
            case STACK_OVERFLOW:
                fprintf(error_file, "-> Stack overflow:\n\n");
                StackPrint(stk, error_file);
                break;
            case STACK_UNDERFLOW:
                fprintf(error_file, "-> Stack underflow\n\n");
                StackPrint(stk, error_file);
                break;
        }
    }
    fclose(error_file);

    return;

}
