#include "stack.h"
#include "tools.h"

int main()
{
    stack_t stk = {};
    stack_el_t value = 0;
    size_t size = 0;
    int ch = 0;

    printf(MENU);
    while ((ch = getchar()) != 'q')
    {
        switch (ch)
        {
            case '1':
                scanf("%zu", &stk.capacity);
                STACKINIT(&stk, stk.capacity);
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
                StackPrint(&stk, stdout);
                break;
            case 'q':
                break;
            default:
                printf(RED "Invalid input\n" RETURN_COL);
                break;

            STACK_OK(&stk, IS_OK)
        }
        printf(MENU);
        CleanBuffer();
    }
    StackDestroy(&stk);
    return 0;
}
