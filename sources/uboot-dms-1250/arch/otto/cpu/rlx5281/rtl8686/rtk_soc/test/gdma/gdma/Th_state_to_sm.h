#ifndef __TH_STATE_TO_SM_H__
#define __TH_STATE_TO_SM_H__

#include "stateMachine.h"
#include "Th_Auto.h"
void AddLastNotTrans(pSM th_sm);
pSM Th_state_to_sm(struct state_info **statelist, int IstateID);
void setFailState(void);
void unsetFailState(void);
void setNotSymbol(char ns);
void unsetNotSymbol(void);
#endif
