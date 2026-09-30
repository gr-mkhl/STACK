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
                scanf(OUTPUT_SPECIFIER, &value);
                STACKINIT(&stk, value);
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
                if (StackOk(&stk) != FATAL_ERROR_NO_STACK)
                    StackPrint(&stk, stdout);
                break;
            default:
                printf(RED "Invalid input\n" RETURN_COL);
                break;
        }
        printf(MENU);
        CleanBuffer();
    }
    StackDestroy(&stk);
    return 0;
}
