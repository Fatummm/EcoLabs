#ifndef __C_KD_TREE_H__
#define __C_KD_TREE_H__

#include "IKD_Tree.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CKD_Tree_B0ABC511* CKD_Tree_B0ABC511Ptr_t;

typedef struct CKD_Tree_B0ABC511 {

    /* IKD_Tree interface function table (primary, offset 0) */
    IKD_TreeVTbl* m_pVTblIKD_Tree;

    /* Non-delegating IEcoUnknown for aggregation support */
    IEcoUnknownVTbl* m_pVTblINondelegatingUnk;

    /* Delegating IEcoCalculatorX (containment of component A) */
    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;

    /* Delegating IEcoCalculatorY (containment of component D) */
    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;

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

    /* Outer unknown (for aggregation support) */
    IEcoUnknown* m_pIUnkOuter;

    /* Instance data */
    char_t* m_Name;

    uint16_t m_dimension;
    KDNodePtr_t m_pRoot;
    uint32_t m_pointCount;
    /* Coordinate element type (KD_TYPE_*) */
    int16_t m_type;

    /* Contained component A — IEcoCalculatorX */
    IEcoCalculatorX* m_pInnerCalculatorX;

    /* Contained component D — IEcoCalculatorY (D contains A internally) */
    IEcoCalculatorY* m_pInnerCalculatorY;

} CKD_Tree_B0ABC511;

/* Creates a new instance of the component; free it with Release */
int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IKD_Tree** ppIKD_Tree);

/* Creates a new instance; returns non-delegating IEcoUnknown for aggregation */
int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_CreateInstanceAggr(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoUnknown** ppIUnk);

#endif /* __C_KD_TREE_H__ */
