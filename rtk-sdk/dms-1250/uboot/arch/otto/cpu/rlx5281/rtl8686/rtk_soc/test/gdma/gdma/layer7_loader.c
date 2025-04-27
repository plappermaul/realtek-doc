#include "layer7.h"
#include "rtl_glue.h"
#include "rtl865xc_asicregs.h"
#include "gdma.h"
#include "rtl_glue.h"
#define UNCACHED_ADDRESS(x) ((void *)(0x20000000 | (uint32)x ))
#define PHYSICAL_ADDRESS(x) (((uint32)x) & 0x1fffffff)

void _gdmaLoad(descriptor_t* sourceData, int numOfSourceBlock, stateMachine_t* sm, uint32 beginningRule)
{
        int i;

        WRITE_MEM32( GDMACNR, 0 );
        WRITE_MEM32( GDMACNR, GDMA_ENABLE );
        WRITE_MEM32( GDMAIMR, 0 );
        WRITE_MEM32( GDMAISR, 0xffffffff );
        WRITE_MEM32( GDMAICVL, beginningRule );//Beginning rule index
        WRITE_MEM32( GDMAICVR, 0);   //The source length that has already processed.

        for( i= 0 ; i<8 && i<numOfSourceBlock ; i++){//Setting source data blocks.
                WRITE_MEM32( (GDMASBP0)+i*8, (uint32)(sourceData[i].pData) );
                WRITE_MEM32( (GDMASBL0)+i*8, (uint32)((sourceData[i].ldb<<31)|sourceData[i].length) );
        }

        WRITE_MEM32( GDMADBP0, (uint32)sm );
}

int _gdmaRun(uint32* nextRuleIndex, uint32* doneLength)
{

        WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
        while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */


        *doneLength = READ_MEM32(GDMAICVR);
        *nextRuleIndex = READ_MEM32(GDMAICVL);
        //There is an occurance.
        if((READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP)==GDMA_NEEDCPUIP){
                return 0;
        }
        //There is no occurance.
        else{
                return 1;
        }
}

int gdmaRegExec(stateMachine_t* gdmaStateMachine, uint8* data,uint32 dataLength)
{
        uint32 nextRuleIndex;
        uint32 doneLength;
        uint32 needCPU;
        descriptor_t descriptorBlock[8];

        needCPU=0;
        nextRuleIndex = 0;
        descriptorBlock[0].pData = data;
        descriptorBlock[0].ldb = 1;
        descriptorBlock[0].length = dataLength;
        doneLength = 0;
        _gdmaLoad(descriptorBlock, 1, gdmaStateMachine, 0);
        needCPU = _gdmaRun(&nextRuleIndex, &doneLength);

        if(needCPU == 0){
                if(nextRuleIndex==0)//no match
                        return 0;
                else//match
                        return 1;
        }
        return 0;//no match
}

stateMachine_t* gdmaReadCompFile(uint8* filename)
{
	int numOfRules;
	int fp;
	int transRulesIndex;
	int tmpRuleArrayIndex;
	stateMachine_t* cachedGdmaStateMachine;
	stateMachine_t* gdmaStateMachine;
	pmRule_t0* tmpRuleArray;
	pmRule_t0 ruleBuf;
		
	cachedGdmaStateMachine = (stateMachine_t*)rtlglue_malloc(sizeof(stateMachine_t));
	if(cachedGdmaStateMachine==NULL){
		rtlglue_printf("stateMachine malloc fail,  Function = %s, line = %d\n",   __FUNCTION__, __LINE__);
		return NULL;
	}
        gdmaStateMachine = UNCACHED_ADDRESS(cachedGdmaStateMachine);
	      
	fp = rtlglue_open(filename, 0, 0);
        if (fp<=0){
                rtlglue_printf("fp = %d, file open fail\n",fp);
		cachedGdmaStateMachine->cntTotalRules = 0;
		gdmaFreeStateMachine(cachedGdmaStateMachine);
		return NULL;
        }
        
        numOfRules = 0;
        transRulesIndex = 0;
        tmpRuleArrayIndex = 0;
        tmpRuleArray = NULL;

        while(rtlglue_read( fp, &ruleBuf, 4 )){
                if(tmpRuleArrayIndex%(INTERNAL_RULE_NUMBER) == 0){
                        tmpRuleArray = (pmRule_t0*)rtlglue_malloc((INTERNAL_RULE_NUMBER)*sizeof(pmRule_t0));
                        if(tmpRuleArray==NULL){
                        		rtlglue_printf("pmRule_t0 malloc fail, Function = %s, line = %d\n",  __FUNCTION__, __LINE__);
					cachedGdmaStateMachine->cntTotalRules = numOfRules;
					gdmaFreeStateMachine(cachedGdmaStateMachine);
			}
                        gdmaStateMachine->subsm[transRulesIndex] = (subStateMachine_t*)tmpRuleArray;
                        tmpRuleArray = UNCACHED_ADDRESS(tmpRuleArray);
                        transRulesIndex++;
                }
                tmpRuleArray[tmpRuleArrayIndex].matchChar = ruleBuf.matchChar;
                tmpRuleArray[tmpRuleArrayIndex].not = ruleBuf.not;
                tmpRuleArray[tmpRuleArrayIndex].cpu = ruleBuf.cpu;
                tmpRuleArray[tmpRuleArrayIndex].jmpState = ruleBuf.jmpState;
                tmpRuleArray[tmpRuleArrayIndex].type = ruleBuf.type;
                tmpRuleArrayIndex = (tmpRuleArrayIndex+1)%(INTERNAL_RULE_NUMBER);
                numOfRules++;
        }
        gdmaStateMachine->cntTotalRules = numOfRules;
        rtlglue_close(fp);

	return cachedGdmaStateMachine;
}

void gdmaFreeStateMachine(stateMachine_t* sm)
{
        int ruleArrayNum;
        int i;
				
	if(sm==NULL)
		return;
        ruleArrayNum = (sm->cntTotalRules+INTERNAL_RULE_NUMBER-1)/INTERNAL_RULE_NUMBER;
        for(i=0; i<ruleArrayNum; i++){
                rtlglue_free(sm->subsm[i]->rules);
        }
        rtlglue_free(sm);
}
