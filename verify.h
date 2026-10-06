#ifndef _VERIFY_H_
#define _VERIFY_H_


#ifdef CANARY_DEFENSE
const unsigned long long LEFT_STACK_CANARY_CORRECT_VALUE = 0xDEADBEEF;
const unsigned long long RIGHT_STACK_CANARY_CORRECT_VALUE = 0xFEEDDED;
const stack_el_t LEFT_BUFFER_CANARY_CORRECT_VALUE= 0xEDADEDA;
const stack_el_t RIGHT_BUFFER_CANARY_CORRECT_VALUE= 0xBEDADEDA;

void InitCanaries( stack_t* stk );
bool AreCanariesDead( const stack_t* stk );
#endif


#ifdef HASH_DEFENSE
uint64_t Hash_djb2( const void* beg, const void* end, const void* except, size_t except_el_size );
void RecalcHash( stack_t* stk );
bool IsHashBad( const stack_t* stk );
#endif


#ifdef WINDOWS_SYS
bool CheckPointer( const void* ptr );
bool CheckAllStackPointers( stack_t* stk );
#endif


error_id StackVerifier( stack_t* stk );
#define StackAssert(STK) StackAssertF( STK ON_DEBUG(, __FILE__, __func__, __LINE__) )
error_id StackAssertF( stack_t* stk
             ON_DEBUG(, const char* file, const char* func, int line));
#define StackDump(STK, ERR) StackDumpF( STK, ERR ON_DEBUG(, file, func, line) )
void StackDumpF( stack_t* stk,  error_id error
      ON_DEBUG(, const char* file, const char* func, int line ));
#endif
