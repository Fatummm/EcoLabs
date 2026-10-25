#ifndef __I_ECO_CALCULATOR_Y_H__
#define __I_ECO_CALCULATOR_Y_H__

#include "IEcoBase1.h"

/* IEcoCalculatorY IID = {5C3B5422-7218-4B29-94E3-8A8EAB9C7E40} */
#ifndef __IID_IEcoCalculatorY
static const UGUID IID_IEcoCalculatorY = {0x01, 0x10, {0x5C, 0x3B, 0x54, 0x22, 0x72, 0x18, 0x4B, 0x29, 0x94, 0xE3, 0x8A, 0x8E, 0xAB, 0x9C, 0x7E, 0x40}};
#endif

typedef struct IEcoCalculatorY* IEcoCalculatorYPtr_t;

typedef struct IEcoCalculatorYVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoCalculatorYPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoCalculatorYPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoCalculatorYPtr_t me);

    /* IEcoCalculatorY */
    int32_t (ECOCALLMETHOD *Multiplication)(/* in */ IEcoCalculatorYPtr_t me, /* in */ int32_t a, /* in */ int32_t b);
    int32_t (ECOCALLMETHOD *Division)(/* in */ IEcoCalculatorYPtr_t me, /* in */ int32_t a, /* in */ int32_t b);

} IEcoCalculatorYVTbl, *IEcoCalculatorYVTblPtr_t;

interface IEcoCalculatorY {
    struct IEcoCalculatorYVTbl *pVTbl;
} IEcoCalculatorY;

#endif /* __I_ECO_CALCULATOR_Y_H__ */
