#ifndef __C_ECO_CALCULATOR_A_H__
#define __C_ECO_CALCULATOR_A_H__

#include "IEcoCalculatorX.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoCalculatorA* CEcoCalculatorAPtr_t;

typedef struct CEcoCalculatorA {

    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;

} CEcoCalculatorA;

int16_t ECOCALLMETHOD CEcoCalculatorA_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoCalculatorX** ppIX);

#endif
