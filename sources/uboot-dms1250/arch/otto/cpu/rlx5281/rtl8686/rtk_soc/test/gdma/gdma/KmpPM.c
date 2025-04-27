#include "gdma_glue.h"
#include "stateMachine.h"
#include "ruleTransform.h"
#include "_kmp.h"
#include "gdma.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEFAULT_IN_FILENAME "KmpPatterns"
#define DEFAULT_OUT_FILENAME "KmpTrans"


void show_usage(void)
{
	rtlglue_printf("\n");
	rtlglue_printf("kmptrans : compile single pattern(string) into GDMA transition rules.\n");
	rtlglue_printf("\n");
	rtlglue_printf("Description\n");
	rtlglue_printf("\t-P\tThe input pattern.\n");
	rtlglue_printf("\t-f\tThe input file name with the input patterns. Each line cantains one patterns.\n");
	rtlglue_printf("\t-o\tThe output file name of the transition rules for the input regular expression pattern.\n");
	rtlglue_printf("\t-r\tShow transition rules in the generated FSM.\n");
	rtlglue_printf("\t-s\tShow states in the generated FSM.\n");
	rtlglue_printf("\n");
	rtlglue_printf("\n");
}
int main(int argc, char** argv)
{
	SM* kmp_sm;
	pmRule_t0** transRules;
	int numOfRules;
	//int i;
	char* kmpPattern;
	//uint32 totalLength;
	//int  k, l, m, y;
	int showState;
	int showRules;
	char* inFileName;
	char* outFileName;
	char readBuf[1024];
	int bufIndex;
	int fp;
	char inputChar;


	showState = 0;
	showRules = 0;
	inFileName = NULL;
	outFileName = NULL;
	kmpPattern = NULL;

        if (argc < 2) {
               show_usage();
               exit(0);
	}
        /* Parse any options */
        while (--argc > 0 && **(++argv) == '-') {
                if (*++(*argv))
                        switch (**argv) {
                                case 'P':
                                        ++argv;
                                        --argc;
					kmpPattern = *argv;
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

	if(kmpPattern ==NULL){
		//open file
		if(inFileName == NULL){
			//rtlglue_printf("pattern file name = %s\n", DEFAULT_IN_FILENAME);
			//fp = rtlglue_open( DEFAULT_IN_FILENAME, 0, 0);
			//if(fp <= 0){
				show_usage();
				exit(0);
			//}
		}
		else{
		
			rtlglue_printf("input file name = %s\n", inFileName);
		
			fp = rtlglue_open(inFileName,0,0);
			if(fp<=0){
				rtlglue_printf("%s : File open file\n",inFileName);
				exit(0);
			}
		}
		bufIndex = 0;
		while(rtlglue_read(fp, &inputChar, 1) && bufIndex < 1023){
			if(inputChar == '\n'|| inputChar == '\r')
				break;
			else
				readBuf[bufIndex++] = inputChar;
		}
		readBuf[bufIndex++] = '\0';
		if(bufIndex > 1023){
			rtlglue_printf("woring Pattern size > 1024\n");
			exit(0);
		}
		kmpPattern = readBuf;
		close(fp);
	}
	else
		bufIndex = strlen(kmpPattern);

	rtlglue_printf("pattern = %s\n", kmpPattern);
	kmp_sm = kmp_MakeStateMachine(kmpPattern);
	
	if(showState == 1)
		showStateMachine(kmp_sm);

	transRules =(pmRule_t0**)stateToRules(kmp_sm, &numOfRules);

	if(showRules == 1)
		showTransRules( transRules, numOfRules);
	
	rtlglue_printf("%d\t%d.%02d \t", (int)strlen(kmpPattern), 
		       	(int)(numOfRules*100/strlen(kmpPattern)/100),(int)(numOfRules*100/strlen(kmpPattern)%100));	
	
	if(outFileName == NULL)
		WriteTransitionRulesToFile( DEFAULT_OUT_FILENAME, transRules, numOfRules);
	else	
		WriteTransitionRulesToFile( outFileName, transRules, numOfRules);

	freeStateMachine(kmp_sm);
	freeTransRules(transRules, numOfRules);
	rtlglue_printf("complete\n");

	return SUCCESS;
}

