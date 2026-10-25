/*
 * CEcoCalculatorD — Component D
 * Implements IEcoCalculatorY (Multiplication, Division)
 * Uses CONTAINMENT of component A for IEcoCalculatorX
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoCalculatorD.h"
#include "CEcoCalculatorA.h"
#include "IdEcoCalculatorD.h"

#include <stddef.h>

/* --- IEcoCalculatorY (primary interface) --- */

static int16_t ECOCALLMETHOD CEcoCalculatorD_Y_QueryInterface(IEcoCalculatorYPtr_t me, const UGUID* riid, void** ppv) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)me;

    if (me == 0 || riid == 0 || ppv == 0) return ERR_ECO_POINTER;

    if (IsEqualUGUID(riid, &IID_IEcoCalculatorY) || IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoCalculatorY->AddRef((IEcoCalculatorY*)pCMe);
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        /* Containment: return our delegating vtable */
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoCalculatorY->AddRef((IEcoCalculatorY*)pCMe);
    } else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorD_Y_AddRef(IEcoCalculatorYPtr_t me) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)me;
    if (me == 0) return -1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoCalculatorD_Y_Release(IEcoCalculatorYPtr_t me) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)me;
    if (me == 0) return -1;
    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        IEcoMemoryAllocator1* pIMem = pCMe->m_pIMem;
        if (pCMe->m_pInnerCalculatorA != 0) {
            pCMe->m_pInnerCalculatorA->pVTbl->Release(pCMe->m_pInnerCalculatorA);
        }
        if (pCMe->m_pISys != 0) pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    return pCMe->m_cRef;
}

static int32_t ECOCALLMETHOD CEcoCalculatorD_Multiplication(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    return a * b;
}

static int32_t ECOCALLMETHOD CEcoCalculatorD_Division(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    if (b == 0) return 0;
    return a / b;
}

static IEcoCalculatorYVTbl g_xCEcoCalculatorDYVTbl = {
    CEcoCalculatorD_Y_QueryInterface,
    CEcoCalculatorD_Y_AddRef,
    CEcoCalculatorD_Y_Release,
    CEcoCalculatorD_Multiplication,
    CEcoCalculatorD_Division
};

/* --- IEcoCalculatorX (delegating to contained component A) --- */

static int16_t ECOCALLMETHOD CEcoCalculatorD_X_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, void** ppv) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
    return CEcoCalculatorD_Y_QueryInterface((IEcoCalculatorY*)pCMe, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoCalculatorD_X_AddRef(IEcoCalculatorXPtr_t me) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
    return CEcoCalculatorD_Y_AddRef((IEcoCalculatorY*)pCMe);
}

static uint32_t ECOCALLMETHOD CEcoCalculatorD_X_Release(IEcoCalculatorXPtr_t me) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
    return CEcoCalculatorD_Y_Release((IEcoCalculatorY*)pCMe);
}

static int32_t ECOCALLMETHOD CEcoCalculatorD_X_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorA->pVTbl->Addition(pCMe->m_pInnerCalculatorA, a, b);
}

static int32_t ECOCALLMETHOD CEcoCalculatorD_X_Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorA->pVTbl->Subtraction(pCMe->m_pInnerCalculatorA, a, b);
}

static IEcoCalculatorXVTbl g_xCEcoCalculatorDXVTbl = {
    CEcoCalculatorD_X_QueryInterface,
    CEcoCalculatorD_X_AddRef,
    CEcoCalculatorD_X_Release,
    CEcoCalculatorD_X_Addition,
    CEcoCalculatorD_X_Subtraction
};

static int16_t CEcoCalculatorD_GetMemoryAllocator(IEcoSystem1* pISys, IEcoMemoryAllocator1** ppIMem) {
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

int16_t ECOCALLMETHOD CEcoCalculatorD_CreateInstance(IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter, IEcoCalculatorY** ppIY) {
    CEcoCalculatorD* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoCalculatorX* pInnerA = 0;
    int16_t result;

    if (ppIY == 0 || pIUnkSystem == 0) return ERR_ECO_POINTER;
    *ppIY = 0;

    if (pIUnkOuter != 0) return ERR_ECO_NOAGGREGATION;

    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) return ERR_ECO_NOSYSTEM;

    result = CEcoCalculatorD_GetMemoryAllocator(pISys, &pIMem);
    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Create contained component A */
    result = CEcoCalculatorA_CreateInstance(pIUnkSystem, 0, &pInnerA);
    if (result != 0 || pInnerA == 0) {
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return result;
    }

    pCMe = (CEcoCalculatorD*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoCalculatorD));
    if (pCMe == 0) {
        pInnerA->pVTbl->Release(pInnerA);
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    pCMe->m_pVTblIEcoCalculatorY = &g_xCEcoCalculatorDYVTbl;
    pCMe->m_pVTblIEcoCalculatorX = &g_xCEcoCalculatorDXVTbl;
    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_pISys = pISys;
    pCMe->m_pInnerCalculatorA = pInnerA;

    *ppIY = (IEcoCalculatorY*)pCMe;
    return ERR_ECO_SUCCESS;
}
