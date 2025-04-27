#include "stateMachine.h"
#include "gdma.h"

#ifndef	__RULETRANSFORM_H__
#define __RULETRANSFORM_H__

pmRule_t0** stateToRules(SM* sm, int* numOfRules);
void freeTransRules(pmRule_t0** transformedRules, int totalNumOfRules);
void showTransRules(pmRule_t0** transformedRules, int totalNumOfRules);
void WriteTransitionRulesToFile(char* filename, pmRule_t0** rules, int numOfRules);

#endif	/* __RULETRANSFORM_H__ */

