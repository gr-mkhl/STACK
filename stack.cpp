#include "stack.h"
#include "tools.h"

int StackInit( stack_t* stk, size_t capacity
               ON_DEBUG(, const char* file, const char* func, int line)) //NOTE capacity must be > 0
{
    assert(stk);
    assert(capacity > 0);
    assert(stk->data == NULL);
    assert(stk->capacity == 0);
    assert(stk->size == 0);

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

    return 0;
}

int StackPush( stack_t* stk, stack_el_t value )
{
    assert(stk);
    assert(stk->data);


    if (stk->size == stk->capacity)
    {
        stk->capacity *= 2;
        stk->data = (stack_el_t*)realloc(stk->data, stk->capacity * sizeof(stack_el_t));
        assert(stk->data);
        for (size_t i = stk->size; i < stk->capacity; i++)
            stk->data[i] = POISON;
    }


    stk->data[stk->size++] = value;

    return 0;
}

stack_el_t StackPop( stack_t* stk )
{
    assert(stk);
    assert(stk->size > 0);

    if (2 * stk->size  < stk->capacity)
    {
       stk->capacity /= 2;
       stk->data = (stack_el_t*)realloc(stk->data, stk->capacity * sizeof(stack_el_t));
       assert(stk->data);
    }
    stack_el_t value = stk->data[--stk->size];
    stk->data[stk->size] = POISON;

    return value;
}

int StackDestroy( stack_t* stk )
{
    assert(stk);

    size_t capacity = stk->capacity;
    for (size_t i = 0; i < capacity; i++)
    {
        stk->data[i] = POISON;
    }

    stk->size = 0;
    stk->capacity = 0;
    assert(stk->data != NULL);
    free(stk->data);

    return 0;
}

int StackPrint( stack_t* stk, FILE* stream )
{
    assert(stk);

    size_t tabs = 0;

    fprintf(stream, "__stack[0x%p]__"
            ON_DEBUG(" initialised at %s:%s():%i")
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
            (CompareDoubles(stk->data[i], POISON) == EQUAL ? " (POISON_VALUE2)" : ""));
    }
    fclose_bracket(&tabs, stream);
    fclose_bracket(&tabs, stream);

    return 0;
}


/*
enum ERRORS StackVerifier( stack_t* stk, enum FUNC func )
{
    if (stk == NULL)
        return UNINIT_STACK;

    if (stk->data != NULL && func == INIT)
        return ALREADY_INIT_STACK;
    if (stk->capacity < 0)
}
*/



