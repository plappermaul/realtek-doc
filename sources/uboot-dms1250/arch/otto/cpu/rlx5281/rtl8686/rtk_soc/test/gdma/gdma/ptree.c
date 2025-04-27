//#include <unistd.h>
//#include <fcntl.h>
#include "gdma_glue.h"
#include "ptree.h"

/*struct parsenode{
	char nodeChar;
	struct parsenode  *rightNode;
	struct parsenode  * leftNode;
};
typedef struct parsenode *ptrNode;*/
char operators[]=".|*";
char specialChars[] = ".|*()";


char* insertDot(char* reg)
{
	char* regdot;
	int regIndex;
	int regLength;
	int i;
	
	regLength = strlen(reg);
	regIndex = 0;
	regdot = (char*)malloc(regLength*2*sizeof(char));

	if(regdot==NULL){
		rtlglue_printf("Dot malloc fail function: %s, line %d\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	regdot[regLength*2-1] = '\0';
	
	for(i=1; (i<strlen(reg)) && (regIndex < (regLength*2-1));i++){
		if(isCharactor(reg[i]) && isCharactor(reg[i-1])){
			regdot[regIndex++] = reg[i-1];
			regdot[regIndex++] ='.';
		}
		else
			regdot[regIndex++] = reg[i-1];

	}
	regdot[regIndex++] = reg[i];
	regdot[regIndex] = '\0';
	return regdot;	
}

int isCharactor(char c)
{
	if(NULL!=strchr(operators,c))
		return 0;
	
	return 1;
}

ptrNode CreatePNode(char c)
{
	ptrNode newNode;
	
	newNode = NULL;
	newNode = (ptrNode)malloc(sizeof(struct parsenode));
	if(newNode){
		newNode->nodeChar = c;
		newNode->operation = INVALIED_OPERATION;
		newNode->rightNode = NULL;
		newNode->leftNode =NULL;
	}
	return newNode;
}

ptrNode MergeWithOp(char op, ptrNode r1, ptrNode r2)
{
	ptrNode NewRoot;

	NewRoot = CreatePNode(op);
	if(NewRoot!=NULL){
		NewRoot->leftNode = r1;
		NewRoot->rightNode = r2;
		if(op == '*'){
			 NewRoot->operation = STAR_OPERATION;
		}
		else if(op == '|'){
			NewRoot->operation = OR_OPERATION;
		}
		else if(op == '.'){
			NewRoot->operation = DOT_OPERATION;
		}
		else{
			NewRoot->operation = INVALIED_OPERATION;
		}

	}
	else
		rtlglue_printf("Fail to create node, function %s, line %d\n", __FUNCTION__, __LINE__);

	return NewRoot;
}

ptrNode Parse(char* reg, int *last,int regEnd)
{
	int plast;
	ptrNode v;
	ptrNode vr;
	ptrNode vtmp;

	v = NULL;
	plast = *last;
	//while(reg[plast] != '\0'){
	while(plast < regEnd){

		if(reg[plast]=='|'){
			plast++;
			vr = Parse(reg,&plast,regEnd);
			vtmp = MergeWithOp('|', v, vr);
			if(vtmp!=NULL){
				v = vtmp;
			}
		}
		else if(reg[plast]=='*'){
			vtmp = MergeWithOp('*', v, NULL);
			if(vtmp)
				v = vtmp;

			plast++;
		}
		else if(reg[plast]=='('){
			plast++;
			vr = Parse(reg,&plast,regEnd);
			plast++;
			if(v!=NULL){
				v = MergeWithOp('.', v, vr);
			}
			else{
				v = vr;
			}
		}
		else if(reg[plast]==')'){
			*last = plast;
			return v;
		}
		else if(reg[plast]=='\\'){
			plast++;
			vr = CreatePNode(reg[plast]);
			if(vr!=NULL){
				
				if(v!=NULL){
					vtmp = NULL;
					vtmp = MergeWithOp('.', v, vr);
					if(vtmp!=NULL){
						v = vtmp;
					}
					else{
						//freeTree(v);
						//freeTree(vr);
						return NULL;
					}
				}
				else
					v = vr;
			
				plast++;
			}
			else{
				//freeTree(v);
				//freeTree(vr);
				return NULL;
			}
		}
		else{//charactors
			vr = CreatePNode(reg[plast]);
			if(vr!=NULL){
				
				if(v!=NULL){
					vtmp = NULL;
					vtmp = MergeWithOp('.', v, vr);
					if(vtmp!=NULL){
						v = vtmp;
					}
					else{
						//freeTree(v);
						//freeTree(vr);
						return NULL;
					}
				}
				else
					v = vr;
			
				plast++;
			}
			else{
				//freeTree(v);
				//freeTree(vr);
				return NULL;
			}
		}
	}
	*last = plast;
	return v;
}

void freeTree(ptrNode v)
{
	if(v->leftNode!=NULL){
		freeTree(v->leftNode);
	}
	if(v->rightNode!=NULL){
		freeTree(v->rightNode);
	}
	rtlglue_free(v);	
}

void showTree(ptrNode root)
{


}

