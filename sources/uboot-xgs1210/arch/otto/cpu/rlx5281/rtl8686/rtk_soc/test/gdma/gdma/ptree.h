#ifndef __PTREE_H__
#define __PTREE_H__
#define INVALIED_OPERATION 0
#define DOT_OPERATION 1
#define OR_OPERATION 2
#define STAR_OPERATION 3
/*
 * The parse tree node data structure.
 * */
struct parsenode{
	char nodeChar;
	int operation;
	struct parsenode  *rightNode;
	struct parsenode  * leftNode;
};

typedef struct parsenode *ptrNode;
/*char operators[]=".|*()";*/

ptrNode Parse(char* reg, int *last,int regEnd);
void freeTree(ptrNode v);
char* insertDot(char* reg);
int isCharactor(char c);
ptrNode CreatePNode(char c);
ptrNode MergeWithOp(char op, ptrNode r1, ptrNode r2);
void freeTree(ptrNode v);

#endif
