#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <stdio.h>
#include <math.h>

#define EPS 1E-10
enum SIGNS {LESS = -1, EQUAL = 0, MORE = 1, UNDEFINED = -2};

#define FPRINTFWITHTABS(TABS, STREAM, ...) do                  \
                            {                                  \
                                FprintNTabs(STREAM, TABS);     \
                                fprintf(STREAM, __VA_ARGS__);  \
                            } while(0);


void fopen_bracket( size_t* tabs, FILE* stream );
void fclose_bracket( size_t* tabs, FILE* stream );
void CleanBuffer();
void FprintNTabs( FILE* stream, size_t tabs );
int CompareDoubles( const double a, const double b );

#endif
