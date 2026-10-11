/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CKD_Tree
 * </summary>
 *
 * <description>
 *   This header describes the implementation of the CKD_Tree component
 * </description>
 *
 * <author>
 *   Copyright (c) 2018 Dmitriy Lepa. All rights reserved.
 * </author>
 *
 */

#ifndef __C_KD_TREE_H__
#define __C_KD_TREE_H__

#include "IKD_Tree.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CKD_Tree_B0ABC511* CKD_Tree_B0ABC511Ptr_t;

typedef struct CKD_Tree_B0ABC511 {

    /* IKD_Tree interface function table */
    IKD_TreeVTbl* m_pVTblIKD_Tree;


    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CKD_Tree_B0ABC511Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);
    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CKD_Tree_B0ABC511Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);
    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CKD_Tree_B0ABC511Ptr_t pCMe);


    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* Instance data */
    char_t* m_Name;

    uint16_t m_dimension;
    KDNodePtr_t m_pRoot;
    uint32_t m_pointCount;
    /* Coordinate element type (KD_TYPE_*) */
    int16_t m_type;

} CKD_Tree_B0ABC511;

/* Creates a new instance of the component; free it with Release */
int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IKD_Tree** ppIKD_Tree);

#endif /* __C_KD_TREE_H__ */
