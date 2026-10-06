#include "stack.h"
#include "unit_test.h"


void CleanBuffer();
#define MENU BLUE                                 \
            "menu:\n"                             \
            "1 - init stack sizeof\n"             \
            "2 - push to stack\n"                 \
            "3 - pop from stack\n"                \
            "4 - print stack\n"                   \
            "5 - destroy stack\n"                 \
            "t - test verifications\n"            \
            "q - exit program\n"                  \
            RET_COL

int main()
{
    stack_t stk = {};
    stack_el_t value = 0;
    size_t stack_init_capacity = 0;
    int ch = 0;

    printf(MENU);
    while ((ch = getchar()) != 'q')
    {
        switch (ch)
        {
            case '1':
                scanf("%zu", &stack_init_capacity);
                StackInit(&stk, stack_init_capacity);
                break;
            case '2':
                scanf(OUTPUT_SPECIFIER, &value);
                StackPush(&stk, value);
                break;
            case '3':
                StackPop(&stk, &value);
                printf(OUTPUT_SPECIFIER "\n", value);
                break;
            case '4':
                if (StackAssert(&stk) != FATAL_ERROR_NO_STACK)
                    StackPrint(&stk, stdout);
                break;
            case '5':
                StackDtor(&stk);
                break;
            case 't':
                StackUnitTest();
                break;
            default:
                printf(RED "Invalid input\n" RET_COL);
                break;
        }
        printf(MENU);
        CleanBuffer();
    }

    StackDtor(&stk);
    return 0;
}

void CleanBuffer()
{
    int n = 0;

    while ((n = getchar()) != '\n' && n != EOF)
        continue;

    return;
}
