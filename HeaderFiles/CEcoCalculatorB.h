#ifndef __C_ECO_CALCULATOR_B_H__
#define __C_ECO_CALCULATOR_B_H__

#include "IEcoCalculatorX.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoCalculatorB* CEcoCalculatorBPtr_t;

typedef struct CEcoCalculatorB {

    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;

    IEcoUnknownVTbl* m_pVTblINondelegatingUnk;

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;
    IEcoUnknown* m_pIUnkOuter;

} CEcoCalculatorB;

int16_t ECOCALLMETHOD CEcoCalculatorB_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoUnknown** ppIUnk);

#endif
