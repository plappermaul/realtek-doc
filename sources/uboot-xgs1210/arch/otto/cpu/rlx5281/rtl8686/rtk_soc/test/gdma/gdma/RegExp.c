#include "ptree.h"
#include "Th_Auto.h"
#include "stateMachine.h"
#include "ruleTransform.h"
#include "Th_state_to_sm.h"
#include "gdma.h"
#include "regPreprocessing.h"
#include <fcntl.h>
#include "_kmp.h"
#include "gdma.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gdma_glue.h"

#define DEFAULT_IN_FILENAME	"RegPatterns"
#define DEFAULT_OUT_FILENAME	"RegTransRules"

void show_usage(void)
{
	rtlglue_printf("\n");
	rtlglue_printf("regtrans : Compile regular expression pattern into GDMA transition rules.\n");
	rtlglue_printf("\n");
	rtlglue_printf("Description\n");
	rtlglue_printf("\t-p\tCompile the input pattern with prefix feature. currently, our pattern compiler's syntax preprocessing does not\n");
	rtlglue_printf("\t\tsupport prefix symbol ^, and there are still difficulties to combine with regular expression patterns with prefix\n");
	rtlglue_printf("\t\tfeature. We may add this function with combining prefix and non-prefix regular expression patterns in next version\n");
	rtlglue_printf("\t\tof the FSM compiler.\n");
	rtlglue_printf("\t-N\tSet Not symbol to the indicated character. The transition rules produced with Not symbols are not the same\n");
	rtlglue_printf("\t\twith the last rule in the state with not feature.Since we might go to the next success state when the compared\n");
	rtlglue_printf("\t\tcharactor does not belong to the indicated charactors, for example, RE = \"[lw]?[lw]?AB\", when the first compared\n");
	rtlglue_printf("\t\tcharactor may not l or w, we won't go back to the initial state in this case, instead, we go to the next\n");
	rtlglue_printf("\t\tsuccess state. The transition rule for this purpose is not the same with the last transition in the state in the\n");
	rtlglue_printf("\t\tgeneral case of patterns. We don't have a mechanism in our regular expression core, we need to add the Not symbol\n");
	rtlglue_printf("\t\tin the favor of artifical effort. Therefore, in the example RE = \"[lw]?[lw]?AB\", we need to add a Not symbol into\n");
	rtlglue_printf("\t\tthe pattern. Here we use 'N' to be the Not symbol and modify RE pattern to \"[lwN][lwN]AB\".\n");
	rtlglue_printf("\t-F\tEnable fail state in FSM. In the FSM generated for patterns with prefix feature, once there is no success charactors\n");
	rtlglue_printf("\t\tmatch the compared charactor in a state, we just stop the FSM and reply there is no occurance in the context instead\n");
	rtlglue_printf("\t\tof going on proceessing next input charactor from the initial state. If it is required to stop FSM once there is \n");
	rtlglue_printf("\t\tmismatch in a state, we can set this flag on.\n");
	rtlglue_printf("\t-o\tThe output file name of the transition rules for the input regular expression pattern.\n");
	rtlglue_printf("\t-s\tShow states in the generated FSM.\n");
	rtlglue_printf("\t-r\tShow transition rules in the generated FSM.\n");
	rtlglue_printf("\t-f\tThe input file name with the input pattern.\n");
	rtlglue_printf("\n");
	rtlglue_printf("\n");

}
int main(int argc, char** argv)
{

	ptrNode pRoot;
	ptrThTreeNode Th_Auto;
	int IstateID;
	int totalNumOfRules;
	int fp;
	pmRule_t0** transformedRules;

	struct state_info *statelist;
	pSM regsm;
	char* transformedReg;
	int transformedRegLength;
	int last;
	char*  outFileName;
	char*  inFileName;
	char*  regPattern;
	int showState;
	int showRules;
	char readBuf[1024];
	char inputChar;
	int bufIndex;


	outFileName = NULL;
	inFileName = NULL;
	regPattern = NULL;
	unsetPrefix();
	unsetFailState();
	unsetNotSymbol();
	showState = 0;
	showRules = 0;

	if (argc < 2) {
		show_usage();
		exit(0);
	}

 	/* Parse any options */
	while (--argc > 0 && **(++argv) == '-') {
		if (*++(*argv))
			switch (**argv) {
				case 'p':
					setPrefix();
					break;
				case 'P':
					++argv;
					--argc;
					regPattern = *argv;
					break;
				case 'N':
					++argv;
					--argc;
					setNotSymbol(**argv);
					break;
				case 'F':
					setFailState();
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
	if(regPattern == NULL){/*Get no pattern in the command options.*/
		//open file;
		if(inFileName == NULL){/*error, no input pattern.*/
	//		rtlglue_printf("pattern file name = %s\n", DEFAULT_IN_FILENAME);
	//		fp = rtlglue_open( DEFAULT_IN_FILENAME, 0, 0);
	//		if(fp <= 0){
				show_usage();
                                exit(0);
	//		}
		}
		else{
			rtlglue_printf("pattern file name = %s\n", inFileName);
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
		regPattern = readBuf;
		close(fp);
	}
	else
		bufIndex = strlen(regPattern);		

	rtlglue_printf("bufIndex=%d\n",bufIndex);
	last = 0;
	transformedReg = regExpTransform(regPattern,bufIndex, &transformedRegLength);
	pRoot = Parse(transformedReg, &last, transformedRegLength);
	if(pRoot==NULL)
		rtlglue_printf("Parse regular expression fail!\n");
	//printf("string length = %d, last = %d\n",strlen(transformedReg), last);
	Th_Auto_init();
	Th_Auto = Thompson_reucr(pRoot);
	statelist = BuildDFA(Th_Auto, &IstateID);


	regsm = (pSM)Th_state_to_sm(&statelist,IstateID);
	//show_states(statelist);

	if(showState)
		showStateMachineNonChar(regsm);

	
	//showStateMachineNonChar(regsm);
	transformedRules = (pmRule_t0**)stateToRules(regsm,(int*)&totalNumOfRules);
	
	if(showRules)
		showTransRules(transformedRules, totalNumOfRules);

	if(outFileName)
		WriteTransitionRulesToFile(outFileName, transformedRules, totalNumOfRules);
	else
		WriteTransitionRulesToFile(DEFAULT_OUT_FILENAME, transformedRules, totalNumOfRules);
	
	rtlglue_printf("Output File: %s, total number of rules = %d\n", outFileName, totalNumOfRules);
	freeTransRules(transformedRules, totalNumOfRules);
        
	freeTree( pRoot );
	freeTh_Auto();
	freeStateMachine( regsm );
	return 0;
}

