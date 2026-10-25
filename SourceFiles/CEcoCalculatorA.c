/*
 * CEcoCalculatorA — Component A
 * Implements IEcoCalculatorX (Addition, Subtraction)
 * Does NOT support aggregation
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoCalculatorA.h"
#include "IdEcoCalculatorA.h"

static int16_t ECOCALLMETHOD CEcoCalculatorA_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, void** ppv) {
    CEcoCalculatorA* pCMe = (CEcoCalculatorA*)me;

    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    if (IsEqualUGUID(riid, &IID_IEcoCalculatorX) || IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoCalculatorX->AddRef((IEcoCalculatorX*)pCMe);
    } else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }

    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorA_AddRef(IEcoCalculatorXPtr_t me) {
    CEcoCalculatorA* pCMe = (CEcoCalculatorA*)me;
    if (me == 0) return -1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorA_Release(IEcoCalculatorXPtr_t me) {
    CEcoCalculatorA* pCMe = (CEcoCalculatorA*)me;
    if (me == 0) return -1;
    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        IEcoMemoryAllocator1* pIMem = pCMe->m_pIMem;
        if (pCMe->m_pISys != 0) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    return pCMe->m_cRef;
}

static int32_t ECOCALLMETHOD CEcoCalculatorA_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    return a + b;
}

static int32_t ECOCALLMETHOD CEcoCalculatorA_Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    return a - b;
}

static IEcoCalculatorXVTbl g_xCEcoCalculatorAVTbl = {
    CEcoCalculatorA_QueryInterface,
    CEcoCalculatorA_AddRef,
    CEcoCalculatorA_Release,
    CEcoCalculatorA_Addition,
    CEcoCalculatorA_Subtraction
};

static int16_t CEcoCalculatorA_GetMemoryAllocator(IEcoSystem1* pISys, IEcoMemoryAllocator1** ppIMem) {
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

int16_t ECOCALLMETHOD CEcoCalculatorA_CreateInstance(IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter, IEcoCalculatorX** ppIX) {
    CEcoCalculatorA* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    int16_t result;

    if (ppIX == 0 || pIUnkSystem == 0) return ERR_ECO_POINTER;
    *ppIX = 0;

    /* Component A does not support aggregation */
    if (pIUnkOuter != 0) return ERR_ECO_NOAGGREGATION;

    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) return ERR_ECO_NOSYSTEM;

    result = CEcoCalculatorA_GetMemoryAllocator(pISys, &pIMem);
    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    pCMe = (CEcoCalculatorA*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoCalculatorA));
    if (pCMe == 0) {
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    pCMe->m_pVTblIEcoCalculatorX = &g_xCEcoCalculatorAVTbl;
    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_pISys = pISys;

    *ppIX = (IEcoCalculatorX*)pCMe;
    return ERR_ECO_SUCCESS;
}
