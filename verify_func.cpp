#include "stack.h"


//   ATTENTION!!!                   //TODO maybe i should do sth with this
//
//   StackPrint doesn't check stack
//   you need to check it before calling it
//
//   ATTENTION!!!


void StackPrint( const stack_t* stk, FILE* stream )
{
    if (stream == NULL || stk->status == UNINIT_STACK)
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
    fopen_bracket(tabs, stream);
    fprintf_with_tabs(*tabs, stream, "canary = 0x%llX (correct value = 0x%llX <%s>)\n",
                      stk->left_stack_canary, LEFT_STACK_CANARY_CORRECT_VALUE,
                      stk->left_stack_canary == LEFT_STACK_CANARY_CORRECT_VALUE ? "true" : "false")
    fprintf_with_tabs(*tabs, stream, "capacity = %zd\n", stk->capacity)
    fprintf_with_tabs(*tabs, stream, "size = %zd\n", stk->size)
    fprintf_with_tabs(*tabs, stream, "alloc_ptr[0x%p]\n", &stk->alloc_ptr)
    #elif
    fprintf(stream, "__stack[0x%p]__\n", stk);
    fopen_bracket(tabs, stream);
    fprintf_with_tabs(*tabs, stream, "capacity = %zd\n", stk->capacity)
    fprintf_with_tabs(*tabs, stream, "size = %zd\n", stk->size)
    #endif
}

void PrintStackBody( const stack_t* stk, FILE* stream, size_t* tabs )
{
    fprintf_with_tabs(*tabs, stream, "data[0x%p]\n", &stk->data)
    fopen_bracket(tabs, stream);

    if (stk->status != CAPACITY_ERROR && stk->status != UNINIT_STACK)
    {
        #ifdef STACK_DEBUG
        fprintf_with_tabs(*tabs, stream, "canary = 0x%X (correct value = 0x%X <%s>)\n",
                          stk->alloc_ptr[0], LEFT_BUFFER_CANARY_CORRECT_VALUE,
                          stk->alloc_ptr[0] == LEFT_BUFFER_CANARY_CORRECT_VALUE ? "true" : "false")
        #endif

        for (ssize_t i = 0; i < stk->capacity; i++)
        {
            if (i < stk->size)
                fprintf_with_tabs(*tabs, stream, "(+)")
            else
                fprintf_with_tabs(*tabs, stream, "(-)")
            fprintf(stream, "[%zd] = " OUTPUT_SPECIFIER "%s\n",
                    i, stk->data[i], stk->data[i] == POISON ? " (POISON_VALUE)" : "");
        }

        #ifdef STACK_DEBUG
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
    fprintf_with_tabs(*tabs, stream, "stack_hash = %llu\n", stk->hash_stack_value)
    fprintf_with_tabs(*tabs, stream, "buffer_hash = %llu\n", stk->hash_buffer_value)
    #endif

    fclose_bracket(tabs, stream);
}

//used hash algorithm djb2: https://dev.to/doogal/djb2-hash-function-string-to-integer-algorithm-explained-4bii?ysclid=mur1ep12vp401464212

uint64_t Hash_djb2( const void* beg, const void* end, const void* except, size_t except_el_size )
{
    uint64_t hash = 5381;

    for (const uint8_t* i = (const uint8_t*)beg; i <= end; i++)
    {
        if (i == except)
            i += except_el_size;
        hash =  (hash << 5) + hash + *i;
    }
    return hash;
}

void RecalcHash( stack_t* stk )
{
    stk->hash_buffer_value = Hash_djb2(&stk->alloc_ptr[0], &stk->alloc_ptr[stk->capacity + 1], NULL, 0);

    stk->hash_stack_value = Hash_djb2(stk, (uint8_t*)(stk) + sizeof(stack_t) - 1, &stk->hash_stack_value, sizeof(stk->hash_stack_value));

    //NOTE - dont change order of this lines
}

error_id StackAssertF( stack_t* stk
            ON_DEBUG(, const char* file, const char* func, int line))
{
    if (stk == NULL)
    {
        StackDump(stk, FATAL_ERROR_NO_STACK
         ON_DEBUG(, file, func, line));
        return FATAL_ERROR_NO_STACK;
    }
    if (stk->status != ALL_CORRECT)
    {
        StackDump(stk, stk->status
         ON_DEBUG(, file, func, line));
        return stk->status;
    }
    error_id error = StackVerifier(stk);
    if (error != ALL_CORRECT)
    {
        StackDump(stk, error
         ON_DEBUG(, file, func, line));
        return stk->status = error;
    }
    return stk->status = ALL_CORRECT;
}



void StackDump( stack_t* stk,  error_id error
      ON_DEBUG(, const char* file, const char* func, int line))
{
    FILE* error_file = fopen(ERROR_FILE_NAME, "a");
    if (error_file == NULL)
        return;
#ifdef STACK_DEBUG
    if (error != UNINIT_STACK)
        fprintf(error_file, "ERROR %d: in file \"%s\" function \"%s()\" line %d\n\n",
                error, file, func, line);
    else
        fprintf(error_file, "ERROR %d:\n\n", error);

#elif
    fprintf(error_file, "ERROR %d:\n\n", error);
#endif

    switch (error)
    {
        case FATAL_ERROR_NO_STACK:
            fprintf(error_file, "-> NULL-pointer was received instead of pointer to the stack\n\n\n");
            fclose(error_file);
            abort();
            break;
        case DEAD_CANARY:
            fprintf(error_file, "-> Canary is dead. Press F to pay respect\n\n\n");
            StackPrint(stk, error_file);
            break;
        case SIZE_ERROR:
            fprintf(error_file, "-> Stack size error\n\n\n");
            StackPrint(stk, error_file);
            break;
        case CAPACITY_ERROR:
            fprintf(error_file, "-> Stack capacity error\n\n\n");
            StackPrint(stk, error_file);
            break;
        case STACK_OOM:
            fprintf(error_file, "-> Stack allocation error\n\n\n");
            StackPrint(stk, error_file);
            stk->status = ALL_CORRECT;
            break;
        case UNINIT_STACK:
            fprintf(error_file, "-> Stack is not initialized\n\n\n");
            //NOTE we can't print uninit stack
            break;
        case ALREADY_INIT:
            fprintf(error_file, "-> Stack is already initialized\n\n\n");
            StackPrint(stk, error_file);
            stk->status = ALL_CORRECT;
            break;
        case BAD_HASH:
            fprintf(error_file, "-> Hash destroyed\n\n\n");
            StackPrint(stk, error_file);
            break;
        case STACK_UNDERFLOW:
            fprintf(error_file, "-> Stack underflow\n\n\n");
            StackPrint(stk, error_file);
            stk->status = ALL_CORRECT;
            break;
        case CANT_INIT:
            fprintf(error_file, "-> Stack wasn't init, because there wasn't enough memory for it\n\n\n");
            break;
        default:
            fprintf(error_file, "-> UNDEFINED ERROR?!\n"
                                "ERROR CODE:%d\n\n\n", error);
            StackPrint(stk, error_file);
            break;
    }
    fclose(error_file);

    return;
}

 error_id StackVerifier( stack_t* stk )
{
    if (stk == NULL)
        return FATAL_ERROR_NO_STACK;

    if (stk->alloc_ptr == NULL)
        return stk->status = UNINIT_STACK;

    if (stk->capacity < 0)
        return stk->status = CAPACITY_ERROR;

    if (stk->size < 0 || stk->size > stk->capacity)
        return stk->status = SIZE_ERROR;

    if (AreNotCanariesAlive(stk))
        return stk->status = DEAD_CANARY;

    if (Hash_djb2(stk, (const uint8_t*)stk + sizeof(stack_t) - 1, &stk->hash_stack_value, sizeof(stk->hash_stack_value)) != stk->hash_stack_value ||
        Hash_djb2(&stk->alloc_ptr[0], &stk->alloc_ptr[stk->capacity + 1], NULL, 0) != stk->hash_buffer_value)
        return stk->status = BAD_HASH;

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
