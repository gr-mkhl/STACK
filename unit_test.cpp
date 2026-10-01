#include "stack.h"


void ALL_CORRECT_Stack()
{
    printf(YELLOW "ALL_CORRECT_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDestroy(&stk));
}

void SIZE_ERROR_Stack()
{
    printf(YELLOW "SIZE_ERROR_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, 7));
    stk.size = -7;
    //StackPrint(&stk, stdout);
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDestroy(&stk));
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
    printf("destroy: %d\n", StackDestroy(&stk));
}

void UNINIT_Stack()
{
    printf(YELLOW "UNINIT_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDestroy(&stk));
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
    printf("destroy: %d\n", StackDestroy(&stk));
}

void OOM_Stack()
{
    printf(YELLOW "OOM_Stack\n" RET_COL);

    stack_t stk = {};
    stack_el_t value = 0;

    printf("init:    %d\n", StackInit(&stk, SIZE_MAX - EPS - 3));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("destroy: %d\n", StackDestroy(&stk));
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
    printf("destroy: %d\n", StackDestroy(&stk));
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
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("pop:     %d\n", StackPop(&stk, &value));
    printf("push:    %d\n", StackPush(&stk, 4));
    printf("destroy: %d\n", StackDestroy(&stk));
}

void StackUnitTest()
{

    ALL_CORRECT_Stack();

    ALREADY_INIT_Stack();

    UNINIT_Stack();

    SIZE_ERROR_Stack();

    CAPACITY_ERROR_Stack();

    OOM_Stack();

    UNDERFLOW_Stack();

    DEAD_CANARY_Stack();

}
