#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

#include "stack.h"

void StackUnitTest();

void ALL_CORRECT_Stack();
void ALREADY_INIT_Stack();
void UNINIT_Stack();
void INCORRECT_COPY_Stack();
void CAPACITY_ERROR_Stack();
void SIZE_ERROR_Stack();
void OOM_Stack();
void UNDERFLOW_Stack();
void DEAD_CANARY_Stack();
void BAD_HASH_Stack();

#endif
