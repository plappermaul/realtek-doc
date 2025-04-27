#include "stateMachine.h"

#ifndef	__KMP_H__
#define	__KMP_H__

//int* failFunction(char* pattern);
SM* kmp_MakeStateMachine(char* pattern);
void showFailFunction(char* pattern);

#endif	/* __KMP_H__ */

