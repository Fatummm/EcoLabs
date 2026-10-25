/*
 * CEcoCalculatorE — Component E
 * Implements IEcoCalculatorY (Multiplication, Division)
 * Uses AGGREGATION of component B for IEcoCalculatorX
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoCalculatorE.h"
#include "CEcoCalculatorB.h"
#include "IdEcoCalculatorE.h"

/* --- IEcoCalculatorY (primary interface) --- */

static int16_t ECOCALLMETHOD CEcoCalculatorE_QueryInterface(IEcoCalculatorYPtr_t me, const UGUID* riid, void** ppv) {
    CEcoCalculatorE* pCMe = (CEcoCalculatorE*)me;

    if (me == 0 || riid == 0 || ppv == 0) return ERR_ECO_POINTER;

    if (IsEqualUGUID(riid, &IID_IEcoCalculatorY) || IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoCalculatorY->AddRef((IEcoCalculatorY*)pCMe);
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        /* Aggregation: delegate to inner B's non-delegating QI */
        return pCMe->m_pInnerUnkB->pVTbl->QueryInterface(pCMe->m_pInnerUnkB, riid, ppv);
    } else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorE_AddRef(IEcoCalculatorYPtr_t me) {
    CEcoCalculatorE* pCMe = (CEcoCalculatorE*)me;
    if (me == 0) return -1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorE_Release(IEcoCalculatorYPtr_t me) {
    CEcoCalculatorE* pCMe = (CEcoCalculatorE*)me;
    if (me == 0) return -1;
    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        IEcoMemoryAllocator1* pIMem = pCMe->m_pIMem;
        if (pCMe->m_pInnerUnkB != 0) {
            pCMe->m_pInnerUnkB->pVTbl->Release(pCMe->m_pInnerUnkB);
        }
        if (pCMe->m_pISys != 0) pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    return pCMe->m_cRef;
}

static int32_t ECOCALLMETHOD CEcoCalculatorE_Multiplication(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    return a * b;
}

static int32_t ECOCALLMETHOD CEcoCalculatorE_Division(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    if (b == 0) return 0;
    return a / b;
}

static IEcoCalculatorYVTbl g_xCEcoCalculatorEVTbl = {
    CEcoCalculatorE_QueryInterface,
    CEcoCalculatorE_AddRef,
    CEcoCalculatorE_Release,
    CEcoCalculatorE_Multiplication,
    CEcoCalculatorE_Division
};

static int16_t CEcoCalculatorE_GetMemoryAllocator(IEcoSystem1* pISys, IEcoMemoryAllocator1** ppIMem) {
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    int16_t result;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    if (pISys == 0 || ppIMem == 0) return ERR_ECO_POINTER;
    *ppIMem = 0;

    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus);
    if (result != 0 || pIBus == 0) return ERR_ECO_NOBUS;

    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**)ppIMem);
    pIBus->pVTbl->Release(pIBus);

    if (result != 0 || *ppIMem == 0) {
        *ppIMem = 0;
        return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }
    return result;
}

int16_t ECOCALLMETHOD CEcoCalculatorE_CreateInstance(IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter, IEcoCalculatorY** ppIY) {
    CEcoCalculatorE* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoUnknown* pInnerUnkB = 0;
    int16_t result;

    if (ppIY == 0 || pIUnkSystem == 0) return ERR_ECO_POINTER;
    *ppIY = 0;

    if (pIUnkOuter != 0) return ERR_ECO_NOAGGREGATION;

    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) return ERR_ECO_NOSYSTEM;

    result = CEcoCalculatorE_GetMemoryAllocator(pISys, &pIMem);
    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    pCMe = (CEcoCalculatorE*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoCalculatorE));
    if (pCMe == 0) {
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    pCMe->m_pVTblIEcoCalculatorY = &g_xCEcoCalculatorEVTbl;
    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_pISys = pISys;
    pCMe->m_pInnerUnkB = 0;

    /* Create aggregated component B, passing our IEcoUnknown as pIUnkOuter */
    result = CEcoCalculatorB_CreateInstance(pIUnkSystem, (IEcoUnknown*)pCMe, &pInnerUnkB);
    if (result != 0 || pInnerUnkB == 0) {
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return result;
    }
    pCMe->m_pInnerUnkB = pInnerUnkB;

    *ppIY = (IEcoCalculatorY*)pCMe;
    return ERR_ECO_SUCCESS;
}
