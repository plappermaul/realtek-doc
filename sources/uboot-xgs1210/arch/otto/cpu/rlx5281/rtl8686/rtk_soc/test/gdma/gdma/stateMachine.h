//#include <stdlib.h>

#ifndef	__STATEMACHINE_H__
#define	__STATEMACHINE_H__

#define STATE_ARRAY_CAPACITY 512
/********************
 * Transcation Rule *
 ********************/
struct rule
{
  char cmpChar;
  int not;
  int cpu;
  int jmpState;
};
typedef struct rule Rule;
typedef struct rule* pRule;


/**********************
 *     State Data     *
 **********************/
struct state
{
  int stateID;
  int numOfRules;
  Rule *transRules;
};
typedef struct state State;
typedef struct state* pState;


/**********************
 * State Machine Data *
 **********************/
struct stateMachine
{
  int numOfStates;
  State **states;
};
typedef struct stateMachine SM;
typedef struct stateMachine* pSM;


void showStateMachine(SM* sm);
void showStateMachineNonChar(SM* sm);
int countStateMachineRules(SM* sm);
void freeStateMachine(SM* sm);

#endif	/* __STATEMACHINE_H__ */

