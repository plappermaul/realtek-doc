//#include <linux/slab.h>
#include "stateMachine.h"
#include "gdma_glue.h"

void showStateMachine(SM* sm)
{
   int i, j;
   State* tmpState;

   tmpState = NULL;
   for( i=0 ; i < sm->numOfStates ; i++ ){
	if(i%STATE_ARRAY_CAPACITY==0){
		tmpState = sm->states[i/STATE_ARRAY_CAPACITY];
	}
         rtlglue_printf("state %d \n", tmpState[i%STATE_ARRAY_CAPACITY].stateID);
         for( j=0 ; j< tmpState[i%STATE_ARRAY_CAPACITY].numOfRules ; j++ ){
               rtlglue_printf("Rules[%d] ",j);
               rtlglue_printf(" char = %c,", tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].cmpChar);
               rtlglue_printf(" jmpState = %d,", tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].jmpState);
               rtlglue_printf(" not = %d," , tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].not);
               rtlglue_printf(" cpu = %d \n", tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].cpu);
         }
   }
}
void showStateMachineNonChar(SM* sm)
{
        int i, j;
        State* tmpState;

        tmpState = NULL;
        for( i=0 ; i < sm->numOfStates ; i++ ){
                if(i%STATE_ARRAY_CAPACITY==0){
                        tmpState = sm->states[i/STATE_ARRAY_CAPACITY];
                }
                rtlglue_printf("state %d \n", tmpState[i%STATE_ARRAY_CAPACITY].stateID);
                for( j=0 ; j< tmpState[i%STATE_ARRAY_CAPACITY].numOfRules ; j++ ){
                        rtlglue_printf("Rules[%d] ",j);
                        rtlglue_printf(" char = %x,", (unsigned char)tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].cmpChar);
                        rtlglue_printf(" jmpState = %d,",tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].jmpState);
                        rtlglue_printf(" not = %d," , tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].not);
                        rtlglue_printf(" cpu = %d \n", tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].cpu);
                }
        }
}


void freeStateMachine(SM* sm)
{
	int i;
	State* tmpState;
	
	tmpState = NULL;
	for(i=0; i < sm->numOfStates ; i++){
		if(i%STATE_ARRAY_CAPACITY==0){
			tmpState = sm->states[i/STATE_ARRAY_CAPACITY];
		}
		rtlglue_free(tmpState[i%STATE_ARRAY_CAPACITY].transRules);
		if((i%STATE_ARRAY_CAPACITY) == (STATE_ARRAY_CAPACITY-1)|| i == sm->numOfStates-1)
			rtlglue_free(tmpState);
	}
	rtlglue_free(sm->states);
	rtlglue_free(sm);
}

int countStateMachineRules(SM* sm)
{
        int i;
	int totalRules;
	State* tmpState;
	
	tmpState = NULL;
	totalRules = 0;
        for(i=0; i < sm->numOfStates ; i++){
		if((i%STATE_ARRAY_CAPACITY) == 0)
			tmpState = sm->states[(i/STATE_ARRAY_CAPACITY)];
		totalRules += tmpState[(i%STATE_ARRAY_CAPACITY)].numOfRules;
	}
        return totalRules;
}
