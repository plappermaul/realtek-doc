#include "stateMachine.h"
#include "ruleTransform.h"
#include "aho.h"
#include "gdma.h"
#include "_kmp.h"
#include "gdma.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gdma_glue.h"

#define DEFAULT_IN_FILENAME "AhoPatterns"
#define DEFAULT_OUT_FILENAME "AhoTransRules"

void show_usage()
{

	rtlglue_printf("\n");
	rtlglue_printf("ahotrans: compile multiple patterns(strings) into GDMA transition rules.\n");
	rtlglue_printf("\n");
	rtlglue_printf("Description\n");
	rtlglue_printf("\t-f\tThe input file name with the input patterns. Each line cantains one patterns.\n");
	rtlglue_printf("\t-o\tThe output file name of the transition rules for the input regular expression pattern.\n");
	rtlglue_printf("\t-s\tShow states in the generated FSM.\n");
	rtlglue_printf("\t-r\tShow transition rules in the generated FSM.\n");
	rtlglue_printf("\t-n\tThe maximum number of patterns to get in the input file.\n");
	rtlglue_printf("\n");
	rtlglue_printf("\n");
}

int main(int argc, char** argv)
{
	SM* aho_sm;
	pmRule_t0** transRules;
	int numOfRules;
	int i;
        int j;
	char** Patterns;
	char b;
	int numOfPatterns;
	int bufIndex;
	char buf[1024];
	char* inFileName;
	char* outFileName;
	int fp;
	int maxNumOfPatterns;
	int showState;
	int showRules;
	char* endptr;

	showState = 0;
	showRules = 0;
	inFileName = NULL;
	outFileName = NULL;
	maxNumOfPatterns = 1000000;
	if (argc < 2) {
                show_usage();
                exit(0);
	}
        /* Parse any options */
        while (--argc > 0 && **(++argv) == '-') {
		if (*++(*argv))
			switch (**argv) {
				case 'n':
					++argv;
					--argc;
					maxNumOfPatterns = strtol(*(argv), &endptr, 0);
					break;
				case 'o':
					outFileName = *(++argv);
					--argc;
					break;
				case 's':
					showState = 1;
					break;
				case 'r':
					showRules = 1;
					break;
				case 'f':
					inFileName = *(++argv);
					--argc;
					break;
				default:
					show_usage();
					exit(0);
			}
	}



	aho_sm = NULL;
	numOfPatterns = 0;
	Patterns = (char**)rtlglue_malloc(maxNumOfPatterns*sizeof(char*));
	if(Patterns == NULL){
		rtlglue_printf("Patterns malloc fail\n");
		return FAILED;
	}
	//open Patterns file
	if(inFileName == NULL){
		fp = rtlglue_open( DEFAULT_IN_FILENAME, 0 , 0);
		rtlglue_printf("input file name = %s\n", DEFAULT_IN_FILENAME);
	}
	else{
		rtlglue_printf("input file name = %s\n", inFileName);
		fp = rtlglue_open(inFileName, 0, 0);
	}
	if (fp<=0){
		rtlglue_printf("fp = %d, %s file open fail\n",fp , inFileName);
		goto out1;
	}
	else{
		
		i = 0;
		bufIndex = 0;
		while(rtlglue_read( fp, &b, 1 )){
			if(b=='\n'){
				buf[bufIndex] = '\0';
				if(strlen(buf) == 0)
					continue;
				Patterns[i] = (char*)rtlglue_malloc((bufIndex+1)*sizeof(char));
				if(Patterns[i] == NULL){
					rtlglue_printf("Pattern[%d] malloc fail\n",i);
					rtlglue_close(fp);
					goto out2;
				}
				strcpy(Patterns[i], buf);
				//rtlglue_printf("Pattern[%d] = %s\n", i, Patterns[i]);
				numOfPatterns++;
				i++;
				bufIndex = 0;
				if(i >= maxNumOfPatterns )
					break;
			}
			else if(b=='\r'){
					
			}
			else{
				buf[bufIndex++] = b;
				if(bufIndex>1022){
					buf[bufIndex] = '\0';
					Patterns[i] = (char*)rtlglue_malloc((bufIndex+1)*sizeof(char));
					if(Patterns[i] == NULL){
						rtlglue_printf("Pattern[%d] malloc fail\n",i);
						rtlglue_close(fp);
						goto out2;
					}
					strcpy(Patterns[i], buf);
					numOfPatterns++;
					i++;
					bufIndex = 0;
					if( i >= maxNumOfPatterns)
						break;	
				}
			}
		}
		rtlglue_close(fp);
	}	

	aho_sm = aho_MakeStateMachine(Patterns, numOfPatterns);
	if(aho_sm==NULL){
		rtlglue_printf("aho_MakeStateMachine fail\n");
		goto out2;
	}
	if(showState)
		showStateMachine(aho_sm);

	transRules = (pmRule_t0**)stateToRules(aho_sm, &numOfRules);
	if(transRules==NULL){
		rtlglue_printf("transRule fail\n");
		goto out5;
	}
	if(showRules == 1)
		showTransRules(transRules, numOfRules);

	rtlglue_printf("number of transition rules = %d\n", numOfRules);
	
	if(outFileName == NULL){
		WriteTransitionRulesToFile( DEFAULT_OUT_FILENAME, transRules, numOfRules);
		rtlglue_printf("output file name : %s\n", DEFAULT_OUT_FILENAME);
	}
	else{
		WriteTransitionRulesToFile( outFileName, transRules, numOfRules);
		rtlglue_printf("output file name : %s\n", outFileName);
	}
	
	freeStateMachine(aho_sm);
	freeTransRules(transRules,numOfRules);
	for(j=0;j<numOfPatterns;j++){
                rtlglue_free(Patterns[j]);
	}
	rtlglue_free(Patterns);
	return SUCCESS;

out5:
	freeStateMachine(aho_sm);
	return FAILED;
out2:
	
	for(j=0;j<numOfPatterns;j++){
		rtlglue_free(Patterns[j]);
	}
out1:
	rtlglue_free(Patterns);
	return 0;
}

