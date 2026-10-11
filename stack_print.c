#include "stack_print.h"

void StackPrint( stack_t* stk, FILE* stream )
{
    if (stream == NULL || stk == NULL)
        return;

    size_t tabs = 0;

    PrintStackHead(stk, stream, &tabs);
    PrintStackBody(stk, stream, &tabs);
    PrintStackTail(stk, stream, &tabs);

    return;
}

void PrintStackHead( const stack_t* stk, FILE* stream, size_t* tabs )
{
    #ifdef STACK_DEBUG
    fprintf(stream, "__stack[0x%p]__ created at %s:%s():%i\n", stk, stk->file, stk->func, stk->line);
    #else
    fprintf(stream, "__stack[0x%p]__\n", stk);
    #endif

    fopen_bracket(tabs, stream);

    #ifdef CANARY_DEFENSE
    fprintf_with_tabs(*tabs, stream, "canary = 0x%llX (correct value = 0x%llX <%s>)\n",
                      stk->left_stack_canary, LEFT_STACK_CANARY_CORRECT_VALUE,
                      stk->left_stack_canary == LEFT_STACK_CANARY_CORRECT_VALUE ? "true" : "false")
    #endif

    fprintf_with_tabs(*tabs, stream, "capacity = %zd\n", stk->capacity)
    fprintf_with_tabs(*tabs, stream, "size = %zd\n", stk->size)

    #ifdef STACK_DEBUG
    fprintf_with_tabs(*tabs, stream, "alloc_ptr[0x%p]\n", &stk->alloc_ptr)
    #endif
}

void PrintStackBody( stack_t* stk, FILE* stream, size_t* tabs )
{
    fprintf_with_tabs(*tabs, stream, "data[0x%p]\n", &stk->data)
    fopen_bracket(tabs, stream);

    #ifdef _VERIFY_H_
    stk->status = StackVerifier(stk);
    #endif

    if (stk->status != CAPACITY_ERROR && stk->status != UNINIT_STACK HASH_ON(&& stk->status != BAD_HASH))
    {
        #ifdef CANARY_DEFENSE
        fprintf_with_tabs(*tabs, stream, "canary = 0x%X (correct value = 0x%X <%s>)\n",
                          stk->alloc_ptr[0], LEFT_BUFFER_CANARY_CORRECT_VALUE,
                          stk->alloc_ptr[0] == LEFT_BUFFER_CANARY_CORRECT_VALUE ? "true" : "false")
        #endif

        for (size_t i = 0; i < stk->capacity; i++)
        {
            if (i < stk->size)
                fprintf_with_tabs(*tabs, stream, "(+)")
            else
                fprintf_with_tabs(*tabs, stream, "(-)")
            fprintf(stream, "[%zd] = " OUTPUT_SPECIFIER "%s\n",
                    i, stk->data[i], stk->data[i] == POISON ? " (POISON_VALUE)" : "");
        }

        #ifdef CANARY_DEFENSE
        fprintf_with_tabs(*tabs, stream, "canary = 0x%X (correct value = 0x%X <%s>)\n",
                          stk->alloc_ptr[stk->capacity + 1], RIGHT_BUFFER_CANARY_CORRECT_VALUE,
                          stk->alloc_ptr[stk->capacity + 1] == RIGHT_BUFFER_CANARY_CORRECT_VALUE ? "true" : "false")
        #endif
    }

    fclose_bracket(tabs, stream);
}

void PrintStackTail( const stack_t* stk, FILE* stream, size_t* tabs )
{
    #ifdef STACK_DEBUG
    fprintf_with_tabs(*tabs, stream, "canary = 0x%llX (correct value = 0x%llX <%s>)\n",
                      stk->right_stack_canary, RIGHT_STACK_CANARY_CORRECT_VALUE,
                      stk->right_stack_canary == RIGHT_STACK_CANARY_CORRECT_VALUE ? "true" : "false")
    fprintf_with_tabs(*tabs, stream, "stack_hash = 0x%llX\n", stk->hash_stack_value)
    fprintf_with_tabs(*tabs, stream, "buffer_hash = 0x%llX\n", stk->hash_buffer_value)
    #endif

    stk = stk;

    fclose_bracket(tabs, stream);
    fprintf(stream, "\n\n\n");
}

void fopen_bracket( size_t* tabs, FILE* stream )
{
    fprintf_with_tabs(*tabs, stream, "{\n")
    (*tabs)++;
}

void fclose_bracket( size_t* tabs, FILE* stream )
{
    (*tabs)--;
    fprintf_with_tabs(*tabs, stream, "}\n")
}

void FprintNTabs( FILE* stream, size_t tabs )
{
    for (size_t i = 0; i < tabs; i++)
        fprintf(stream, "\t");

    return;
}
