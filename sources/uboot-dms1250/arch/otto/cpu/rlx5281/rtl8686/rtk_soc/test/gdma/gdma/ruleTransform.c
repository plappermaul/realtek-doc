#include "gdma_glue.h"
#include "ruleTransform.h"
#include "gdma.h"
#define UNCACHED_ADDRESS(x) ((void *)(0x20000000 | (uint32)x ))
#define PHYSICAL_ADDRESS(x) (((uint32)x) & 0x1fffffff

uint32*	assignStateLocation(SM* sm);

void WriteTransitionRulesToFile(char* filename, pmRule_t0** rules, int numOfRules)
{
        int fp;
        int i;
        pmRule_t0* tmpRuleArray;
        pmRule_t0* tmpRule;
        uint8 *tt;
        uint8 b1, b2, b3, b4;
        uint8 n,c;
        fp = rtlglue_open( filename, O_RDWR | O_CREAT, 0666);
        if ( fp<0 ){
                rtlglue_printf("open %s file error!!\n", filename);
        }
        else{
                for(i=0; i< numOfRules; i++){
                        tmpRuleArray = rules[(i/256)];
                        tt = (char*)&tmpRuleArray[(i%256)];
                        b1 = tt[0];
                        b2 = tt[1]<<6;
                        n = tt[1] & 4;
                        n = n << 3;
                        c = tt[1] & 8;
                        c = c << 1;
                        b2 = b2 | n;
                        b2 = b2 | c;
                        b2 = b2 | (tt[3] >> 4);
                        b3 = tt[2];
                        b3 = b3 >> 4;
                        b3 = b3 | (tt[3] << 4);
                        b4 = tt[1]>>4;
                        b4 = b4 | (tt[2]<<4);
                        tt[0] = b1;
                        tt[1] = b2;
                        tt[2] = b3;
                        tt[3] = b4;
                        rtlglue_write(fp, tt, sizeof(pmRule_t0));
                }
        }
        rtlglue_close(fp);
}


void freeTransRules(pmRule_t0** transformedRules, int totalNumOfRules)
{
	int ruleArrayNum;
	int i;

	ruleArrayNum = (totalNumOfRules+INTERNAL_RULE_NUMBER-1)/INTERNAL_RULE_NUMBER;
	for(i=0; i<ruleArrayNum; i++){
		rtlglue_free(transformedRules[i]);
	}
	rtlglue_free(transformedRules);
}

void showTransRules(pmRule_t0** transformedRules, int totalNumOfRules)
{
	int i;
	int tmpTransArrayIndex;
	uint32 *u;
	pmRule_t0* tmpTransArray;
	
	tmpTransArrayIndex=0;
	for(i=0;i<totalNumOfRules;i++){
		if(i%INTERNAL_RULE_NUMBER == 0){
			tmpTransArray = transformedRules[i/INTERNAL_RULE_NUMBER];
		}
		u =(uint32*)(&tmpTransArray[tmpTransArrayIndex]); 
	//	rtlglue_printf("%u ",*u);
		rtlglue_printf("%d matchChar: %c ",i,tmpTransArray[tmpTransArrayIndex].matchChar);
		rtlglue_printf("cpu: %d ",tmpTransArray[tmpTransArrayIndex].cpu);
		rtlglue_printf("type: %d ",tmpTransArray[tmpTransArrayIndex].type);
		rtlglue_printf("not: %d ",tmpTransArray[tmpTransArrayIndex].not);
		rtlglue_printf("jmpState: %u\n",tmpTransArray[tmpTransArrayIndex].jmpState);
		tmpTransArrayIndex = (tmpTransArrayIndex+1)%INTERNAL_RULE_NUMBER;	
	}
}

pmRule_t0** stateToRules(SM* sm, int* numOfRules)
{
	int totalNumRules;
	uint32* stateLocation;
	int ruleIndex, i, j, l;
	int numOfsubState;
	State* tmpState;
	pmRule_t0** transformedRules;
	pmRule_t0** cachedTransformedRules;
	pmRule_t0* pRule;

	//count total number and malloc memory of the rules.
	totalNumRules = countStateMachineRules(sm);
	*numOfRules = totalNumRules;
	numOfsubState = (totalNumRules+(INTERNAL_RULE_NUMBER-1))/(INTERNAL_RULE_NUMBER);
	if(numOfsubState>SUB_STATE_MACHINE_NUMBER)
		rtlglue_printf("Woring! Number of subState>SUB_STATE_MACHINE_NUMBER(%d). Number of subState = %d\n",SUB_STATE_MACHINE_NUMBER ,numOfsubState);
	cachedTransformedRules = NULL;
	pRule = NULL;
#if 0
#ifdef RTL865X_MODEL_KERNEL
	cachedTransformedRules = (pmRule_t0*)rtlglue_malloc(totalNumRules*sizeof(pmRule_t0));
	if(cachedTransformedRules == NULL){
		rtlglue_printf("cachedTransformedRules malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out0;
	}
	transformedRules = UNCACHED_ADDRESS(cachedTransformedRules);
#else
	transformedRules = (pmRule_t0*)rtlglue_malloc(totalNumRules*sizeof(pmRule_t0));
	if(transformedRules == NULL){
		rtlglue_printf("transformedRules malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out0;
	}
#endif

#endif

#ifdef RTL865X_MODEL_KERNEL
        cachedTransformedRules = (pmRule_t0**)rtlglue_malloc(numOfsubState*sizeof(pmRule_t0*));
        if(cachedTransformedRules == NULL){
		rtlglue_printf("cachedTransformedRules malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out0;
	}
        transformedRules = UNCACHED_ADDRESS(cachedTransformedRules);
#else
	transformedRules = (pmRule_t0**)rtlglue_malloc(numOfsubState*sizeof(pmRule_t0*));
	//rtlglue_printf("transformed malloc: size=%d point=%p\n", (numOfsubState*sizeof(pmRule_t0*)), transformedRules );
	if(transformedRules == NULL){
                rtlglue_printf("transformedRules malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out0;
        }
#endif
	//assign location to each state.
	
        stateLocation = assignStateLocation(sm);
	if(stateLocation == NULL){
		rtlglue_printf("assignStateLocation fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out1;
	}
	
	//Transform states into gdma pattern match rule format.
	ruleIndex = 0;
	l = -1;
	tmpState = NULL;
	for( i=0 ; i < sm->numOfStates ; i++){
		if(i%STATE_ARRAY_CAPACITY == 0){
			tmpState = sm->states[i/STATE_ARRAY_CAPACITY];
		}
		for( j=0; j < tmpState[i%STATE_ARRAY_CAPACITY].numOfRules ; j++){
			if(ruleIndex == 0){
				pRule = NULL;
				pRule = (pmRule_t0*)rtlglue_malloc((INTERNAL_RULE_NUMBER)*sizeof(pmRule_t0));
			//	rtlglue_printf("pRule malloc[%d]: size=%d point=%p\n",l+1,  (256*sizeof(pmRule_t0)), pRule );
			/*	if(l>=0){
					//rtlglue_printf("free transformedRules[%d] = %p\n",l , transformedRules[l]);
					rtlglue_free(transformedRules[l]);
				}*/
				if(pRule==NULL){
					rtlglue_printf("pRule malloc fail, l = %d, Function = %s, line = %d\n", l,  __FUNCTION__, __LINE__);
					goto out2;
				}
				l++;
				transformedRules[l] = pRule;
				//rtlglue_free(pRule);
			//	return transformedRules;

#ifdef RTL865X_MODEL_KERNEL
				pRule = UNCACHED_ADDRESS(pRule);
#endif
			}
		//	if(j == (tmpState[i%STATE_ARRAY_CAPACITY].numOfRules-1)){// reverse last two rules to improve performacnce
		//		pRule[ruleIndex].matchChar = pRule[ruleIndex-1].matchChar;
		//		pRule[ruleIndex].cpu = pRule[ruleIndex-1].cpu;
		//		pRule[ruleIndex].type = pRule[ruleIndex-1].type;
		//		pRule[ruleIndex].not = pRule[ruleIndex-1].not;
		//		pRule[ruleIndex].jmpState = pRule[ruleIndex-1].jmpState;

		//		pRule[ruleIndex-1].not = 1;
		//		pRule[ruleIndex-1].cpu = 0;
		//		pRule[ruleIndex-1].jmpState = s1tateLocation[tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].jmpState];
		//	}
		//	else{
				pRule[ruleIndex].matchChar = tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].cmpChar;
				pRule[ruleIndex].cpu = tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].cpu;
				pRule[ruleIndex].type = 0;
				pRule[ruleIndex].not = tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].not;
				pRule[ruleIndex].jmpState = stateLocation[tmpState[i%STATE_ARRAY_CAPACITY].transRules[j].jmpState];
		//	}
			ruleIndex = (ruleIndex+1)%(INTERNAL_RULE_NUMBER);
		}
	}
 
 /*	for(i=0;i<=l;i++){
		rtlglue_printf("free i = %d, ptr = %p\n",i,transformedRules[i]);
		rtlglue_free(transformedRules[i]);
	        transformedRules[i]=NULL;
	}*/

	rtlglue_free(stateLocation);
	stateLocation = NULL;

#ifdef RTL865X_MODEL_KERNEL
	return cachedTransformedRules;
#else
	return transformedRules;
#endif

out2:
	rtlglue_free(stateLocation);
	stateLocation = NULL;
	for(i=0;i<=l;i++){
		rtlglue_free(transformedRules[i]);
		transformedRules[i]=NULL;
	}
out1:
#ifdef RTL865X_MODEL_KERNEL
	rtlglue_free(cachedTransformedRules);
	cachedTransformedRules = NULL;
#else
	rtlglue_free(transformedRules);
	transformedRules = NULL;
#endif
out0:
	return NULL;
}

uint32* assignStateLocation(SM* sm)
{
	uint32* stateLocation;
	uint32 i;
	//State* tmpState;
	uint32 maxStateID;
	
	maxStateID = 0;
	for(i=0; i< sm->numOfStates; i++){
		if((sm->states[i/STATE_ARRAY_CAPACITY])[i%STATE_ARRAY_CAPACITY].stateID > maxStateID)
			maxStateID = (sm->states[i/STATE_ARRAY_CAPACITY])[i%STATE_ARRAY_CAPACITY].stateID;	
	}

	stateLocation = (uint32*)malloc(maxStateID*sizeof(int));

	if(stateLocation == NULL){
		rtlglue_printf("stateLocation malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	memset((void *)stateLocation, maxStateID*sizeof(int), 0);	

	stateLocation[(sm->states[0/STATE_ARRAY_CAPACITY])[0%STATE_ARRAY_CAPACITY].stateID] = 0;
	//tmpState = sm->states[0];
	for( i=1; i < sm->numOfStates ; i++){
	//	if(i%STATE_ARRAY_CAPACITY==0){
			stateLocation[(sm->states[i/STATE_ARRAY_CAPACITY])[i%STATE_ARRAY_CAPACITY].stateID] = stateLocation[(sm->states[(i-1)/STATE_ARRAY_CAPACITY])[(i-1)%STATE_ARRAY_CAPACITY].stateID] + (sm->states[(i-1)/STATE_ARRAY_CAPACITY])[(i-1)%STATE_ARRAY_CAPACITY].numOfRules;
			//tmpState = sm->states[i/STATE_ARRAY_CAPACITY];	
	//	}
	//	else{
	//		stateLocation[] = stateLocation[i-1] + tmpState[i%STATE_ARRAY_CAPACITY-1].numOfRules; 		
	//	}
	}
        return stateLocation;
}
