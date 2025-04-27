#include "stateMachine.h"

#ifndef __AHO_H__
#define __AHO_H__
/*
 *Aho-Corasick key trie node data structure.
 *
 */
typedef struct keytreenode{
struct keytreenode* children;
struct keytreenode* parent;
struct keytreenode* nextSibling;
struct keytreenode* nextNode;
int nodeID;
int outputIndex;
int failIndex;
char nodeChar;
} keyTreeNode;

SM* aho_MakeStateMachine(char* patList[], int numOfPatterns);
#endif
