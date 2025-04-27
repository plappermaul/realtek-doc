#include "Th_state_to_sm.h"
#include "gdma_glue.h"

int ID = 0;
static int not_symbol;//For prefix feature regular expression. Once the char. match the Not transition rule in a state, FSM stops and return no match.
static int fail_state;/*For example, we have {a, b, c} char. space in a given pattern. Since there might be input chars outside {a,b,c},
	 	       we choose a char to represent chars not in {a, b,c}. This can simplify the output rules with the artificial favors.*/
static char NS;
void SetProperStateID(struct state_info* statelist)
{

	while(statelist){
		statelist->stateID = ID;
		ID++;
		statelist = statelist->nextstate;
	}
}
void setNotSymbol(char ns)
{
	not_symbol = 1;
	NS = ns;
}

void unsetNotSymbol()
{
	not_symbol = 0;
}

void setFailState()
{
	fail_state = 1;
}

void unsetFailState()
{
	fail_state = 0;
}
pSM Th_state_to_sm(struct state_info** statelist, int IstateID)
{
	int numOfRules;
	int numOfStates;
	int ruleIndex;
	int stateIndex;
	struct state_info *tmpslist;
	struct state_info *tmpPreIstate;
	struct state_info *tmpstate;
	ptrThStateLink tmpslink;
	pSM th_sm;
	State* tmpStateArray;

	tmpslist = (*statelist);
	numOfStates = 0;
	tmpPreIstate = NULL;
	tmpStateArray = NULL;
	while(tmpslist){
		if(tmpslist->nextstate){
			if(tmpslist->nextstate->stateID == IstateID){
				tmpPreIstate = tmpslist;
			}
		}
	//	if(tmpslist->isfinal != 1)
			numOfStates++;

		tmpslist = tmpslist->nextstate;
	}
	/*find the initial state and set it to the first place of the state list.*/
	if(tmpPreIstate!=NULL){/*swap initial state to be the head of  statelist.*/
		tmpstate = tmpPreIstate->nextstate;
		tmpPreIstate->nextstate = tmpPreIstate->nextstate->nextstate;
		tmpstate->nextstate = *statelist;
		*statelist = tmpstate;		
	}
	else{
		if((*statelist)->stateID != IstateID){
			rtlglue_printf("Error, Initial state is not found, function %s, line %d\n", __FUNCTION__, __LINE__);
		}
	}
	

	SetProperStateID(*statelist);//The first state of the list is the initial state.

	/*new SM*/
	th_sm = (pSM)rtlglue_malloc(sizeof(SM));
	if(!th_sm){
		rtlglue_printf("state machine rtlglue_malloc fail, function %s, line %d\n", __FUNCTION__, __LINE__);
	}
	/*set numOfState.*/
	th_sm->numOfStates = numOfStates;
	th_sm->states = (State**)rtlglue_malloc(((numOfStates+STATE_ARRAY_CAPACITY-1)/STATE_ARRAY_CAPACITY)*sizeof(State*));
	if(!th_sm->states){
		rtlglue_printf("states rtlglue_malloc fail, function %s, line %d\n", __FUNCTION__, __LINE__);
	}
	/*for each state, count numOfRules.*/
	tmpslist = *statelist;
	stateIndex = 0;
	while(tmpslist){
		if( stateIndex%STATE_ARRAY_CAPACITY == 0){
			tmpStateArray = (State*)rtlglue_malloc(STATE_ARRAY_CAPACITY*sizeof(State));
			if(tmpStateArray == NULL)
				rtlglue_printf("State rtlglue_malloc fail, function %s, line %d\n", __FUNCTION__, __LINE__);
			th_sm->states[stateIndex/STATE_ARRAY_CAPACITY] = tmpStateArray;
		}
		if(tmpslist->isfinal == 0){
			tmpslink = (ptrThStateLink)tmpslist->outlink;
			numOfRules = 0;
			while(tmpslink){
				if(numOfRules != 0){
					if(tmpslink->linkedState->stateID != IstateID ){
						numOfRules++;
					}
				}
				else{
					numOfRules++;
				}
				tmpslink = (ptrThStateLink)tmpslink->nextLink;
			}
			tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].stateID = tmpslist->stateID;
			tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].numOfRules = numOfRules;
			tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules = (Rule *)rtlglue_malloc(numOfRules*sizeof(Rule));
			if(!tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules){
				rtlglue_printf("Rules rtlglue_malloc fail, function %s, line %d\n", __FUNCTION__, __LINE__);
			}
			tmpslink = (ptrThStateLink)tmpslist->outlink;
			ruleIndex = 0;
			while(tmpslink){
				if(ruleIndex!=0){
					if(tmpslink->linkedState->stateID != IstateID){
						tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].cmpChar = tmpslink->symbol;
						tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].cpu = tmpslink->linkedState->isfinal;
						tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].not = 0;
					//	if(tmpslink->linkedState->isfinal == 1)//Modify jmpState to 0, if cpu == 1;
					//		tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].jmpState = 0;
					//	else
							tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].jmpState = tmpslink->linkedState->stateID;
						ruleIndex++;
					}
				}
				else{
					tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].cmpChar = tmpslink->symbol;
					tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].cpu = tmpslink->linkedState->isfinal;
					tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].not = 0;
					//if(tmpslink->linkedState->isfinal == 1)//Modify jmpState to 0, if cup == 1;
					//	tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].jmpState = 0;
					//else		
						tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules[ruleIndex].jmpState = tmpslink->linkedState->stateID;
					ruleIndex++;
				}
				tmpslink = (ptrThStateLink)tmpslink->nextLink;
			}
		}//end of if(tmpslist->isfinal == 0){
		else{//Final state, we don't generate its transitions.
			tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].stateID = tmpslist->stateID;
			tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].numOfRules = 0;
			tmpStateArray[stateIndex%STATE_ARRAY_CAPACITY].transRules = NULL;
		}
		stateIndex++;
		tmpslist = tmpslist->nextstate;
	}

	AddLastNotTrans(th_sm);

	return th_sm;
}

void AddLastNotTrans(pSM th_sm)
{
        int IstateID;
	Rule tmpRule;
	pRule newRules;
	int i,j,k,found;
	int numOfRules;
	State* tmpStateArray;
	int tmpStateArrayIndex;

        tmpStateArray = th_sm->states[0];
        IstateID = tmpStateArray[0].stateID;

        for(i=0;i<th_sm->numOfStates;i++){
		found =0;
		tmpStateArrayIndex = i%STATE_ARRAY_CAPACITY;
		tmpStateArray = th_sm->states[i/STATE_ARRAY_CAPACITY];
		if(tmpStateArray[tmpStateArrayIndex].numOfRules > 0){
if(not_symbol == 1){
			for(j=0;j<tmpStateArray[tmpStateArrayIndex].numOfRules;j++){//if cpu == 1, that is because we modify jmpState to 0;
				//if(tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar == 'N' || tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar == 'G'){
				if(tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar == NS ){
					found = 2;
					break;
				}			
			}
}
			if(found == 0){
				for(j=0;j<tmpStateArray[tmpStateArrayIndex].numOfRules;j++){//if cpu == 1, that is because we modify jmpState to 0;
					if(tmpStateArray[tmpStateArrayIndex].transRules[j].jmpState == IstateID && tmpStateArray[tmpStateArrayIndex].transRules[j].cpu == 0){
						found = 1;
						break;
					}			
				}
			}
			numOfRules = tmpStateArray[tmpStateArrayIndex].numOfRules;
			if(found == 1){
				if(j!=numOfRules-1){
				//	rtlglue_printf("state[%d] found j = %d\n",i,j);
					tmpRule.cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cmpChar;
					tmpRule.not = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].not;
					tmpRule.cpu = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cpu;
					tmpRule.jmpState = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].jmpState;
		
		 			//th_sm->states[i].transRules[numOfRules-1].cmpChar = th_sm->states[i].transRules[j].cmpChar;
					tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].not = 1;
if(fail_state == 1){
					tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cpu = 1;/*Change this for fail state*/
}
else{
					tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cpu = tmpStateArray[tmpStateArrayIndex].transRules[j].cpu;
}
					tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].jmpState = tmpStateArray[tmpStateArrayIndex].transRules[j].jmpState;

					tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar = tmpRule.cmpChar;
					tmpStateArray[tmpStateArrayIndex].transRules[j].not = tmpRule.not;
					tmpStateArray[tmpStateArrayIndex].transRules[j].cpu = tmpRule.cpu; 
					tmpStateArray[tmpStateArrayIndex].transRules[j].jmpState = tmpRule.jmpState;
				}
				else{
					if(j>0){// not first transition rules of the state.
						tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[j-1].cmpChar;
						tmpStateArray[tmpStateArrayIndex].transRules[j].not = 1;
					}
					else{//Can't happen.
						tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[j+1].cmpChar;
						tmpStateArray[tmpStateArrayIndex].transRules[j].not = 1;
					}
				}
			}
			else if(found == 2){
if(not_symbol == 1){
				if(j!=numOfRules-1){
				tmpRule.cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cmpChar;
				tmpRule.not = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].not;
				tmpRule.cpu = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cpu;
				tmpRule.jmpState = tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].jmpState;
		
	 			//th_sm->states[i].transRules[numOfRules-1].cmpChar = th_sm->states[i].transRules[j].cmpChar;
				tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].not = 1;
				tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].cpu = tmpStateArray[tmpStateArrayIndex].transRules[j].cpu;
				tmpStateArray[tmpStateArrayIndex].transRules[numOfRules-1].jmpState = tmpStateArray[tmpStateArrayIndex].transRules[j].jmpState;

				tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar = tmpRule.cmpChar;
				tmpStateArray[tmpStateArrayIndex].transRules[j].not = tmpRule.not;
				tmpStateArray[tmpStateArrayIndex].transRules[j].cpu = tmpRule.cpu; 
				tmpStateArray[tmpStateArrayIndex].transRules[j].jmpState = tmpRule.jmpState;
				}
				else{

					if(j>0){
						tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[j-1].cmpChar;
						tmpStateArray[tmpStateArrayIndex].transRules[j].not = 1;
					}
					else{//Can't happen.
						//rtlglue_printf("Worning! There's only one transition in the state with NOT_SYMBOL charactor.\n");
						tmpStateArray[tmpStateArrayIndex].transRules[j].cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[j+1].cmpChar;
						tmpStateArray[tmpStateArrayIndex].transRules[j].not = 1;
					}
				}
}
			}
			else{
			//	rtlglue_printf("state[%d] not found j = %d\n",i,j);
				newRules = (pRule)rtlglue_malloc((tmpStateArray[tmpStateArrayIndex].numOfRules+1)*sizeof(Rule));
				for(k=0;k<tmpStateArray[tmpStateArrayIndex].numOfRules;k++){
					newRules[k].cmpChar = tmpStateArray[tmpStateArrayIndex].transRules[k].cmpChar;
					newRules[k].not = tmpStateArray[tmpStateArrayIndex].transRules[k].not;
					newRules[k].cpu = tmpStateArray[tmpStateArrayIndex].transRules[k].cpu;
					newRules[k].jmpState = tmpStateArray[tmpStateArrayIndex].transRules[k].jmpState;
				}
				newRules[numOfRules].cmpChar = newRules[numOfRules-1].cmpChar;
				newRules[numOfRules].not = 1;
if(fail_state == 1){
				newRules[numOfRules].cpu = 1; /*Change this for fail state*/
}
else{
				newRules[numOfRules].cpu = 0;
}
				newRules[numOfRules].jmpState = IstateID;	
				tmpStateArray[tmpStateArrayIndex].numOfRules = numOfRules + 1;
				rtlglue_free(tmpStateArray[tmpStateArrayIndex].transRules);
				tmpStateArray[tmpStateArrayIndex].transRules = newRules;		
			}
		}
	}
}
