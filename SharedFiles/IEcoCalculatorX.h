#ifndef __I_ECO_CALCULATOR_X_H__
#define __I_ECO_CALCULATOR_X_H__

#include "IEcoBase1.h"

/* IEcoCalculatorX IID = {3B92D548-1E54-4D90-9C94-FB9C1E2A77CE} */
#ifndef __IID_IEcoCalculatorX
static const UGUID IID_IEcoCalculatorX = {0x01, 0x10, {0x3B, 0x92, 0xD5, 0x48, 0x1E, 0x54, 0x4D, 0x90, 0x9C, 0x94, 0xFB, 0x9C, 0x1E, 0x2A, 0x77, 0xCE}};
#endif

typedef struct IEcoCalculatorX* IEcoCalculatorXPtr_t;

typedef struct IEcoCalculatorXVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoCalculatorXPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoCalculatorXPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoCalculatorXPtr_t me);

    /* IEcoCalculatorX */
    int32_t (ECOCALLMETHOD *Addition)(/* in */ IEcoCalculatorXPtr_t me, /* in */ int32_t a, /* in */ int32_t b);
    int32_t (ECOCALLMETHOD *Subtraction)(/* in */ IEcoCalculatorXPtr_t me, /* in */ int32_t a, /* in */ int32_t b);

} IEcoCalculatorXVTbl, *IEcoCalculatorXVTblPtr_t;

interface IEcoCalculatorX {
    struct IEcoCalculatorXVTbl *pVTbl;
} IEcoCalculatorX;

#endif /* __I_ECO_CALCULATOR_X_H__ */
