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
                scanf("%zd", &size);
                STACKINIT(&stk, size);
                StackPrint(&stk, stdout);
                break;
            case '2':
                scanf(OUTPUT_SPECIFIER, &value);
                StackPush(&stk, value);
                StackPrint(&stk, stdout);
                break;
            case '3':
                printf(OUTPUT_SPECIFIER "\n", StackPop(&stk));
                StackPrint(&stk, stdout);
                break;
            case '4':
                StackPrint(&stk, stdout);
                break;
            case 'q':
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
