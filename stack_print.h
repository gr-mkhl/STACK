#ifndef _STACK_PRINT_H_
#define _STACK_PRINT_H_

#include <stdio.h>
#include "stack_struct.h"

#define fprintf_with_tabs(TABS, STREAM, ...) do                \
                            {                                  \
                                FprintNTabs(STREAM, TABS);     \
                                fprintf(STREAM, __VA_ARGS__);  \
                            } while(0);
void StackPrint( stack_t* stk, FILE* stream );
void PrintStackHead( const stack_t* stk, FILE* stream, size_t* tabs );
void PrintStackBody( stack_t* stk, FILE* stream, size_t* tabs );
void PrintStackTail( const stack_t* stk, FILE* stream, size_t* tabs );

void fopen_bracket( size_t* tabs, FILE* stream );
void fclose_bracket( size_t* tabs, FILE* stream );
void FprintNTabs( FILE* stream, size_t tabs );

#endif
