#ifndef __C_ECO_CALCULATOR_D_H__
#define __C_ECO_CALCULATOR_D_H__

#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoCalculatorD* CEcoCalculatorDPtr_t;

typedef struct CEcoCalculatorD {

    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;

    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;

    /* Contained component A */
    IEcoCalculatorX* m_pInnerCalculatorA;

} CEcoCalculatorD;

int16_t ECOCALLMETHOD CEcoCalculatorD_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoCalculatorY** ppIY);

#endif
