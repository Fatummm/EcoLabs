#ifndef __C_ECO_LAB2_H__
#define __C_ECO_LAB2_H__

#include "IKD_Tree.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoLab2* CEcoLab2Ptr_t;

typedef struct CEcoLab2 {

    /* Primary interface — IKD_Tree delegated through containment/aggregation */
    IKD_TreeVTbl* m_pVTblIKD_Tree;

    /* Delegating vtables for contained calculators */
    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;
    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;

    /* Aggregated Lab1: non-delegating unknown and interface */
    IEcoUnknown* m_pInnerUnkLab1;
    IKD_Tree* m_pInnerLab1;

    /* Contained component E (IEcoCalculatorY; E aggregates B for IEcoCalculatorX) */
    IEcoCalculatorY* m_pInnerCalculatorY;

    /* Contained component B (IEcoCalculatorX) */
    IEcoCalculatorX* m_pInnerCalculatorX;

} CEcoLab2;

int16_t ECOCALLMETHOD CEcoLab2_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IKD_Tree** ppIKD_Tree);

#endif
