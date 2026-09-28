#include "tools.h"

void fopen_bracket( size_t* tabs, FILE* stream )
{
    FPRINTFWITHTABS(*tabs, stream, "{\n")
    (*tabs)++;
}

void fclose_bracket( size_t* tabs, FILE* stream )
{
    (*tabs)--;
    FPRINTFWITHTABS(*tabs, stream, "{\n")
}

void FprintNTabs( FILE* stream, size_t tabs )
{
    for (size_t i = 0; i < tabs; i++)
        fprintf(stream, "\t");

    return;
}

void CleanBuffer()
{
    int n = 0;

    while ((n = getchar()) != '\n' && n != EOF)
        continue;

    return;
}

int CompareDoubles(const double a, const double b)
{
    if ((a - b) > EPS)
        return MORE;
    else if ((a - b) < -EPS)
        return LESS;
    else if (isnan(a) == 1 && isnan(b) == 1)
        return EQUAL;
    else if ((isnan(a) == 1 || isnan(b) == 1) && (isnan(a) != 1 || isnan(b) != 1))
        return UNDEFINED;
    else
        return EQUAL;
}
