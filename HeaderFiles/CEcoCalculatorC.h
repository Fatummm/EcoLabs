#ifndef __C_ECO_CALCULATOR_C_H__
#define __C_ECO_CALCULATOR_C_H__

#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoCalculatorC* CEcoCalculatorCPtr_t;

typedef struct CEcoCalculatorC {

    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;

    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;

} CEcoCalculatorC;

int16_t ECOCALLMETHOD CEcoCalculatorC_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoCalculatorX** ppIX);

#endif
