#include "gdma.h"

#ifndef	__LAYER7_H__
#define	__LAYER7_H__

int gdmaRegExec(stateMachine_t* gdmaStateMachine, uint8* data,uint32 dataLength);
stateMachine_t* gdmaReadCompFile(uint8* filename);
void gdmaFreeStateMachine(stateMachine_t* sm);
#endif	/* __LAYER7_H__ */

