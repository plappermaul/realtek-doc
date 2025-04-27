#include "Th_Auto.h"
#include "gdma_glue.h"

/*struct closure_entity{
	ptrThTreeNode node;
	struct closure_entity *next_entity;
};*/

/*struct node_info{
	ptrThTreeNode node;
	//struct closure_entity *cn;
	struct node_info *next_node_info;
};*/	

/*struct node_list{
	ptrThTreeNode node;
	struct node_list *next_entity;
};*/

/*struct state_info{
	struct node_list *nodelist;
	struct th_state_link *outlink;
	struct state_info *nextstate;
}*/

struct state_info *statelist;

struct node_list *th_stack;//used for EpsClosure operation.

struct node_list *nodelist;//use this for tracking all nodes;
struct state_info *states;
int gNodeID;
int stateID;
static int _NonPrefix;
void setPrefix()
{
	_NonPrefix = 0;
}
void unsetPrefix()
{
	_NonPrefix = 1;
}

void Add_Closure_Entity(struct node_list *nodecn, ptrThTreeNode node)
{
	struct node_list* entitytmp;

	if(node == NULL){
		rtlglue_printf("Add_Closure_Entity fail, node is NULL, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		return;
	}
	if(nodecn){
		entitytmp = (struct node_list*)rtlglue_malloc(sizeof(struct node_list));
		if(entitytmp){
			entitytmp->node = node;
			entitytmp->next_entity = nodecn->node->cn;
			nodecn->node->cn = entitytmp;
		}
		else
			 rtlglue_printf("closure entity malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);

	}
	else
		rtlglue_printf("Add Closure Entity = NULL, function = %s, line = %d\n",__FUNCTION__, __LINE__);
}

void Th_Node_Push(ptrThTreeNode node)
{
	struct node_list *newEntity;

	newEntity = (struct node_list *)rtlglue_malloc(sizeof(struct node_list));
	if(newEntity){
		newEntity->node = node;
		newEntity->next_entity = th_stack;
		th_stack = newEntity;
	}
	else
		rtlglue_printf("node list malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
}

ptrThTreeNode Th_Node_Pop()
{
	ptrThTreeNode retNode;
	struct node_list* target;

	if(th_stack){
		retNode = th_stack->node;
		target = th_stack;
		th_stack = th_stack->next_entity;
		free(target);
		return retNode;
	}
	else
		return NULL;
}

void clearMarked()
{
	struct node_list *tmp_node_info;

	tmp_node_info = nodelist;
	
	while(tmp_node_info){
		tmp_node_info->node->marked = 0;
		tmp_node_info = tmp_node_info->next_entity;
	}
}

void EpsClosure()
{
	struct node_list *countedNode;
	ptrThTreeLink linktmp;
	ptrThTreeNode nodetmp;
	countedNode = nodelist;
	
	while(countedNode){
		clearMarked();
		nodetmp = countedNode->node;
		nodetmp->marked = 1;
		Add_Closure_Entity(countedNode, countedNode->node);
		while(nodetmp){
			linktmp = nodetmp->outlink;
			while(linktmp){
				//if((linktmp->symbol == '$')&&(linktmp->linkedNode->marked != 1)){
				if((linktmp->isElink == 1)&&(linktmp->linkedNode->marked != 1)){
					Add_Closure_Entity(countedNode, linktmp->linkedNode);
					linktmp->linkedNode->marked = 1;
					Th_Node_Push(linktmp->linkedNode);
				}
				linktmp = linktmp->nextLink;	
			}
			nodetmp = Th_Node_Pop();
		}
		countedNode = countedNode->next_entity;
	}
}

void Th_Auto_init(void)
{
	gNodeID = 0;
	nodelist = NULL;
	th_stack = NULL;
	stateID = 0;
}

ptrThTreeNode createThNode(void)
{
	ptrThTreeNode newNode;
	struct node_list *new_nodeinfo;

	if(nodelist==NULL){
		nodelist = (struct node_list *)rtlglue_malloc(sizeof(struct node_list));
		if(nodelist){
			nodelist->next_entity = NULL;
		//	nodelist->cn = NULL;
			new_nodeinfo = nodelist;	
		}
		else{
			rtlglue_printf("node info malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
			return NULL;
		}
	}
	else{
		new_nodeinfo = (struct node_list *)rtlglue_malloc(sizeof(struct node_list));
		if(new_nodeinfo){
			//new_nodeinfo->cn = NULL;
			new_nodeinfo->next_entity = nodelist;
		}
		else{
			rtlglue_printf("node info malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
			return NULL;
		}
	}
	
	newNode = (ptrThTreeNode)rtlglue_malloc(sizeof(Th_Tree_Node));
	if(newNode){
		new_nodeinfo->node = newNode;
		new_nodeinfo->node->cn = NULL;
	}
	else{
		rtlglue_printf("node info malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		free(new_nodeinfo);
		return NULL;
	}

	nodelist = new_nodeinfo;

	return newNode;
}

void freeThNode(ptrThTreeNode v)
{
	struct node_list *nlist;
	struct node_list *target_node_info;
	struct node_list *target_cn;
	nlist = nodelist;
	
	if(nlist->node == v){
		nodelist = NULL;
		free(nlist);
		return;
	}
	while(nlist->next_entity){
		if(nlist->next_entity->node == v){
			target_node_info = nlist->next_entity;
			nlist->next_entity = nlist->next_entity->next_entity;
			while(target_node_info->node->cn){
				target_cn = target_node_info->node->cn;
				target_node_info->node->cn = target_node_info->node->cn->next_entity;
				free(target_cn);
			}
			free(target_node_info);
			free(v);
			return;
		}
		nlist = nlist->next_entity;
	}
}

ptrThTreeNode Th_Op_One()
{
	ptrThTreeNode I;
	
	I = NULL;
	I =  createThNode();
	if(I){
		I->outlink = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));
		if(I->outlink){
			I->outlink->nextLink = NULL;
			I->outlink->symbol = '$';
			I->outlink->isElink = 1;
			I->nodeID = gNodeID++;
			I->final_node = createThNode();
			if(I->final_node){
				I->final_node->final_node=NULL;
				I->final_node->outlink=NULL;
				I->final_node->nodeID=gNodeID++;
				I->outlink->linkedNode = I->final_node;			
			}
			else
				rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		}
		else
			rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
	}
	else
		rtlglue_printf("Tree node malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		
	return I;
}
ptrThTreeNode Th_Op_Two(char nodeSymbel)
{
	ptrThTreeNode I;

	I =(ptrThTreeNode)Th_Op_One();
	I->outlink->isElink = 0;
	I->outlink->symbol=nodeSymbel;
	return I;
}
ptrThTreeNode Th_Op_Three(ptrThTreeNode v1,ptrThTreeNode v2)
{
	//check v1,v2
	if(v1==NULL)
		return v2;
	if(v2==NULL)
		return v1;
	v1->final_node->outlink = v2->outlink;
	v1->final_node = v2->final_node;
	v2->final_node = NULL;
	freeThNode(v2);
	return v1;
}
ptrThTreeNode Th_Op_Four(ptrThTreeNode v1,ptrThTreeNode v2)
{
	ptrThTreeNode I;

	
//	I =  (ptrThTreeNode)rtlglue_malloc(sizeof(Th_Tree_Node));
	if(v1==NULL && v2==NULL)
		return NULL;

	I = NULL;
	I =  createThNode();
	I->final_node = createThNode();//Final node

	if(I->final_node){
		I->final_node->final_node = NULL;
		I->final_node->outlink = NULL;
		I->final_node->nodeID=gNodeID++;
	}
	else
		rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);

	if(I){
		I->outlink = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));//Initial node outlink 1
		if(I->outlink){
			I->outlink->symbol = '$';
			I->outlink->isElink = 1;
			if(v1==NULL)
				I->outlink->linkedNode = I->final_node;
			else
				I->outlink->linkedNode = v1;

			I->nodeID = gNodeID++;
		}
		else
			rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		
			
		I->outlink->nextLink = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));//Initial node outlink 2
		if(I->outlink->nextLink){
			I->outlink->nextLink->nextLink = NULL;
			I->outlink->nextLink->symbol = '$';
			I->outlink->nextLink->isElink = 1;
			if(v2==NULL)
				I->outlink->nextLink->linkedNode = I->final_node;
			else
				I->outlink->nextLink->linkedNode = v2;
		}
		else
			rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		
		if(v1!=NULL){
			v1->final_node->outlink = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));
			if(v1->final_node->outlink){
				v1->final_node->outlink->symbol = '$';
				v1->final_node->outlink->isElink = 1;
				v1->final_node->outlink->nextLink = NULL;
				v1->final_node->outlink->linkedNode = I->final_node;
				v1->final_node = NULL;
			}
			else
				rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
		}
		if(v2!=NULL){
			v2->final_node->outlink = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));
			if(v2->final_node->outlink){
				v2->final_node->outlink->symbol = '$';
				v2->final_node->outlink->isElink = 1;
				v2->final_node->outlink->nextLink = NULL;
				v2->final_node->outlink->linkedNode = I->final_node;
				v2->final_node = NULL;
			}
			else
				rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
			}
		}
	else
		rtlglue_printf("Tree node malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);

	return I;

}
ptrThTreeNode Th_Op_Five(ptrThTreeNode v)
{
	ptrThTreeNode I;
	ptrThTreeLink link;
	
	if(v == NULL)
		return NULL;
	I = Th_Op_One();
	//Outlink of I
	link = NULL;
	link = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));
	if(link){
		link->symbol = '$';
		link->isElink = 1;
		link->linkedNode = v;
		link->nextLink = I->outlink;
		I->outlink = link;
	}
	else
		rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
	
	//Outlink of v
	link = NULL;
	link = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));
	if(link){
		link->symbol = '$';
		link->isElink = 1;
		link->linkedNode = I->final_node;
		link->nextLink=v->final_node->outlink;
		v->final_node->outlink = link;
	}
	else
		rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);
	//loop link of v
	link = NULL;
	link = (ptrThTreeLink)rtlglue_malloc(sizeof(Th_Tree_Link));
	if(link){
		link->symbol = '$';
		link->isElink = 1;
		link->linkedNode = v;
		link->nextLink=v->final_node->outlink;
		v->final_node->outlink = link;
	}
	else
		rtlglue_printf("Tree link malloc fail, function = %s, line = %d\n",__FUNCTION__, __LINE__);

	v->final_node = NULL;

	return I;
}
/*
ptrThTreeNode Thompson_reucr(ptrNode v)
{
	ptrThTreeNode Th_vl;
	ptrThTreeNode Th_vr;
	ptrThTreeNode Th_star;
	ptrThTreeNode retv;

	if(v->nodeChar == '|' || v->nodeChar == '.'){
		Th_vl = Thompson_reucr(v->leftNode);
		Th_vr = Thompson_reucr(v->rightNode);
	}
	else if(v->nodeChar == '*'){
		Th_star = Thompson_reucr(v->leftNode);
	}

	else{
	}

	if(isCharactor(v->nodeChar)){
		return Th_Op_Two(v->nodeChar);
	}
	else if(v->nodeChar == '.'){
		return Th_Op_Three( Th_vl, Th_vr);
	}
	else if(v->nodeChar == '|'){
		retv = Th_Op_Four( Th_vl, Th_vr);
		return retv;
	}
	else if(v->nodeChar == '*'){
		retv = Th_Op_Five(Th_star);
		return retv;
	}
	else{
	}
}
*/
ptrThTreeNode Thompson_reucr(ptrNode v)
{
	ptrThTreeNode Th_vl;
	ptrThTreeNode Th_vr;
	ptrThTreeNode Th_star;
	ptrThTreeNode retv;

	Th_vl = NULL;
	Th_vr = NULL;
	Th_star = NULL;
	if(v->operation == OR_OPERATION || v->operation == DOT_OPERATION){
		if(v->leftNode != NULL)
			Th_vl = Thompson_reucr(v->leftNode);
		if(v->rightNode != NULL)
			Th_vr = Thompson_reucr(v->rightNode);
	}
	else if(v->operation == STAR_OPERATION){
		Th_star = Thompson_reucr(v->leftNode);
	}

	else{
	}

	if(v->operation == INVALIED_OPERATION){
		return Th_Op_Two(v->nodeChar);
	}
	else if(v->operation == DOT_OPERATION){
		return Th_Op_Three( Th_vl, Th_vr);
	}
	else if(v->operation == OR_OPERATION){
		retv = Th_Op_Four( Th_vl, Th_vr);
		return retv;
	}
	else if(v->operation == STAR_OPERATION){
		retv = Th_Op_Five(Th_star);
		return retv;
	}
	else{
	}
	return NULL;
}

void show_nodes_info(void)
{
	struct node_list *target_info;
	struct th_tree_link *linktmp;
	struct node_list  *tmpcn; 
	target_info = nodelist;
	while(target_info){
		if(target_info->node){
			rtlglue_printf("Node ID: %d\n",target_info->node->nodeID);
			linktmp = target_info->node->outlink;
			while(linktmp){
				rtlglue_printf("       ---%c--->node %d\n",linktmp->symbol,linktmp->linkedNode->nodeID);
				linktmp = linktmp->nextLink;
			}

			rtlglue_printf("      closure nodes: ");
			tmpcn = target_info->node->cn;
			while(tmpcn){
				rtlglue_printf(" %d,",tmpcn->node->nodeID);
				tmpcn = tmpcn->next_entity;
			}
			rtlglue_printf("\n");
			target_info = target_info->next_entity;
		}
	}
}

void freeTh_Auto()
{
	struct node_list *target_info;
	struct node_list *tmpcn;
	ptrThTreeLink tmplink;

	while(nodelist){
		target_info = nodelist;
		if(target_info->node){
			while(target_info->node->outlink){
				tmplink = target_info->node->outlink;
				target_info->node->outlink = target_info->node->outlink->nextLink;
				free(tmplink);
			}
			free(target_info->node);
		}
		while(target_info->node->cn){
			tmpcn = target_info->node->cn;
			target_info->node->cn = target_info->node->cn->next_entity;
			free(tmpcn);
		}

		free(target_info);
		nodelist = nodelist->next_entity;
	}
}


void BuildState(struct state_info *State)
{
	struct state_info *T;
	struct state_info *foundState;
	struct node_list  *tmplist;
	ptrThTreeLink tmplink;
	ptrThStateLink tmpstatelink;
	char sym_array[256];
	int symindex;
	int i;
	
	tmplist = State->nodelist;
	symindex = 0;
	//Traverse all links to record symbols.
	while(tmplist){
		tmplink = tmplist->node->outlink;
		while(tmplink){
			//if the symbol is new
			//if(tmplink->symbol !='$'){
			if(tmplink->isElink != 1){
				for(i=0; i<symindex; i++){
					if(sym_array[i] == tmplink->symbol)
						break;
				}
				if(i==symindex)
					sym_array[symindex++] = tmplink->symbol;
			}
			//else next link
			tmplink = tmplink->nextLink;
		}

		tmplist = tmplist->next_entity;
	}

	for(i=0; i<symindex; i++){
		T = createState();
		tmplist = State->nodelist;
		while(tmplist){
			tmplink = tmplist->node->outlink;
			while(tmplink){
				if((tmplink->symbol==sym_array[i])&&(tmplink->isElink != 1)){
					T->nodelist = nodelist_Union(T->nodelist,tmplink->linkedNode->cn);		
				}
				tmplink = tmplink->nextLink;
			}

			tmplist = tmplist->next_entity;
		}

		foundState = findState(T, states);
		tmpstatelink = (ptrThStateLink)rtlglue_malloc(sizeof(Th_State_Link));

		if(foundState){//T belongs to states, free T.
			if(tmpstatelink){
				tmpstatelink->linkedState = foundState;
				tmpstatelink->symbol = sym_array[i];
				tmpstatelink->nextLink = State->outlink;
				State->outlink = (struct Th_State_Link *)tmpstatelink;
			}
			else
				rtlglue_printf("state link malloc fail, function %s, line %d\n", __FUNCTION__, __LINE__);
			
			while(T->nodelist){
				tmplist = T->nodelist;
				T->nodelist = T->nodelist->next_entity;
				free(tmplist);	
			}
			free(T);
		}
		else{//T is not belongs to states, add it into states.
			if(tmpstatelink){
				tmpstatelink->linkedState = T;
				tmpstatelink->symbol = sym_array[i];
				tmpstatelink->nextLink = State->outlink;
				State->outlink = (struct Th_State_Link *)tmpstatelink;

				T->nextstate = states;
				states = T;
				BuildState(T);
			}
			else{
				rtlglue_printf("state link malloc fail, function %s, line %d\n", __FUNCTION__, __LINE__);
				while(T->nodelist){
					tmplist = T->nodelist;
					T->nodelist = T->nodelist->next_entity;
					free(tmplist);	
				}
				free(T);
			}	
		}	
	}
	
		
}

struct state_info* findState(struct state_info *T, struct state_info *statelist)
{
	struct state_info *tmpstate;

	tmpstate = states;

	while(tmpstate){
		if(isBelong(T->nodelist, tmpstate->nodelist) && isBelong(tmpstate->nodelist, T->nodelist))
			break;
		tmpstate = tmpstate->nextstate;
	}
	return tmpstate;
}

int isBelong(struct node_list *listA, struct node_list *listB)
{
	struct node_list *tmplist;

	while(listA){
		tmplist = listB;
		while(tmplist){
			if(listA->node == tmplist->node)
				break;
			tmplist = tmplist->next_entity;
		}
		if(tmplist == NULL)
			return 0;
		listA = listA->next_entity;
	}
	return 1;	
}

struct state_info* BuildDFA(ptrThTreeNode NFA,int *IstateID)
{
	//struct state_info *DFA;
	struct state_info *Istate;
	struct th_tree_link *tmptlink;
	ptrThTreeNode finalNode;
	char* symstr;
	int numOfChar, i;

	EpsClosure();
	//show_nodes_info();
	states = NULL;
	//set initial DFA states
	symstr = FindAllSymInNFA(&numOfChar);
	
	//numOfChar = strlen(symstr);
if(_NonPrefix){
	for(i=0; i<numOfChar; i++){
		tmptlink = (struct th_tree_link *)rtlglue_malloc(sizeof(Th_Tree_Node));
		if(tmptlink==NULL)
			rtlglue_printf("tmplink malloc fail!\n");
		tmptlink->symbol = symstr[i];
		tmptlink->linkedNode = NFA;
		tmptlink->nextLink = NFA->outlink;
		NFA->outlink = tmptlink;
	}
}
	free(symstr);

	Istate =  createState();
	Istate->nodelist = nodelist_Union(Istate->nodelist, NFA->cn);
	finalNode = NFA->final_node;
	//set final DFA states
	//set all DFA states
	*IstateID = Istate->stateID;
	states = Istate;

	BuildState(Istate);
	SetFinalState(states, finalNode);

	return states;
}
int findSymInArray(char* charArray, char sym, int numOfCharInArray)
{
	int i;
	for(i=0; i<numOfCharInArray; i++){
		if(charArray[i] == sym)
			return 1;
	}
	return 0;
}

char* FindAllSymInNFA(int *retNumOfChar)
{	
	char* charArray;
	struct node_list* tmplist;
	struct th_tree_link *tmplink;
	int index;

	tmplist = nodelist;
	charArray = (char*)rtlglue_malloc(256*sizeof(char));
	if(charArray==NULL)
		rtlglue_printf("charArray malloc fail!\n");
	charArray[0]='\0';
	index = 0;
	while(tmplist){
		tmplink = tmplist->node->outlink;
		while(tmplink){
			if(tmplink->isElink != 1){
				if(!findSymInArray(charArray,tmplink->symbol,index)){
					charArray[index] = tmplink->symbol;
					index++;
					//charArray[index]='\0';
				}		
			}	
			tmplink = tmplink->nextLink;
		}	
		tmplist = tmplist->next_entity;
	}
	if(index>256)
		rtlglue_printf("The number of all symbols are too large, Function %s, line %d\n",__FUNCTION__, __LINE__);

	*retNumOfChar = index;
	return charArray;
}

void SetFinalState(struct state_info* statelist,ptrThTreeNode finalNode)
{
	struct node_list* tmplist;

	while(statelist){
		tmplist = statelist->nodelist;
		statelist->isfinal = 0;
		while(tmplist){
			if(tmplist->node==finalNode){
				statelist->isfinal = 1;
				break;
			}
			tmplist = tmplist->next_entity;
		}
		statelist = statelist->nextstate;
	}
}
void show_states(struct state_info* statelist)
{	
	struct node_list *cntmp;
	ptrThStateLink tmpstatelink;
	while(statelist){
		cntmp = statelist->nodelist;
		rtlglue_printf("state %d node: ",statelist->stateID);
		while(cntmp){
			rtlglue_printf(" %d", cntmp->node->nodeID);
			cntmp = cntmp->next_entity;
		}
		rtlglue_printf("\n");
		rtlglue_printf("isfinal = %d\n",statelist->isfinal);
		tmpstatelink =(ptrThStateLink)statelist->outlink;
		rtlglue_printf("state link: \n");
		while(tmpstatelink){
			rtlglue_printf("==%c==> %d\n", tmpstatelink->symbol, tmpstatelink->linkedState->stateID);
			tmpstatelink = (ptrThStateLink)tmpstatelink->nextLink;
		}
		statelist = statelist->nextstate;
	}

}

void show_states_nonchar(struct state_info* statelist)
{	
	struct node_list *cntmp;
	ptrThStateLink tmpstatelink;
	while(statelist){
		cntmp = statelist->nodelist;
		rtlglue_printf("state %d node: ",statelist->stateID);
		while(cntmp){
			rtlglue_printf(" %d", cntmp->node->nodeID);
			cntmp = cntmp->next_entity;
		}
		rtlglue_printf("\n");
		rtlglue_printf("isfinal = %d\n",statelist->isfinal);
		tmpstatelink =(ptrThStateLink)statelist->outlink;
		rtlglue_printf("state link: \n");
		while(tmpstatelink){
			rtlglue_printf("==%x==> %d\n", tmpstatelink->symbol, tmpstatelink->linkedState->stateID);
			tmpstatelink = (ptrThStateLink)tmpstatelink->nextLink;
		}
		statelist = statelist->nextstate;
	}
}


struct node_list* nodelist_Union(struct node_list* listA, struct node_list* listB)
{
	struct node_list* listtmp;
	struct node_list* found;
	
	if(listB != NULL){
		while(listB){
			found = listA;
			while(found){
				if(found->node == listB->node)
					break;
				found = found->next_entity;
			}
			if(!found){//if we don't find it in listA, we add it into listA.
				listtmp = NULL;
				listtmp = (struct node_list*)rtlglue_malloc(sizeof(struct node_list));
				if(listtmp){
					listtmp->node = listB->node;
					listtmp->next_entity =listA;
					listA = listtmp;
				}
				else
					rtlglue_printf("node list malloc fail, function %s, line %d\n",__FUNCTION__, __LINE__);	
			}
			listB = listB->next_entity;
		}
	}

	return listA;
}	
struct state_info* createState()
{
	struct state_info *retState;
	
	retState = (struct state_info *)rtlglue_malloc(sizeof(struct state_info));
	if(!retState)
		rtlglue_printf("state info malloc fail, function %s, line %d\n",__FUNCTION__, __LINE__);

	retState->nodelist = NULL;
	retState->outlink = NULL;
	retState->nextstate = NULL;
	retState->isfinal = 0;
	retState->stateID = stateID;
	stateID++;

	return retState;
}

void freeStates(struct state_info *statelist)
{
	struct state_info *targetstate;
	ptrThStateLink tmplink;
	ptrThStateLink targetlink;

	while(statelist){
		tmplink = (ptrThStateLink)statelist->outlink;
		while(tmplink){	
			targetlink = tmplink;
			tmplink =(ptrThStateLink)tmplink->nextLink;
			free(targetlink);	
		}
		targetstate = statelist;
		statelist = statelist->nextstate;
		free(targetstate);
	}
}
