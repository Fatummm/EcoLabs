/*
 * CEcoCalculatorB — Component B
 * Implements IEcoCalculatorX (Addition, Subtraction)
 * SUPPORTS aggregation (NonDelegating IEcoUnknown)
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoCalculatorB.h"
#include "IdEcoCalculatorB.h"

#include <stddef.h>

/* --- Non-delegating IEcoUnknown --- */

static int16_t ECOCALLMETHOD CEcoCalculatorB_NondelegatingQI(IEcoUnknownPtr_t me, const UGUID* riid, void** ppv) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)((char_t*)me - offsetof(CEcoCalculatorB, m_pVTblINondelegatingUnk));

    if (me == 0 || riid == 0 || ppv == 0) return ERR_ECO_POINTER;

    if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoCalculatorX->AddRef((IEcoCalculatorX*)pCMe);
    } else if (IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblINondelegatingUnk;
        ((IEcoUnknown*)*ppv)->pVTbl->AddRef((IEcoUnknown*)*ppv);
    } else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorB_NondelegatingAddRef(IEcoUnknownPtr_t me) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)((char_t*)me - offsetof(CEcoCalculatorB, m_pVTblINondelegatingUnk));
    if (me == 0) return -1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorB_NondelegatingRelease(IEcoUnknownPtr_t me) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)((char_t*)me - offsetof(CEcoCalculatorB, m_pVTblINondelegatingUnk));
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

static IEcoUnknownVTbl g_xCEcoCalculatorBNondelegatingUnkVTbl = {
    CEcoCalculatorB_NondelegatingQI,
    CEcoCalculatorB_NondelegatingAddRef,
    CEcoCalculatorB_NondelegatingRelease
};

/* --- Delegating IEcoCalculatorX (delegates QI/AddRef/Release to pIUnkOuter) --- */

static int16_t ECOCALLMETHOD CEcoCalculatorB_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, void** ppv) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)me;
    if (me == 0) return ERR_ECO_POINTER;
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoCalculatorB_AddRef(IEcoCalculatorXPtr_t me) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)me;
    if (me == 0) return -1;
    return pCMe->m_pIUnkOuter->pVTbl->AddRef(pCMe->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoCalculatorB_Release(IEcoCalculatorXPtr_t me) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)me;
    if (me == 0) return -1;
    return pCMe->m_pIUnkOuter->pVTbl->Release(pCMe->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CEcoCalculatorB_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    return a + b;
}

static int32_t ECOCALLMETHOD CEcoCalculatorB_Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    return a - b;
}

static IEcoCalculatorXVTbl g_xCEcoCalculatorBVTbl = {
    CEcoCalculatorB_QueryInterface,
    CEcoCalculatorB_AddRef,
    CEcoCalculatorB_Release,
    CEcoCalculatorB_Addition,
    CEcoCalculatorB_Subtraction
};

static int16_t CEcoCalculatorB_GetMemoryAllocator(IEcoSystem1* pISys, IEcoMemoryAllocator1** ppIMem) {
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

int16_t ECOCALLMETHOD CEcoCalculatorB_CreateInstance(IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter, IEcoUnknown** ppIUnk) {
    CEcoCalculatorB* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    int16_t result;

    if (ppIUnk == 0 || pIUnkSystem == 0) return ERR_ECO_POINTER;
    *ppIUnk = 0;

    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) return ERR_ECO_NOSYSTEM;

    result = CEcoCalculatorB_GetMemoryAllocator(pISys, &pIMem);
    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    pCMe = (CEcoCalculatorB*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoCalculatorB));
    if (pCMe == 0) {
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    pCMe->m_pVTblIEcoCalculatorX = &g_xCEcoCalculatorBVTbl;
    pCMe->m_pVTblINondelegatingUnk = &g_xCEcoCalculatorBNondelegatingUnkVTbl;
    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_pISys = pISys;

    if (pIUnkOuter != 0) {
        pCMe->m_pIUnkOuter = pIUnkOuter;
    } else {
        pCMe->m_pIUnkOuter = (IEcoUnknown*)&pCMe->m_pVTblINondelegatingUnk;
    }

    /* Return non-delegating unknown */
    *ppIUnk = (IEcoUnknown*)&pCMe->m_pVTblINondelegatingUnk;
    return ERR_ECO_SUCCESS;
}
