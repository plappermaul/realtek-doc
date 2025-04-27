#include "gdma.h"

#ifndef __TRANSRULELOADER_H__
#define __TRANSRULELOADER_H__

uint32* exeTransRules(pmRule_t0** transRules, int numOfRules, descriptor_t* cmpText, int numOfcmpText,int runTimes);
uint32* PMAlgoTest(char** patterns, int numOfPatterns, descriptor_t* cmpText, int numOfCmpText, void* pAlogExec, void* pAlogPreprocessing,void* pAlgoCleanup,int runTimes);
int prefixExeTransRules(pmRule_t0** transRules, int numOfRules, descriptor_t* cachedCmpText, int numOfcmpText, int runTimes);
#endif /* transRuleLoader.h */
