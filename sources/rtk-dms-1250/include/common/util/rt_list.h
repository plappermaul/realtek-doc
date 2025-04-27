/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : linked list utility
 *
 * Feature :
 *
 */

#ifndef __COMMON_RT_LIST_H__
#define __COMMON_RT_LIST_H__

/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */
/* Doubly-linked list related macros */
#define RTK_LIST_DEF(_structName, _listName)    \
struct {                                        \
    struct _structName *pHead;                  \
    struct _structName *pTail;                  \
    uint32 length;                              \
} _listName

#define RTK_LIST_INIT(_pList)   \
do {                            \
    (_pList)->pHead = NULL;     \
    (_pList)->pTail = NULL;     \
    (_pList)->length = 0;       \
} while (0)

#define RTK_LIST_NODE_REF_DEF(_structName, _refField)   \
struct {                                                \
    struct _structName *pPrev;                          \
    struct _structName *pNext;                          \
} _refField

#define RTK_LIST_NODE_REF_INIT(_pNode, _refField)   \
do {                                                \
    (_pNode)->_refField.pPrev = NULL;               \
    (_pNode)->_refField.pNext = NULL;               \
} while (0)

#define RTK_LIST_EMPTY(_pList)      ((_pList)->length == 0)
#define RTK_LIST_LENGTH(_pList)     ((_pList)->length)


#define RTK_LIST_NODE_INSERT_BEFORE(_pList, _pBase, _pNode, _refField)  \
do {                                                                    \
    (_pNode)->_refField.pPrev = (_pBase)->_refField.pPrev;              \
    (_pNode)->_refField.pNext = (_pBase);                               \
    if ((_pBase)->_refField.pPrev == NULL)                              \
    {                                                                   \
        (_pBase)->_refField.pPrev = (_pNode);                           \
        (_pList)->pHead = (_pNode);                                     \
    } else {                                                            \
        (_pBase)->_refField.pPrev->_refField.pNext = (_pNode);          \
        (_pBase)->_refField.pPrev = (_pNode);                           \
    }                                                                   \
    (_pList)->length ++;                                                \
} while (0)

#define RTK_LIST_NODE_INSERT_AFTER(_pList, _pBase, _pNode, _refField)   \
do {                                                                    \
    (_pNode)->_refField.pPrev = (_pBase);                               \
    (_pNode)->_refField.pNext = (_pBase)->_refField.pNext;              \
    if ((_pBase)->_refField.pNext == NULL)                              \
    {                                                                   \
        (_pBase)->_refField.pNext = (_pNode);                           \
        (_pList)->pTail = (_pNode);                                     \
    } else {                                                            \
        (_pBase)->_refField.pNext->_refField.pPrev = (_pNode);          \
        (_pBase)->_refField.pNext = (_pNode);                           \
    }                                                                   \
    (_pList)->length ++;                                                \
} while (0)

#define RTK_LIST_NODE_INSERT_HEAD(_pList, _pNode, _refField)                        \
do {                                                                                \
    if ((_pList)->pHead == NULL)                                                    \
    {                                                                               \
        (_pList)->pHead = (_pNode);                                                 \
        (_pList)->pTail = (_pNode);                                                 \
        (_pNode)->_refField.pPrev = NULL;                                           \
        (_pNode)->_refField.pNext = NULL;                                           \
        (_pList)->length ++;                                                        \
    } else {                                                                        \
        RTK_LIST_NODE_INSERT_BEFORE(_pList, (_pList)->pHead, _pNode, _refField);    \
    }                                                                               \
} while (0)

#define RTK_LIST_NODE_INSERT_TAIL(_pList, _pNode, _refField)                    \
do {                                                                            \
    if ((_pList)->pTail == NULL)                                                \
    {                                                                           \
        RTK_LIST_NODE_INSERT_HEAD(_pList, _pNode, _refField);                   \
    } else {                                                                    \
        RTK_LIST_NODE_INSERT_AFTER(_pList, (_pList)->pTail, _pNode, _refField); \
    }                                                                           \
} while (0)

#define RTK_LIST_NODE_REMOVE(_pList, _pNode, _refField)                         \
do {                                                                            \
    if ((_pNode)->_refField.pPrev == NULL)                                      \
    {                                                                           \
        (_pList)->pHead = (_pNode)->_refField.pNext;                            \
    } else {                                                                    \
        (_pNode)->_refField.pPrev->_refField.pNext = (_pNode)->_refField.pNext; \
    }                                                                           \
    if ((_pNode)->_refField.pNext == NULL)                                      \
    {                                                                           \
        (_pList)->pTail = (_pNode)->_refField.pPrev;                            \
    } else {                                                                    \
        (_pNode)->_refField.pNext->_refField.pPrev = (_pNode)->_refField.pPrev; \
    }                                                                           \
    (_pList)->length --;                                                        \
} while (0)

#define RTK_LIST_NODE_HEAD(_pList)              ((_pList)->pHead)
#define RTK_LIST_NODE_TAIL(_pList)              ((_pList)->pTail)
#define RTK_LIST_NODE_PREV(_pNode, _refField)    ((_pNode)->_refField.pPrev)
#define RTK_LIST_NODE_NEXT(_pNode, _refField)    ((_pNode)->_refField.pNext)

#define RTK_LIST_FOREACH(_pList, _pNode, _refField)         \
for ((_pNode)=RTK_LIST_NODE_HEAD(_pList);                   \
     (_pNode);                                              \
     (_pNode)=RTK_LIST_NODE_NEXT(_pNode, _refField))

#define RTK_LIST_FOREACH_REVERSE(_pList, _pNode, _refField) \
for ((_pNode)=RTK_LIST_NODE_TAIL(_pList);                   \
     (_pNode);                                              \
     (_pNode)=RTK_LIST_NODE_PREV(_pNode, _refField))

/*
 * these macros are safe for removing node during iteration
 */
#define RTK_LIST_FOREACH_SAFE(_pList, _pNode, _pTemp, _refField)        \
for ((_pNode)=RTK_LIST_NODE_HEAD(_pList);                               \
     (_pNode) && ((_pTemp) = RTK_LIST_NODE_NEXT(_pNode, _refField), 1); \
     (_pNode)=(_pTemp))

#define RTK_LIST_FOREACH_REVERSE_SAFE(_pList, _pNode, _pTemp, _refField)    \
for ((_pNode)=RTK_LIST_NODE_TAIL(_pList);                                   \
     (_pNode) && ((_pTemp) = RTK_LIST_NODE_PREV(_pNode, _refField), 1);     \
     (_pNode)=(_pTemp))


/*
 * Function Declaration
 */

#endif /* __COMMON_RT_LIST_H__ */

