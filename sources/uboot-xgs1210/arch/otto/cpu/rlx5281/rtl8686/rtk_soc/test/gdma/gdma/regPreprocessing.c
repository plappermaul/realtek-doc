#include "gdma_glue.h"
#include "regPreprocessing.h"
#include "gdma.h"
#define REAL_PROCESS 1
#define COUNT_SPACE 0

int exeflag;//0 for couting required memory space. 1 for real exection.
int transformedLength;
//char* _RegExpProcess(char* regexp, int length);
//char* regExpTransform(char *regexp, int regLength);
char* _RegExpProcess(char* regexp, int length, char* transformedReg, int* transformedRegIndex);
void questionMarkProcess(char* regexp, int *i, int reglength, char* transformedReg, int *transformedRegIndex);
void charProcess(char* regexp, int *i, int reglength,char* transformedReg, int *transformedRegIndex);
int findLeftParentheses(char *regexp, int position);
void escapProcess(char* regexp, int *i, int regexpLength, char* transformedReg, int *transformedRegIndex);
char* processExcapNum(char* regexp, int regexpLength,int *retRegLength);
void squrBracketProcess(char* regexp, int *i, int regexpLength, char* transformedReg, int *transformedRegIndex);
int findRightSqurBacket(char *regexp, int regexpLength, int position);
void dotProcess(char* regexp, int *i,int regexpLength, char* transformedReg, int *transformedRegIndex);
int findRightParentheses(char *regexp,int regLength, int position);

char* regExpTransform(char *regexp, int regLength, int *retRegExpLength)
{
	char* regexpNoExcapNum;
	char* retRegExp;
	int regexpNoExcapNumLength;
	int transformedRegIndex;
	char* transformedReg;
	
	//rtlglue_printf("original = %s\n",regexp);
	
	regexpNoExcapNum = processExcapNum(regexp, regLength,&regexpNoExcapNumLength);
	regexpNoExcapNum[regexpNoExcapNumLength] = '\0';
	rtlglue_printf("no excap num = %s\n",regexpNoExcapNum);
	//exeflag = COUNT_SPACE;//fack exection for count space
	//transformedLength = 0;
	//_RegExpProcess(regexpNoExcapNum,regexpNoExcapNumLength);
	transformedLength = 1024000;
	exeflag = REAL_PROCESS;//real process

	transformedReg = NULL;
	if(exeflag == REAL_PROCESS){
		transformedReg = (char *)rtlglue_malloc(transformedLength*sizeof(char));
	}

	transformedRegIndex = 0;

	retRegExp = _RegExpProcess(regexpNoExcapNum,regexpNoExcapNumLength, transformedReg, &transformedRegIndex);
	//rtlglue_printf("transformed length = %d,transformed regexp = %s\n",transformedLength,retRegExp);
	rtlglue_printf("transformed length = %d\n",transformedLength);
//	showNonCharString(retRegExp,transformedLength);
	rtlglue_free(regexpNoExcapNum);
	*retRegExpLength = transformedLength;
	return retRegExp;
}
/*
void showNonCharString(uint8* ns, int length)
{
	int i;
	for(i=0; i<length; i++){
		rtlglue_printf(" %u",ns[i]);

	}
	rtlglue_printf("\n");
}*/
char* _RegExpProcess(char* regexp, int length, char* transformedReg, int* transformedRegIndex)
{
	int i;
	
	for(i=0;i<length; i++){
		if(regexp[i]=='['){
			squrBracketProcess(regexp, &i, length, transformedReg, transformedRegIndex);
		}
		else if(regexp[i]=='.'){
			dotProcess(regexp, &i, length, transformedReg, transformedRegIndex);
		}
		else if(regexp[i]=='\\'){
			escapProcess(regexp, &i,length, transformedReg, transformedRegIndex);
		}
		else if(regexp[i]==']'){
			rtlglue_printf("] Error at position %d reg[i-1] = %02x\n",i,regexp[i-1]);
		}
		else if(regexp[i]=='?'){
			rtlglue_printf("? Error at position %d\n",i);
			//questionMarkProcess(regexp, &i, transformedReg, &transformedRegIndex);
		}
		else{
			charProcess(regexp, &i,length, transformedReg, transformedRegIndex);
		}

		
	}
	transformedLength = *transformedRegIndex;
	return  transformedReg;
}
//char* questionMarkProcess(char* regexp, int *i, int reglength, char* transformedReg, int *transformedRegIndex)
void questionMarkProcess(char* regexp, int *i, int reglength, char* transformedReg, int *transformedRegIndex)
{
		int subRegEndPosition;
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex + 1;

		if(regexp[*i+1]=='['){
			*i = *i+1;
			squrBracketProcess(regexp, i, reglength, transformedReg, transformedRegIndex);
		}
		else if(regexp[*i+1]=='.'){
			*i = *i+1;
			dotProcess(regexp, i,reglength, transformedReg, transformedRegIndex);
		}
		else if(regexp[*i+1]=='\\'){
			*i = *i+1;
			escapProcess(regexp, i, reglength, transformedReg, transformedRegIndex);
		}
		else if(regexp[*i+1]==']'){
			rtlglue_printf("] Error at position %d\n",*i);
		}
		else if(regexp[*i+1]=='?'){
			rtlglue_printf("? Error at position %d\n",*i);
			//questionMarkProcess(regexp, &i, transformedReg, &transformedRegIndex);
		}
		else if(regexp[*i+1]=='('){
			subRegEndPosition = findRightParentheses(regexp, reglength, (*i+1));
			_RegExpProcess(&regexp[*i+1], subRegEndPosition - (*i), transformedReg, transformedRegIndex);
			*i = subRegEndPosition;
		}
		else{
			*i = *i+1;
			charProcess(regexp, i,reglength, transformedReg, transformedRegIndex);
		}
		transformedReg[*transformedRegIndex] = ')';
		*transformedRegIndex = *transformedRegIndex + 1;
}

void charProcess(char* regexp, int *i, int reglength,char* transformedReg, int *transformedRegIndex)
{	
	int leftParenthPosition;
	int j;
	int copyEnd;

	if((*i+1)<reglength){
		if(regexp[*i+1] == '*'){
			transformedReg[*transformedRegIndex] = regexp[*i];
			transformedReg[*transformedRegIndex+1] = regexp[*i+1];
			*transformedRegIndex = *transformedRegIndex +2;
			*i = *i+1;
		}
		else if(regexp[*i+1] == '+'){//a+ => aa*
			if(regexp[*i]==')'){
				//find coresponding '(' position
				//leftParenthPosition=findLeftParentheses(regexp,*i);
				transformedReg[*transformedRegIndex] = ')';
				copyEnd = *transformedRegIndex;
				leftParenthPosition=findLeftParentheses( transformedReg, *transformedRegIndex);
				*transformedRegIndex = *transformedRegIndex + 1;
				transformedReg[*transformedRegIndex] = '(';
				*transformedRegIndex = *transformedRegIndex + 1;
				if(leftParenthPosition==-1){
					rtlglue_printf("No corresponding left parenthese at position %d\n", *i);
					return;
				}
				else{
					//for(j=leftParenthPosition; j<=(*i); j++){
					for(j=leftParenthPosition; j<=copyEnd; j++){
						//transformedReg[*transformedRegIndex] = regexp[j];
						transformedReg[*transformedRegIndex] = transformedReg[j];
						//transformedReg[*transformedRegIndex+(*i-leftParenthPosition+1)] = regexp[j];
						*transformedRegIndex = *transformedRegIndex + 1;
					}
				//	*transformedRegIndex = *transformedRegIndex + (*i-leftParenthPosition+1);
					transformedReg[*transformedRegIndex] = '*';
					*transformedRegIndex = *transformedRegIndex + 1;
					transformedReg[*transformedRegIndex] = ')';
					*transformedRegIndex = *transformedRegIndex + 1;
				}
				*i = *i+1;
			}
			else{
				transformedReg[*transformedRegIndex] = regexp[*i];
				transformedReg[*transformedRegIndex+1] = regexp[*i];
				transformedReg[*transformedRegIndex+2] = '*';
				*transformedRegIndex = *transformedRegIndex+3;
				*i = *i+1;
			}
		}
		else if(regexp[*i+1] == '?'){
			transformedReg[*transformedRegIndex] = '(';
			*transformedRegIndex = *transformedRegIndex + 1;
			transformedReg[*transformedRegIndex] = regexp[*i];
			*transformedRegIndex = *transformedRegIndex + 1;
			*i = *i+1;
			//startQustionMarkPosition = *transformedRegIndex;
			//questionMarkProcess(regexp,i ,reglength, transformedReg, transformedRegIndex);
			//endQustionMarkPosition = *transformedRegIndex-1;
			transformedReg[*transformedRegIndex] = '|';
			*transformedRegIndex = *transformedRegIndex + 1;
			//for(j=startQustionMarkPosition;j<=endQustionMarkPosition;j++){
			//	transformedReg[*transformedRegIndex] = transformedReg[j];
			//	*transformedRegIndex = *transformedRegIndex + 1;
			//}
			transformedReg[*transformedRegIndex] = ')';
			*transformedRegIndex = *transformedRegIndex + 1;
		}
		else{
			transformedReg[*transformedRegIndex] = regexp[*i];
			*transformedRegIndex = *transformedRegIndex + 1;
		}
		
	}
	else{
		transformedReg[*transformedRegIndex] = regexp[*i];
		*transformedRegIndex = *transformedRegIndex +1;
	}
}

int findRightParentheses(char *regexp,int regLength, int position)
{
	int i;
	int numOfLeftParenthese;
	int inSquareBacket;

	inSquareBacket = 0;
	numOfLeftParenthese = 0;
	for(i=position+1;i<regLength;i++){
		if(regexp[i] == '[' && regexp[i-1] != '\\')
			inSquareBacket++;
		if(regexp[i] == ']' && regexp[i-1] != '\\')
			inSquareBacket--;

		if(inSquareBacket == 0){
			if(regexp[i] == '(' && regexp[i-1] != '\\'){
				numOfLeftParenthese++;
			}
			if(regexp[i] == ')' && regexp[i-1] != '\\'){
				if(numOfLeftParenthese == 0)//we found it.
					return i;
				else
					numOfLeftParenthese--;
			
			}
		}
	}
	return -1;
}

int findLeftParentheses(char *regexp, int position)
{
	int i;
	int numOfRightParenthese;
	int inSquareBacket;

	inSquareBacket = 0;
	numOfRightParenthese = 0;
	for(i=position-1;i>=0;i--){
		if(regexp[i] == '[' && regexp[i-1] != '\\')
			inSquareBacket++;
		if(regexp[i] == ']' && regexp[i-1] != '\\')
			inSquareBacket--;

		if(inSquareBacket == 0){
			if(regexp[i] == ')' && regexp[i-1] != '\\'){
				numOfRightParenthese++;
			}
			if(regexp[i] == '(' && regexp[i-1] != '\\'){
				if(numOfRightParenthese == 0)//we found it.
					return i;
				else
					numOfRightParenthese--;
			
			}
		}
	}
	return -1;
}

void escapProcess(char* regexp, int *i, int regexpLength, char* transformedReg, int *transformedRegIndex)
{
	switch(regexp[*i+1])
	{
		case '.':
		case '*':
		case '(':
		case ')':
		case '|':
		case '\\':
			transformedReg[*transformedRegIndex] = '\\';
			*transformedRegIndex = *transformedRegIndex + 1;
			break;
		default: /* do nothing */
			break;
	}
	
 	transformedReg[*transformedRegIndex] = regexp[*i+1];
	*transformedRegIndex = *transformedRegIndex + 1;
 	*i = *i + 1;
}
char* processExcapNum(char* regexp, int regexpLength,int *retRegLength)
{
	char *retRegExp;
	int j;
	char upNum, downNum;
	retRegExp = (char*)rtlglue_malloc(regexpLength*sizeof(char));
	*retRegLength = 0;
	for(j=0;j<regexpLength-1;j++){
		if(regexp[j]=='\\'&& regexp[j+1]=='\\'){
			retRegExp[*retRegLength] = regexp[j];
			*retRegLength = *retRegLength + 1;
			j = j + 1;
			continue;
		}
		if(regexp[j]=='\\'&& regexp[j+1]=='x'){
			upNum = regexp[j+2];
			downNum = regexp[j+3];
			if(upNum >='0' && upNum <= '9')
					upNum = upNum - '0';
			else if(upNum >='A' && upNum <= 'F'){
					upNum = upNum - 'A' + 10;
			}
			else{
				upNum = upNum - 'a' + 10;
			}
			
			if(downNum >='0' && downNum <= '9')
					downNum = downNum - '0';
			else if(downNum >='A' && downNum <= 'F'){
					downNum = downNum - 'A' + 10;
			}
			else{
				downNum = downNum - 'a' + 10;
			}
			retRegExp[*retRegLength] = (upNum<<4) + downNum;
			*retRegLength = *retRegLength + 1;
			j=j+3;
		}
		else{
			retRegExp[*retRegLength] = regexp[j];
			*retRegLength = *retRegLength + 1;
		}
	}
	if(regexpLength>=4){
		if(regexp[regexpLength-4] != '\\'){
			retRegExp[*retRegLength] = regexp[regexpLength-1];
			*retRegLength = *retRegLength + 1;
		}
		else{
			if(regexp[regexpLength-3] != 'x'){
				retRegExp[*retRegLength] = regexp[regexpLength-1];
				*retRegLength = *retRegLength + 1;
			}
		}
	}
	else{
		retRegExp[*retRegLength] = regexp[regexpLength-1];
		*retRegLength = *retRegLength + 1;
	}
	return retRegExp;
	
}

void squrBracketProcess(char* regexp, int *i, int regexpLength, char* transformedReg, int *transformedRegIndex)
{
	int rightSqBackPosition;
	int j;
	char k;
	int startTransformedRegIndex;
	int endTransformedRegIndex;
	
	
	rightSqBackPosition = findRightSqurBacket(regexp, regexpLength, *i);

	if(rightSqBackPosition==-1){
		return;
	}
	if((rightSqBackPosition+1) < regexpLength){
		if(regexp[rightSqBackPosition+1]== '?'){
			transformedReg[*transformedRegIndex] = '(';
			*transformedRegIndex = *transformedRegIndex + 1;
		}
	}
	startTransformedRegIndex = *transformedRegIndex;
	transformedReg[*transformedRegIndex] = '(';
	*transformedRegIndex = *transformedRegIndex + 1;

	if(rightSqBackPosition+1<regexpLength){/*right parenthese for *. */
		if(regexp[rightSqBackPosition+1] == '*'){
			transformedReg[*transformedRegIndex] = '(';
			*transformedRegIndex = *transformedRegIndex + 1;
		}
	}
	for(j=*i+1;j<rightSqBackPosition; j++){
		if(regexp[j]=='\\'){
			escapProcess(regexp, &j ,rightSqBackPosition , transformedReg, transformedRegIndex);
			transformedReg[*transformedRegIndex] = '|';
			*transformedRegIndex = *transformedRegIndex + 1;
		}
		else if(regexp[j]=='-'){
			if((j+1)<rightSqBackPosition){
				if(regexp[j-1]<regexp[j+1]){
					for(k=regexp[j-1]+1; k<regexp[j+1];k++ ){
						switch(k)
						{
							case '.':
							case '*':
							case '(':
							case ')':
							case '\\':
							case '|':
							case '[':
							case ']':
								transformedReg[*transformedRegIndex] = '\\';
								*transformedRegIndex = *transformedRegIndex + 1;
								break;
							default: /* do nothing */
								break;
						}
						transformedReg[*transformedRegIndex] = k;
						transformedReg[*transformedRegIndex+1] = '|';
						*transformedRegIndex = *transformedRegIndex + 2;
					}
				}
				else{
					for(k=regexp[j-1]-1; k>regexp[j+1];k-- ){
						switch(k)
						{
							case '.':
							case '*':
							case '(':
							case ')':
							case '\\':
							case '|':
							case '[':
							case ']':
								transformedReg[*transformedRegIndex] = '\\';
								*transformedRegIndex = *transformedRegIndex + 1;
								break;
							default: /* do nothing */
								break;
						}
						transformedReg[*transformedRegIndex] = k;
						transformedReg[*transformedRegIndex+1] = '|';
						*transformedRegIndex = *transformedRegIndex + 2;
					}
				}
			}
			else{
				rtlglue_printf("error, - followed by no charactor, position %d\n", j);
			}
		}
		else{
			switch(k=regexp[j])
			{
				case '.':
				case '*':
				case '(':
				case ')':
				case '\\':
				case '|':
				case '[':
				case ']':
					transformedReg[*transformedRegIndex] = '\\';
					*transformedRegIndex = *transformedRegIndex + 1;
					break;
				default: /* do nothing */
					break;
			}
			transformedReg[*transformedRegIndex] = regexp[j];
			transformedReg[*transformedRegIndex+1] = '|';
			*transformedRegIndex = *transformedRegIndex + 2;
		}
	}
	endTransformedRegIndex = *transformedRegIndex-1;
	transformedReg[*transformedRegIndex-1] = ')';
	
	//Process quantum
	if((rightSqBackPosition+1) < regexpLength){
		if(regexp[rightSqBackPosition+1]== '*'){
			transformedReg[*transformedRegIndex] = '*';
			*transformedRegIndex = *transformedRegIndex + 1;
			transformedReg[*transformedRegIndex] = ')';
			*transformedRegIndex = *transformedRegIndex + 1;
			*i = rightSqBackPosition+1;
		}
		else if(regexp[rightSqBackPosition+1]== '+'){
			transformedReg[*transformedRegIndex] = '(';
			*transformedRegIndex = *transformedRegIndex + 1;

			for(j=startTransformedRegIndex; j<=endTransformedRegIndex;j++){
				transformedReg[*transformedRegIndex] = transformedReg[j];
				*transformedRegIndex = *transformedRegIndex + 1;
			}
			transformedReg[*transformedRegIndex] = '*';
			*transformedRegIndex = *transformedRegIndex + 1;
			transformedReg[*transformedRegIndex] = ')';
			*transformedRegIndex = *transformedRegIndex + 1;
			*i = rightSqBackPosition + 1;
		}
		else if(regexp[rightSqBackPosition+1]== '?'){
			*i= rightSqBackPosition+1;
	//		startQustionMarkPosition = *transformedRegIndex;
	//		questionMarkProcess(regexp,i ,regexpLength, transformedReg, transformedRegIndex);
	//		endQustionMarkPosition = *transformedRegIndex - 1;
			transformedReg[*transformedRegIndex] = '|';
			*transformedRegIndex = *transformedRegIndex + 1;
	//		for(j=startQustionMarkPosition;j<=endQustionMarkPosition;j++){
	//			transformedReg[*transformedRegIndex] = transformedReg[j];
	//			*transformedRegIndex = *transformedRegIndex + 1;
	//		}
			transformedReg[*transformedRegIndex] = ')';
			*transformedRegIndex = *transformedRegIndex + 1;
		}
		else{
			*i = rightSqBackPosition;
		}
		
	}
	else{
		*i = rightSqBackPosition;
	}
}

int findRightSqurBacket(char *regexp, int regexpLength, int position)
{
	int j;
	
	for(j=position+1;j<regexpLength ; j++){
		if(regexp[j]=='[' && regexp[j-1]!='\\'){
				rtlglue_printf("[ error, syntax error at position %d %c\n", j, regexp[j-1]);
				return -1;			
		}
		if(regexp[j]==']' && regexp[j-1]!='\\'){//find it.
				return j;
		}
	}
	return -1;//can't find it.
}

void dotProcess(char* regexp, int *i,int regexpLength, char* transformedReg, int *transformedRegIndex)
{
	int j;
	
	if(regexp[*i+1]=='*'){
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		for(j=0;j<255; j++){
			if(j == '.'|| j == '*' || j == '(' || j == ')' || j == '\\' || j == '|' || j == '[' || j == ']'){
				transformedReg[*transformedRegIndex] = '\\';
				*transformedRegIndex = *transformedRegIndex + 1;
			}
			transformedReg[*transformedRegIndex] = j;
			transformedReg[*transformedRegIndex+1] = '|';
			*transformedRegIndex = *transformedRegIndex + 2;
		}
		transformedReg[*transformedRegIndex] = 255;
		transformedReg[*transformedRegIndex+1] = '|';
		*transformedRegIndex = *transformedRegIndex + 2;

		transformedReg[*transformedRegIndex-1] = ')';
		transformedReg[*transformedRegIndex] = '*';
		*transformedRegIndex = *transformedRegIndex+1;
		transformedReg[*transformedRegIndex] = ')';
		*transformedRegIndex = *transformedRegIndex+1;
		*i = *i + 1;
	}
	else if(regexp[*i+1]=='?'){
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		for(j=0; j<255; j++){
			if(j == '.'|| j == '*' || j == '(' || j == ')' || j == '\\' || j == '|' || j == '[' || j == ']'){
				transformedReg[*transformedRegIndex] = '\\';
				*transformedRegIndex = *transformedRegIndex + 1;
			}
			transformedReg[*transformedRegIndex] = j;
			transformedReg[*transformedRegIndex+1] = '|';
			*transformedRegIndex = *transformedRegIndex + 2;
		}
		transformedReg[*transformedRegIndex] = 255;
		transformedReg[*transformedRegIndex+1] = '|';
		*transformedRegIndex = *transformedRegIndex + 2;

		transformedReg[*transformedRegIndex-1] = ')';
		*i = *i+1;
	//	startQustionMarkPosition = *transformedRegIndex;
	//	questionMarkProcess(regexp,i ,regexpLength, transformedReg, transformedRegIndex);
	//	endQustionMarkPosition = *transformedRegIndex - 1;
		transformedReg[*transformedRegIndex] = '|';
		*transformedRegIndex = *transformedRegIndex + 1;
	//	for(j=startQustionMarkPosition;j<=endQustionMarkPosition;j++){
	//		transformedReg[*transformedRegIndex] = transformedReg[j];
	//		*transformedRegIndex = *transformedRegIndex + 1;
	//	}
		transformedReg[*transformedRegIndex] = ')';
		*transformedRegIndex = *transformedRegIndex + 1;

	}
	else if(regexp[*i+1]=='+'){
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		for(j=0; j<255; j++){
			if(j == '.'|| j == '*' || j == '(' || j == ')' || j == '\\' || j == '|' || j == '[' || j == ']'){
				transformedReg[*transformedRegIndex] = '\\';
				*transformedRegIndex = *transformedRegIndex + 1;
			}
			transformedReg[*transformedRegIndex] = j;
			transformedReg[*transformedRegIndex+1] = '|';
			*transformedRegIndex = *transformedRegIndex + 2;
		}
		transformedReg[*transformedRegIndex] = 255;
		transformedReg[*transformedRegIndex+1] = '|';
		*transformedRegIndex = *transformedRegIndex + 2;
		
		transformedReg[*transformedRegIndex-1] = ')';
		transformedReg[*transformedRegIndex] = '*';
		*transformedRegIndex = *transformedRegIndex+1;		
		transformedReg[*transformedRegIndex] = ')';
		*transformedRegIndex = *transformedRegIndex+1;		
		*i = *i + 1;
	}
	else{
		transformedReg[*transformedRegIndex] = '(';
		*transformedRegIndex = *transformedRegIndex+1;
		for(j=0; j<255; j++){
			if(j == '.'|| j == '*' || j == '(' || j == ')' || j == '\\' || j == '|' || j == '[' || j == ']'){
				transformedReg[*transformedRegIndex] = '\\';
				*transformedRegIndex = *transformedRegIndex + 1;
			}
			transformedReg[*transformedRegIndex] = j;
			transformedReg[*transformedRegIndex+1] = '|';
			*transformedRegIndex = *transformedRegIndex + 2;
		}
		
		transformedReg[*transformedRegIndex] = 255;
		transformedReg[*transformedRegIndex+1] = '|';
		*transformedRegIndex = *transformedRegIndex + 2;
		transformedReg[*transformedRegIndex-1] = ')';
	}
}

