#include "stack.h"


void ALL_CORRECT_Stack()
{
    printf(YELLOW "ALL_CORRECT_Stack\n" RET_COL);

    stack_t stk1 = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk1, 7));
    printf("push:    %d\n", StackPush(&stk1, 4));
    printf("pop:     %d\n", StackPop(&stk1, &value));
    printf("destroy: %d\n", StackDtor(&stk1));
}

void INCORRECT_COPY_Stack()
{
    printf(YELLOW "INCORRECT_COPY_Stack\n" RET_COL);

    stack_t stk1 = {}, stk2 = {};

    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk1, 7));
    printf("push1:    %d\n", StackPush(&stk1, 4));
    printf("push2:    %d\n", StackPush(&stk1, -12));

    printf("init_correct: %d\n", StackInitByCopy(&stk1, &stk2));
    printf("push_correct:    %d\n", StackPush(&stk2, 4));
    printf("pop_correct:    %d\n", StackPop(&stk2, &value));

    stack_t stk3 = stk1;
    printf("push_incorrect:    %d\n", StackPush(&stk3, 4));
    printf("pop_incorrect:    %d\n", StackPop(&stk3, &value));
    printf("destroy1: %d\n", StackDtor(&stk1));
    printf("destroy2: %d\n", StackDtor(&stk2));
    printf("destroy3: %d\n", StackDtor(&stk3));
}

void SIZE_ERROR_Stack()
{
    printf(YELLOW "SIZE_ERROR_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    stk.size = -7;
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDtor(&stk));
}

void CAPACITY_ERROR_Stack()
{
    printf(YELLOW "CAPACITY_ERROR_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    stk.capacity = -7;
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDtor(&stk));
}

void UNINIT_Stack()
{
    printf(YELLOW "UNINIT_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDtor(&stk));
}

void ALREADY_INIT_Stack()
{
    printf(YELLOW "ALREADY_INIT_Stack\n" RET_COL);

    stack_t stk1 = {};
    StackInit(&stk1, 3);
    stack_t stk = stk1;
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDtor(&stk));
}

void OOM_Stack()
{
    printf(YELLOW "OOM_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 31431414123));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDtor(&stk));
}

void UNDERFLOW_Stack()
{
    printf(YELLOW "UNDERFLOW_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 0));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop1:    %d\n", StackPop(&stk, &value));
    printf("pop2:    %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDtor(&stk));
}

void DEAD_CANARY_Stack()
{
    printf(YELLOW "DEAD_CANARY_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("push:    %d\n", StackPush(&stk, 4));
    stk.alloc_ptr[0] = 232443;
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("destroy: %d\n", StackDtor(&stk));
}

void BAD_HASH_Stack()
{
    printf(YELLOW "BAD_HASH_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("push:    %d\n", StackPush(&stk, 4));
    stk.data[1] = 12;
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("destroy: %d\n", StackDtor(&stk));
}

void StackUnitTest()
{

    ALL_CORRECT_Stack();

    ALREADY_INIT_Stack();

    INCORRECT_COPY_Stack();

    UNINIT_Stack();

    SIZE_ERROR_Stack();

    CAPACITY_ERROR_Stack();

    OOM_Stack();

    UNDERFLOW_Stack();

    BAD_HASH_Stack();

    DEAD_CANARY_Stack();

}
