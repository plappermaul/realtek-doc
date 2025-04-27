#ifndef __TH_AUTO_H__
#define __TH_AUTO_H__
#include "ptree.h"
//Intial node is the node itself
//final_node points to the final node;
typedef struct th_tree_node{
	struct th_tree_link *outlink;
	struct th_tree_node *final_node;//used for operations.
	int marked;
	int nodeID;
	struct node_list  *cn;
} Th_Tree_Node;

typedef Th_Tree_Node *ptrThTreeNode;

struct node_list{
        ptrThTreeNode node;
        struct node_list *next_entity;
};

typedef struct th_tree_link{
	char symbol;
	ptrThTreeNode linkedNode;
	int isElink;
	struct th_tree_link* nextLink;
} Th_Tree_Link;

typedef Th_Tree_Link *ptrThTreeLink;

struct state_info{
        struct node_list *nodelist;
        struct Th_State_Link *outlink;
        struct state_info *nextstate;
	int isfinal;
	int stateID;
};

typedef struct th_state_link{
	char symbol;
	struct state_info *linkedState;
	struct Th_State_Link* nextLink;
} Th_State_Link;

typedef Th_State_Link *ptrThStateLink;

void setPrefix(void);
void unsetPrefix(void);
void Th_Auto_init(void);
void freeTh_Auto(void);
void show_nodes_info(void);
ptrThTreeNode Th_Op_One(void);
ptrThTreeNode Th_Op_Two(char nodeSymbel);
ptrThTreeNode Th_Op_Three(ptrThTreeNode v1,ptrThTreeNode v2);
ptrThTreeNode Th_Op_Four(ptrThTreeNode v1,ptrThTreeNode v2);
ptrThTreeNode Th_Op_Five(ptrThTreeNode v);
ptrThTreeNode Thompson_reucr(ptrNode v);

void EpsClosure(void);
void clearMarked(void);
ptrThTreeNode Th_Node_Pop(void);
void Th_Node_Push(ptrThTreeNode node);
void Add_Closure_Entity(struct node_list *nodecn, ptrThTreeNode node);

int findSymInArray(char* charArray, char sym, int numOfCharInArray);
char* FindAllSymInNFA(int *retNumOfChar);
void SetFinalState(struct state_info* statelist,ptrThTreeNode finalNode);
void show_states(struct state_info* statelist);
void show_states_nonchar(struct state_info* statelist);
struct state_info* createState(void);
struct node_list* nodelist_Union(struct node_list* listA, struct node_list* listB);
struct state_info* BuildDFA(ptrThTreeNode NFA, int *IstateID);
int isBelong(struct node_list *listA, struct node_list *listB);
struct state_info* findState(struct state_info *T, struct state_info *statelist);
void BuildState(struct state_info *State);
#endif
