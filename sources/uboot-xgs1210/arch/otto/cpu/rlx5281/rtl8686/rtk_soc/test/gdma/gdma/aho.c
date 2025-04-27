#include "aho.h"
#include "gdma_glue.h"

#define rtlglue_read read
#define rtlglue_open open
#define rtlglue_close close

struct qn {
	int stateID;
	struct qn* nextNode;
};
typedef struct qn QNode;

struct aho_queue {
	QNode* headNode;
	QNode* tailNode;
};
typedef struct aho_queue ahoQueue;


keyTreeNode* aho_AddChild(keyTreeNode* parent, char childChar, int nodeID);
keyTreeNode* findChild(keyTreeNode* parent, char childChar);
keyTreeNode** aho_BuildStateArray(keyTreeNode* kt, int numOfStates);
void showFail(keyTreeNode** nodeArray, int numOfStates);
void aho_ShowKeyTree(keyTreeNode** nodeArray, int numOfStates);
void freeKeyTree(keyTreeNode* kt);
void aho_MakeFail(keyTreeNode** nodeArray);
void aho_Enqueue(ahoQueue* Q, int stateID);
int aho_makeKeyTree(keyTreeNode** keytree,char* patList[], int numOfPattern);
int aho_Dequeue(ahoQueue* Q);
int aho_IsEmpty(ahoQueue* Q);

int isRedundant(char Char, char* cmpCharArray, int lastIndex)
{
	int i;
	for(i = 0 ; i < lastIndex ; i++){
		if(Char == cmpCharArray[i])
			return 1;
		}
	return 0;
}

#define STOPTIME 1000

SM* aho_MakeStateMachine(char* patList[], int numOfPatterns)
{

	SM* aho_sm;
	keyTreeNode* aho_kt;
	int numOfStates;
	int numOfRules;
	int ruleIndex;
	int v, i;
	int cmpCharIndex;
	int stateArrayIndex;
	char* cmpCharArray; //use this to record chars that have already appeared.
	State** statesArray;
	State* tmpStatesArray;
	keyTreeNode** pKTNodeArrays;//used to keep track of all key tree nodes.
	keyTreeNode* childNode;

	aho_sm = NULL;
	numOfStates = aho_makeKeyTree( &aho_kt, patList, numOfPatterns);
	if( numOfStates == -1 ){
		rtlglue_printf("aho_makeKeyTree fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out0;
	}
	//Store all the address of key tree nodes into an array for easy processing.
	pKTNodeArrays = aho_BuildStateArray(aho_kt, numOfStates);
	if(pKTNodeArrays == NULL){
		rtlglue_printf("aho_BuildStateArray fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out1;	
	}
	aho_MakeFail(pKTNodeArrays);

	//aho_ShowKeyTree(pKTNodeArrays, numOfStates);
	//showFail(pKTNodeArrays, numOfStates);
	aho_sm = (SM*)rtlglue_malloc(sizeof(SM));
	if(aho_sm == NULL){
		rtlglue_printf("aho_sm malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out2;
	}

	statesArray = (State**)rtlglue_malloc(((numOfStates+STATE_ARRAY_CAPACITY-1)/STATE_ARRAY_CAPACITY)*sizeof(State*));
	if(statesArray == NULL){
	        rtlglue_printf("aho_sm malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
	        goto out3;
	}

	aho_sm->numOfStates = numOfStates;
	aho_sm->states = statesArray;

	//Transform to state machine.
	
	cmpCharArray = (char*)rtlglue_malloc(numOfStates*sizeof(char));
	if(cmpCharArray == NULL){
		rtlglue_printf("cmpCharArray malloc fail, Function %s, line %d\n", __FUNCTION__, __LINE__);
		goto out4;
	}

	numOfRules = 0;
	childNode = pKTNodeArrays[0]->children;
	while(childNode!=NULL){//count the number of children.
		numOfRules++;
		childNode = childNode->nextSibling;
	}
	numOfRules = numOfRules + 1;//add 1 for fail link
	
	tmpStatesArray = (State*)rtlglue_malloc(STATE_ARRAY_CAPACITY*sizeof(State));
	if(!tmpStatesArray)
		rtlglue_printf("State array malloc fail! Number of states is %d, function %s, line %d\n", STATE_ARRAY_CAPACITY,__FUNCTION__, __LINE__);
	statesArray[0] = tmpStatesArray;
	tmpStatesArray[0].stateID = 0;
	tmpStatesArray[0].numOfRules = numOfRules;
	tmpStatesArray[0].transRules = (Rule*)rtlglue_malloc(numOfRules*sizeof(Rule));
	childNode = pKTNodeArrays[0]->children;
	ruleIndex = 0;
	while(childNode!=NULL){//success links of state 0
		tmpStatesArray[0].transRules[ruleIndex].cmpChar = childNode->nodeChar;
		tmpStatesArray[0].transRules[ruleIndex].not = 0;
		tmpStatesArray[0].transRules[ruleIndex].cpu = 0;
		tmpStatesArray[0].transRules[ruleIndex].jmpState = childNode->nodeID;
		ruleIndex++;
		childNode = childNode->nextSibling;	
	}
	if(pKTNodeArrays[0]->children!=NULL){//fail link fo state 0
		tmpStatesArray[0].transRules[ruleIndex].cmpChar = pKTNodeArrays[0]->children->nodeChar;
		tmpStatesArray[0].transRules[ruleIndex].not = 1;
		tmpStatesArray[0].transRules[ruleIndex].cpu = 0;
		tmpStatesArray[0].transRules[ruleIndex].jmpState = 0;
	}
		
        for(i=1;i<numOfStates;i++){
		stateArrayIndex = i%STATE_ARRAY_CAPACITY;
		if(stateArrayIndex == 0){
			tmpStatesArray = (State*)rtlglue_malloc(STATE_ARRAY_CAPACITY*sizeof(State));
			if(!tmpStatesArray)
				rtlglue_printf("State array malloc fail! Number of states is %d, function %s, line %d\n",STATE_ARRAY_CAPACITY,__FUNCTION__, __LINE__);

			statesArray[i/STATE_ARRAY_CAPACITY] = tmpStatesArray;
		}
		tmpStatesArray[stateArrayIndex].stateID = i;
		cmpCharIndex = 0;
		if(pKTNodeArrays[i]->children != NULL){//if it is not a termial state.
			//count the number of rules in each state.
			numOfRules = 0;
			childNode = pKTNodeArrays[i]->children;
			while(childNode!=NULL){//count the number of children.
				numOfRules++;
				cmpCharArray[cmpCharIndex++] = childNode->nodeChar;
				childNode = childNode->nextSibling;
			}
			v = pKTNodeArrays[i]->failIndex;
			while(v >= 0){
				childNode = pKTNodeArrays[v]->children;
				while(childNode!=NULL){//count the number of fail node's children.
					if(isRedundant(childNode->nodeChar, cmpCharArray, cmpCharIndex)== 0){
						numOfRules++;
						cmpCharArray[cmpCharIndex++] = childNode->nodeChar;
					}
					childNode = childNode->nextSibling;
				}
				v = pKTNodeArrays[v]->failIndex;
                        }
			numOfRules = numOfRules + 1;//add 1 for other fail links
			
			tmpStatesArray[stateArrayIndex].numOfRules = numOfRules;
			tmpStatesArray[stateArrayIndex].transRules = (Rule*)rtlglue_malloc(numOfRules*sizeof(Rule));
			childNode = pKTNodeArrays[i]->children;
			ruleIndex = 0;
			cmpCharIndex = 0;
			while(childNode!=NULL){//success links of state 0
				tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cmpChar = childNode->nodeChar;
		        	tmpStatesArray[stateArrayIndex].transRules[ruleIndex].not = 0;
				if(childNode->children == NULL){
					tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cpu = 1;
					tmpStatesArray[stateArrayIndex].transRules[ruleIndex].jmpState = childNode->failIndex;
				}
				else{
					if(childNode->outputIndex >= 0){
						tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cpu = 1;
					}
					else{
						tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cpu = 0;
					}
					tmpStatesArray[stateArrayIndex].transRules[ruleIndex].jmpState = childNode->nodeID;
				}
				cmpCharIndex++;//Because there is no redundant charactor among success links, so we just skip it.
				ruleIndex++;
				childNode = childNode->nextSibling;
			 }

		         v = pKTNodeArrays[i]->failIndex;
		         while(v >= 0){
				childNode = pKTNodeArrays[v]->children;
				while(childNode!=NULL){//count the number of children.
					if(isRedundant(childNode->nodeChar, cmpCharArray, cmpCharIndex)== 0){
						tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cmpChar = childNode->nodeChar;
						tmpStatesArray[stateArrayIndex].transRules[ruleIndex].not = 0;
						tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cpu = 0;
						tmpStatesArray[stateArrayIndex].transRules[ruleIndex].jmpState = childNode->nodeID;
						ruleIndex++;
						cmpCharIndex++;
					}
					childNode = childNode->nextSibling;
				}
				v = pKTNodeArrays[v]->failIndex;
			}

			if(pKTNodeArrays[i]->children!=NULL){//Add last fail links for other characters.
				tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cmpChar = pKTNodeArrays[i]->children->nodeChar;
				tmpStatesArray[stateArrayIndex].transRules[ruleIndex].not = 1;
				tmpStatesArray[stateArrayIndex].transRules[ruleIndex].cpu = 0;
				tmpStatesArray[stateArrayIndex].transRules[ruleIndex].jmpState = 0;
			}



		}
		else{//if it is a terminal state.
			tmpStatesArray[stateArrayIndex].numOfRules = 0;
			tmpStatesArray[stateArrayIndex].transRules = NULL;
		}
	}
	rtlglue_free(cmpCharArray);
	rtlglue_free(pKTNodeArrays);
	freeKeyTree(aho_kt);
	return aho_sm;
out4:
	rtlglue_free(cmpCharArray);
out3:
	rtlglue_free(aho_sm);
out2:
	rtlglue_free(pKTNodeArrays);
out1:
	freeKeyTree(aho_kt);
out0:
	return NULL;
}
//We use list of key tree node to invest all nodes instead of tranversing the key tree structure.
void showFail(keyTreeNode** nodeArray, int numOfStates)
{
	int i;

	for(i=0;i<numOfStates;i++){
		rtlglue_printf("fail[%d] = %d\n", i, nodeArray[i]->failIndex);
	}

}
void aho_MakeFail(keyTreeNode** nodeArray)
{
	ahoQueue Q;
	keyTreeNode* node;
	keyTreeNode* chNode;
	int rootNodeIndex;
	int v;

	Q.headNode = NULL;
	Q.tailNode = NULL;
	nodeArray[0]->failIndex = -1;
	node = nodeArray[0]->children;
	//initialization
	while(node!=NULL){
		node->failIndex = 0;
		aho_Enqueue(&Q, node->nodeID);
		node = node->nextSibling;
	}

	while(aho_IsEmpty(&Q)==0){
		rootNodeIndex = aho_Dequeue(&Q);
		node = nodeArray[rootNodeIndex]->children;
		while(node!=NULL){
			aho_Enqueue(&Q, node->nodeID);
			v = nodeArray[rootNodeIndex]->failIndex;
			while(v >= 0){
				chNode = nodeArray[v]->children; //check all nodeArray[v] children node to see if there is a match.
				while(chNode!=NULL)
				{
					if(chNode->nodeChar == node->nodeChar){
						nodeArray[node->nodeID]->failIndex = chNode->nodeID;
						break;
					}
					chNode = chNode->nextSibling;
				}
				if(chNode != NULL)
					break;

				v = nodeArray[v]->failIndex;
				if(v<0)
				   nodeArray[node->nodeID]->failIndex = 0;
			}

			node = node->nextSibling;
		}
	}

	nodeArray[0]->failIndex = -1;//We set fail index of state 0 to -1
}

void aho_Enqueue(ahoQueue* Q, int stateID)
{
	QNode* newNode;

	newNode = (QNode*)rtlglue_malloc(sizeof(QNode));
	newNode->stateID = stateID;
	newNode->nextNode = NULL;

	if(Q->tailNode == NULL || Q->headNode == NULL){
		Q->headNode = newNode;
		Q->tailNode = newNode;
	}
	else{
		Q->tailNode->nextNode = newNode;
		Q->tailNode = newNode;	
	}
}

int aho_Dequeue(ahoQueue* Q)
{
	QNode* deqNode;
	int returnID;
	if(Q->headNode == NULL)
		return -1;

	else{
		deqNode = Q->headNode;
		Q->headNode = Q->headNode->nextNode;
		returnID = deqNode->stateID;
		rtlglue_free(deqNode);

		return returnID;
	}	
}

int aho_IsEmpty(ahoQueue* Q)
{
	if(Q->headNode == NULL)
		return 1;

	else
		return 0;
}

keyTreeNode** aho_BuildStateArray(keyTreeNode* kt ,int numOfStates)
{
	int i;/*used for loop count.*/
	keyTreeNode** nodeArray;
	nodeArray = (keyTreeNode**)rtlglue_malloc(numOfStates*sizeof(keyTreeNode*));
	if(nodeArray == NULL){
		rtlglue_printf("nodeArray malloc fail,numOfStates = %d, Function = %s, line %d\n", numOfStates , __FUNCTION__, __LINE__);
		return NULL;
	}
	for(i=0; i<numOfStates; i++){
		nodeArray[i] = kt;
		kt = kt->nextNode;
	}

	return nodeArray;	
}

int aho_makeKeyTree(keyTreeNode** keytree, char* patList[], int numOfPattern)
{

	int i, j;/*used for loop count.*/
	keyTreeNode* kt;/*point to the root fo the key tree.*/
	keyTreeNode* curKeyTreeNode;/*point to current node in building key tree.*/
	keyTreeNode* childNode;/*point to child key tree node.*/
	keyTreeNode* previousNode;
	char* pattern;
	int idCount;
	//construct the start node;        
        kt = (keyTreeNode*)rtlglue_malloc(1*sizeof(keyTreeNode));
	if(kt == NULL)
		return -1;

	kt->nodeChar = 0;
	kt->nodeID = 0;
	kt->outputIndex = -1;
	kt->nextSibling = NULL;
	kt->parent = NULL;
	kt->nextNode = NULL;
	kt->children = NULL;
	kt->failIndex = 0;
	idCount = 1;
	previousNode = kt;
	for(i=0; i<numOfPattern; i++){
		curKeyTreeNode = kt;
		pattern = patList[i];
		for(j=0; j<strlen(patList[i]); j++){
			childNode = findChild(curKeyTreeNode, pattern[j]);
			if(childNode == NULL){
				previousNode->nextNode = aho_AddChild(curKeyTreeNode, pattern[j], idCount);
				if(previousNode->nextNode == NULL){
					rtlglue_printf("aho_AddChild error\n");
					goto out;
				}
				previousNode = previousNode->nextNode;
				curKeyTreeNode = previousNode;
				idCount++;
			}
			else{
				curKeyTreeNode = childNode;  
			}			
		}
		curKeyTreeNode->outputIndex = i;
	}
	(*keytree) = kt;
/*	previousNode = kt;
	i = 0;
	while(previousNode){
		i++;
		rtlglue_printf("NodeID = %d = %p\n", previousNode->nodeID,previousNode);
		previousNode = previousNode->nextNode;
	}*/
	return idCount;

out:
	freeKeyTree(kt);
	return -1;

}

void freeKeyTree(keyTreeNode* kt)
{
	keyTreeNode* nodeToFree;
	if(kt==NULL)
		return;

	do{
		nodeToFree = kt;
		kt = kt->nextNode;
		rtlglue_free(nodeToFree);
	}while(kt!=NULL);


}

/*void freeKeyTree(keyTreeNode* kt)
{
	keyTreeNode* targetNode;

	if(kt == NULL)
		return;

	targetNode = kt;
	do{
		
		while(targetNode->children != NULL){
			targetNode = targetNode->children;
		}

		while(targetNode->nextSibling != NULL){
			targetNode = targetNode->nextSibling;
		}

		if(targetNode->children == NULL && targetNode->nextSibling == NULL){
			if(targetNode->parent->children == targetNode)//Last node is parent node;
				targetNode->parent->children = NULL;
			else{//Last node is sibling node.
				lastNode = targetNode->parent->children;
				while(lastNode->nextSibling!=targetNode){
					lastNode = lastNode->nextSibling;
				}
				lastNode->nextSibling = NULL;
			}
			rtlglue_free(targetNode);
			targetNode = kt;
		}
	}while(targetNode!=kt);
}
*/
void aho_ShowKeyTree(keyTreeNode** nodeArray, int numOfStates)
{
	int i;

	for(i=0; i< numOfStates; i++){
		rtlglue_printf("=======================\n");
		rtlglue_printf("State %d \n", i);
		rtlglue_printf("ID = %d, Char = %c, output = %d, fail = %d\n",nodeArray[i]->nodeID, nodeArray[i]->nodeChar,
				nodeArray[i]->outputIndex, nodeArray[i]->failIndex);
	}

}

keyTreeNode* findChild(keyTreeNode* parent, char childChar)
{
	keyTreeNode* nextChild;

	if (parent == NULL) {
		return NULL;
	} 
	if(parent->children!=NULL){
		nextChild = parent->children;
		while(nextChild!=NULL){
			if(nextChild->nodeChar == childChar){
				return nextChild;
			}
			nextChild = nextChild->nextSibling;	
		}
	}
	return NULL;
}

keyTreeNode* aho_AddChild(keyTreeNode* parent, char childChar, int nodeID)
{
	keyTreeNode* childNode;
	keyTreeNode* nextChild;

	childNode = NULL;
	childNode = (keyTreeNode*)rtlglue_malloc(1*sizeof(keyTreeNode));
	
	if(childNode == NULL)
		return NULL;
	childNode->nodeChar = childChar;
	childNode->nodeID = nodeID;
	childNode->outputIndex = -1;
	childNode->nextSibling = NULL;
	childNode->children = NULL;
	childNode->parent = parent;
	childNode->nextNode = NULL;
	childNode->failIndex = 0;
	if(parent->children == NULL){
		parent->children = childNode;
	}
	else{
		nextChild = parent->children;
		while(nextChild->nextSibling!=NULL){
			nextChild = nextChild->nextSibling;
		}
		nextChild->nextSibling = childNode;
	}

	return childNode;
}


