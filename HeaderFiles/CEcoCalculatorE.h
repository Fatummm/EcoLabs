#ifndef __C_ECO_CALCULATOR_E_H__
#define __C_ECO_CALCULATOR_E_H__

#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoCalculatorE* CEcoCalculatorEPtr_t;

typedef struct CEcoCalculatorE {

    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;

    /* Aggregated component B: non-delegating unknown */
    IEcoUnknown* m_pInnerUnkB;

} CEcoCalculatorE;

int16_t ECOCALLMETHOD CEcoCalculatorE_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoCalculatorY** ppIY);

#endif
