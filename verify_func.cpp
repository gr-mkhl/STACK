#include "stack.h"


//   ATTENTION!!!
//
//   StackPrint doesn't check stack
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
    ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%llX (correct value = 0x%llX <%s>)\n",
                             stk->left_stack_canary, LEFT_STACK_CANARY_CORRECT_VALUE,
                             stk->left_stack_canary == LEFT_STACK_CANARY_CORRECT_VALUE ? "true" : "false"))
    FPRINTFWITHTABS(tabs, stream, "capacity = %zd\n", stk->capacity)
    FPRINTFWITHTABS(tabs, stream, "size = %zd\n", stk->size)
    FPRINTFWITHTABS(tabs, stream, "data[0x%p]\n", &stk->data)
    fopen_bracket(&tabs, stream);
    ON_DEBUG
    (
    if (stk->status != CAPACITY_ERROR && stk->status != UNINIT_STACK && stk->status != STACK_OOM)
    )
    {

        ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%X (correct value = 0x%X <%s>)\n",
                                stk->alloc_ptr[0], LEFT_BUFFER_CANARY_CORRECT_VALUE,
                                stk->alloc_ptr[0] == LEFT_BUFFER_CANARY_CORRECT_VALUE ? "true" : "false"))
        size_t stack_size = stk->size;
        size_t stack_capacity = stk->capacity;

        for (size_t i = 0; i < stack_capacity; i++)
        {
            if (i < stack_size)
                FPRINTFWITHTABS(tabs, stream, "(+)")
            else
                FPRINTFWITHTABS(tabs, stream, "(-)")
            fprintf(stream, "[%zd] = " OUTPUT_SPECIFIER "%s\n", i, stk->data[i],
                    stk->data[i] == POISON ? " (POISON_VALUE)" : "");
        }
        ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%X (correct value = 0x%X <%s>)\n",
                                stk->alloc_ptr[stack_capacity + 1], RIGHT_BUFFER_CANARY_CORRECT_VALUE,
                                stk->alloc_ptr[stack_capacity + 1] == RIGHT_BUFFER_CANARY_CORRECT_VALUE ? "true" : "false"))
    }
    fclose_bracket(&tabs, stream);
    ON_DEBUG(FPRINTFWITHTABS(tabs, stream, "canary = 0x%llX (correct value = 0x%llX <%s>)\n",
                             stk->right_stack_canary, RIGHT_STACK_CANARY_CORRECT_VALUE,
                             stk->right_stack_canary == RIGHT_STACK_CANARY_CORRECT_VALUE ? "true" : "false"))
    fclose_bracket(&tabs, stream);

    return ALL_CORRECT;
}



enum error_id StackAssertF( stack_t* stk
              ON_DEBUG(, const char* file, const char* func, int line))
{
    if (stk == NULL)
    {
        StackDump(stk, FATAL_ERROR_NO_STACK
                  ON_DEBUG(, file, func, line));
        return FATAL_ERROR_NO_STACK;
    }
    ON_DEBUG(
    if (stk->status != ALL_CORRECT)
    {
        StackDump(stk, stk->status, file, func, line);
        return stk->status;
    })
    enum error_id is_error = StackVerifier(stk);
    if (is_error != ALL_CORRECT)
    {
        ON_DEBUG(StackDump(stk, is_error, file, func, line);)
        return ON_DEBUG(stk->status = )is_error;
    }

    return ON_DEBUG(stk->status = )ALL_CORRECT;
}



void StackDump( stack_t* stk, enum error_id error
      ON_DEBUG(, const char* file, const char* func, int line))
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
        fprintf(error_file, "ERROR %d: " ON_DEBUG("in file \"%s\" function \"%s()\" line %d") "\n\n", error ON_DEBUG(, file, func, line));
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
            case ALREADY_INIT:
                fprintf(error_file, "-> Stack is already initialized\n\n");
                StackPrint(stk, error_file);
                ON_DEBUG(stk->status = ALL_CORRECT;)
                break;
/*
            case ALREADY_DESTROYED:
                fprintf(error_file, "-> Stack is already destroyed\n\n");
                StackPrint(stk, error_file);
                break;
*/
            case STACK_UNDERFLOW:
                fprintf(error_file, "-> Stack underflow\n\n");
                StackPrint(stk, error_file);
                ON_DEBUG(stk->status = ALL_CORRECT;)
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

enum error_id StackVerifier( stack_t* stk )
{
    if (stk == NULL)
        return FATAL_ERROR_NO_STACK;

    if (stk->alloc_ptr == NULL)
        return ON_DEBUG(stk->status = )UNINIT_STACK;

    if (stk->capacity > SIZE_MAX - EPS)                         // <=> (stk->capacity < 0)
        return ON_DEBUG(stk->status = )CAPACITY_ERROR;          //size_t  is unsigned and always > 0,
                                                                //so we need to check "infinite" value

    if (stk->size > SIZE_MAX - EPS || stk->size > stk->capacity) // <=> (stk->capacity < 0)
        return ON_DEBUG(stk->status = )SIZE_ERROR;               //size_t  is unsigned and always > 0,
                                                                 //so we need to check "infinite" value
    if (AreNotCanariesAlive(stk))
        return ON_DEBUG(stk->status = )DIED_CANARY;

    return ON_DEBUG(stk->status = )ALL_CORRECT;
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




void fopen_bracket( size_t* tabs, FILE* stream )
{
    FPRINTFWITHTABS(*tabs, stream, "{\n")
    (*tabs)++;
}

void fclose_bracket( size_t* tabs, FILE* stream )
{
    (*tabs)--;
    FPRINTFWITHTABS(*tabs, stream, "}\n")
}

void FprintNTabs( FILE* stream, size_t tabs )
{
    for (size_t i = 0; i < tabs; i++)
        fprintf(stream, "\t");

    return;
}
