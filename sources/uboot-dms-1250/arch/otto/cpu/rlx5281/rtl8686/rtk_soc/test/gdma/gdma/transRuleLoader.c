#include "rtl_glue.h"
#include "transRuleLoader.h"
#include "rtl865xc_asicregs.h"
#include "gdma.h"
#include "rtl_glue.h"
#define UNCACHED_ADDRESS(x) ((void *)(0x20000000 | (uint32)x ))
#define PHYSICAL_ADDRESS(x) (((uint32)x) & 0x1fffffff)


int transRun(uint32* nextRuleIndex, uint32* doneLength);
void transLoad(descriptor_t* sourceData, int numOfSourceBlock, stateMachine_t* sm, uint32 beginningRule);

/*Software pattern matching algorithms performance testing*/
uint32* PMAlgoTest(char** patterns, int numOfPatterns, descriptor_t* cmpText, int numOfCmpText, void *pAlogExec, void *pAlogPreprocessing,void  *pAlgoCleanup,int runTimes)
{
	int (*preprocessing)(char**, int);
	void (*cleanup)(void);
	uint32 (*algoExec)(int *,descriptor_t *, int); 
	uint32 totalLength;
	uint32 *occurArray;
	uint32 occurrance;
	uint32 occurIndex;
        uint32 msStop;
	uint32 msStart;
	uint32 cmpTextLocation;
	uint32 i;
	int rt;

	preprocessing = pAlogPreprocessing;
	algoExec = pAlogExec;
	cleanup = pAlgoCleanup;

	occurIndex = 0;
	if(preprocessing(patterns , numOfPatterns)!= 1){
		rtlglue_printf("preprocessing fail!\n");
		return NULL;
	}
	occurArray = NULL;

	totalLength = 0;
	for(i = 0; i<numOfCmpText; i++){
		totalLength = totalLength + cmpText[i].length;	
	}
	/*occurArray = rtlglue_malloc(256*sizeof(uint32));
	if(!occurArray)
		rtlglue_printf("occurence malloc fail!, function %s, line %d\n", __FUNCTION__, __LINE__);
	*/
	rtlglue_getmstime( &msStart );
	for(rt=0; rt<runTimes; rt++){
		occurIndex = 0;
		cmpTextLocation = 0;
		algoExec(NULL,NULL,0);
		while(cmpTextLocation < totalLength){
			occurrance = algoExec(&cmpTextLocation, cmpText, 1);
			if(occurrance>0){
				//occurArray[occurIndex] = occurrance;
				occurIndex++;
			}
			/*if(occurIndex < 255){
				if(occurrance>0){
					occurArray[occurIndex] = occurrance;
					occurIndex++;
				}
			}*/
		}
	}
	rtlglue_getmstime( &msStop );
        if ( (msStop-msStart) > 0 ){
		//rtlglue_printf("pattern_num\toccurrence\ttime(ms)\tspeed(MBps)\n");
		rtlglue_printf("%d\t%u\t%u\t%u.%02u\n",numOfPatterns, occurIndex, msStop-msStart, 
				(((totalLength*runTimes)>>20)*1000)/(msStop-msStart), (((((totalLength*runTimes)>>20)*1000)%(msStop-msStart))*100/(msStop-msStart)));
	}
	else{
		rtlglue_printf("msStop-msStart < 0\n");
        }

	occurArray[occurIndex] = 0xffffffff;
	cleanup();
	return occurArray;
}

/*************************************************************
 * Return value: NULL for no match, others for some ocurrences.
 * At most 1024 locations of the occurrences will be return.
 *************************************************************/
uint32* exeTransRules(pmRule_t0** transRules, int numOfRules, descriptor_t* cachedCmpText, int numOfcmpText, int runTimes)
{
	int numOfSubStates;
	int ocurrIndex;
	int i;
	int cpRuleIndex;
	int rt;
	uint32* ocurrAdd;
	stateMachine_t* gdmaStateMachine;
	uint32 nextRuleIndex;
	uint32 doneLength;
	uint32 totalDoneLength;
	uint32 needCPU;
	uint32 totalSourceLength;
	uint32 doneStartOffset;
	uint32 doneSourceIndex;
	uint32 startOffset;
	uint32 sourceIndex;
	uint32 descriptorIndex;
	descriptor_t* descriptorBlock;
        uint32 msStop;
	uint32 msStart;
	descriptor_t* cmpText;
	//descriptor_t* cachedDescriptorBlock;
	descriptor_t scachedDescriptorBlock;
	//stateMachine_t* cachedGdmaStateMachine;
	stateMachine_t scachedGdmaStateMachine;
	//Transfor to hardware finite state machine data
	cmpText = NULL;
	cachedDescriptorBlock = NULL;
	cachedGdmaStateMachine = NULL;
	ocurrIndex = 0;
#ifdef RTL865X_MODEL_KERNEL
//	cachedGdmaStateMachine = (stateMachine_t*)rtlglue_malloc(1*sizeof(stateMachine_t));
	cachedGdmaStateMachine = (stateMachine_t*)&scachedGdmaStateMachine;
	gdmaStateMachine = UNCACHED_ADDRESS(cachedGdmaStateMachine);
//	cachedDescriptorBlock = (descriptor_t*)rtlglue_malloc(8*sizeof(descriptor_t));
	cachedDescriptorBlock = (descriptor_t*)&scachedDescriptorBlock;
	descriptorBlock = (descriptor_t*)UNCACHED_ADDRESS(cachedDescriptorBlock);
#else
	gdmaStateMachine = (stateMachine_t*)rtlglue_malloc(1*sizeof(stateMachine_t));
	descriptorBlock  = (descriptor_t*)rtlglue_malloc(8*sizeof(descriptor_t));
#endif
	numOfSubStates = (numOfRules+255)/256;
	gdmaStateMachine->subsm[numOfSubStates] = NULL;
	cpRuleIndex = 0;
	for( i = 0; i<numOfSubStates ; i++){
#ifdef RTL865X_MODEL_KERNEL
		gdmaStateMachine->subsm[i] =  (subStateMachine_t*)UNCACHED_ADDRESS(transRules[i]);
		//gdmaStateMachine->subsm[i] =  (subStateMachine_t*)transRules[i];
#else
		gdmaStateMachine->subsm[i] =  (subStateMachine_t*)(transRules[i]);
#endif
	}
	gdmaStateMachine->cntTotalRules = numOfRules;
	totalSourceLength = 0;
#ifdef RTL865X_MODEL_KERNEL	
	cmpText = UNCACHED_ADDRESS(cachedCmpText);
#else
	cmpText = cachedCmpText;
#endif
	for(i=0;i<numOfcmpText;i++){//count total source length
		totalSourceLength = totalSourceLength + cmpText[i].length;
	}

	ocurrAdd = (uint32*)rtlglue_malloc(1025*sizeof(uint32));
	rtlglue_getmstime( &msStart );
	for(rt = 0;rt < runTimes; rt++){
		nextRuleIndex = 0;
		doneStartOffset=0;
		doneSourceIndex=0;
		totalDoneLength = 0;
		ocurrIndex = 0;
		while(totalSourceLength > totalDoneLength){
			descriptorIndex = 0;
			startOffset = doneStartOffset;
			sourceIndex = doneSourceIndex;
			while(descriptorIndex<8){
				descriptorBlock[descriptorIndex].pData = cmpText[sourceIndex].pData + startOffset;
				descriptorBlock[descriptorIndex].ldb = 0;
				if((startOffset+0x1fff)>=cmpText[sourceIndex].length){
					descriptorBlock[descriptorIndex].length = cmpText[sourceIndex].length - startOffset;
					if(sourceIndex+1 == numOfcmpText || descriptorIndex == 7){
						descriptorBlock[descriptorIndex].ldb = 1;
						break;
					}
					sourceIndex++;
					startOffset=0;
				}
				else{
					descriptorBlock[descriptorIndex].length = 0x1fff;
					startOffset = startOffset + 0x1fff;
					if(descriptorIndex == 7){
						descriptorBlock[descriptorIndex].ldb = 1;
						break;
					}
				}
				descriptorIndex++;
			}
			//transLoad(descriptorBlock, descriptorIndex+1, gdmaStateMachine, nextRuleIndex);
			transLoad(descriptorBlock, descriptorIndex+1, gdmaStateMachine, 0);
			doneLength = 0;
			needCPU = transRun(&nextRuleIndex, &doneLength);
			totalDoneLength = totalDoneLength + doneLength;
			if(needCPU == 0){//There is an occurance.
				//if(ocurrIndex < 1024){
				//	ocurrAdd[ocurrIndex] = totalDoneLength;
					ocurrIndex++;
					if(nextRuleIndex==0){//fail to match the pattern, complete.
						totalDoneLength = totalSourceLength;
						break;
					}
				//}
			}	
			while(doneLength>0){
				if(doneStartOffset+doneLength >= cmpText[doneSourceIndex].length){
					doneLength = doneLength + doneStartOffset - cmpText[doneSourceIndex].length; 
					doneStartOffset = 0;
					doneSourceIndex++;
				}
				else{
					doneStartOffset = doneStartOffset + doneLength;
					doneLength = 0;
				}
			}
		}
	}
	rtlglue_getmstime( &msStop );

        if ( (msStop-msStart) > 0 ){
		rtlglue_printf("%u\t%u\t%u\t%d.%02d\n", ocurrIndex, numOfRules, msStop-msStart, 
				(((totalSourceLength*runTimes)>>20)*1000)/(msStop-msStart), ((((totalSourceLength*runTimes)>>10)%(msStop-msStart))*100/(msStop-msStart)));
	}
	else{
		rtlglue_printf("%u\t%u\t%u\t%dMBps\n", ocurrIndex, numOfRules , 0, 0);
        }
	ocurrAdd[ocurrIndex] = 0xffffffff;
#ifdef RTL865X_MODEL_KERNEL
	rtlglue_free(cachedGdmaStateMachine);
	rtlglue_free(cachedDescriptorBlock);
#else
	rtlglue_free(gdmaStateMachine);
	rtlglue_free(descriptorBlock);
#endif
	return ocurrAdd;
}
void transLoad(descriptor_t* sourceData, int numOfSourceBlock, stateMachine_t* sm, uint32 beginningRule)
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
/*Trigger the GDMA FSM*/
int transRun(uint32* nextRuleIndex, uint32* doneLength)
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
/*Loading program for the patterns with prefix features.*/
int prefixExeTransRules(pmRule_t0** transRules, int numOfRules, descriptor_t* cachedCmpText, int numOfcmpText, int runTimes)
{
	int numOfSubStates;
	int i;
	int cpRuleIndex;
	int rt;
	stateMachine_t* gdmaStateMachine;
	uint32 nextRuleIndex;
	uint32 doneLength;
	uint32 totalDoneLength;
	uint32 needCPU;
	uint32 totalSourceLength;
	uint32 descriptorIndex;
	descriptor_t* descriptorBlock;
	descriptor_t* cmpText;
	descriptor_t* cachedDescriptorBlock;
	stateMachine_t* cachedGdmaStateMachine;
	uint32 doneStartOffset;
	uint32 doneSourceIndex;
	uint32 startOffset;
	uint32 sourceIndex;
	
	//Transfor to hardware finite state machine data
	cmpText = NULL;
	cachedDescriptorBlock = NULL;
	cachedGdmaStateMachine = NULL;
#ifdef RTL865X_MODEL_KERNEL
	cachedGdmaStateMachine = (stateMachine_t*)rtlglue_malloc(1*sizeof(stateMachine_t));
	gdmaStateMachine = UNCACHED_ADDRESS(cachedGdmaStateMachine);
	cachedDescriptorBlock = (descriptor_t*)rtlglue_malloc(8*sizeof(descriptor_t));
	descriptorBlock = (descriptor_t*)UNCACHED_ADDRESS(cachedDescriptorBlock);
#else
	gdmaStateMachine = (stateMachine_t*)rtlglue_malloc(1*sizeof(stateMachine_t));
	descriptorBlock  = (descriptor_t*)rtlglue_malloc(8*sizeof(descriptor_t));
#endif
	numOfSubStates = (numOfRules+255)/256;
	gdmaStateMachine->subsm[numOfSubStates] = NULL;
	cpRuleIndex = 0;
	for( i = 0; i<numOfSubStates ; i++){
#ifdef RTL865X_MODEL_KERNEL
		gdmaStateMachine->subsm[i] =  (subStateMachine_t*)transRules[i];
#else
		gdmaStateMachine->subsm[i] =  (subStateMachine_t*)(transRules[i]);
#endif
	}
	gdmaStateMachine->cntTotalRules = numOfRules;
	totalSourceLength = 0;
#ifdef RTL865X_MODEL_KERNEL	
	cmpText = UNCACHED_ADDRESS(cachedCmpText);
#else
	cmpText = cachedCmpText;
#endif
	for(i=0;i<numOfcmpText;i++){//count total source length
		totalSourceLength = totalSourceLength + cmpText[i].length;
	}
	needCPU=0;
		nextRuleIndex = 0;
		doneStartOffset=0;
		doneSourceIndex=0;
		totalDoneLength = 0;
		descriptorIndex = 0;
		startOffset = doneStartOffset;
		sourceIndex = doneSourceIndex;
		while(descriptorIndex<8){
			descriptorBlock[descriptorIndex].pData = cmpText[sourceIndex].pData + startOffset;
			descriptorBlock[descriptorIndex].ldb = 0;
			if((startOffset+0x1fff)>=cmpText[sourceIndex].length){
				descriptorBlock[descriptorIndex].length = cmpText[sourceIndex].length - startOffset;
				if(sourceIndex+1 == numOfcmpText || descriptorIndex == 7){
					descriptorBlock[descriptorIndex].ldb = 1;
					break;
				}
				sourceIndex++;
				startOffset=0;
			}
			else{
				descriptorBlock[descriptorIndex].length = 0x1fff;
				startOffset = startOffset + 0x1fff;
				if(descriptorIndex == 7){
					descriptorBlock[descriptorIndex].ldb = 1;
					break;
				}
			}
			descriptorIndex++;
		}
	for(rt = 0;rt < runTimes; rt++){
		needCPU = 0;
		transLoad(descriptorBlock, descriptorIndex+1, gdmaStateMachine, 0);
		doneLength = 0;
		needCPU = transRun(&nextRuleIndex, &doneLength);
	}

#ifdef RTL865X_MODEL_KERNEL
	rtlglue_free(cachedGdmaStateMachine);
	rtlglue_free(cachedDescriptorBlock);
#else
	rtlglue_free(gdmaStateMachine);
	rtlglue_free(descriptorBlock);
#endif
	if(needCPU == 0){
		if(nextRuleIndex==0){//no match.
			return 0;
		}
		else{//match
			return 1;
		}
	}
	return 0;
}
