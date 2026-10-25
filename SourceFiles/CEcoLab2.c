/*
 * CEcoLab2 — Component Eco.Lab2
 * Aggregates Lab1 (CKD_Tree) for IKD_Tree
 * Contains component B for IEcoCalculatorX
 * Contains component E for IEcoCalculatorY (E aggregates B)
 * Demonstrates combined containment/aggregation of all components
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab2.h"
#include "CKD_Tree.h"
#include "CEcoCalculatorB.h"
#include "CEcoCalculatorE.h"
#include "IdEcoLab2.h"

#include <stddef.h>

/* --- Delegating IKD_Tree (delegates all calls to aggregated Lab1) --- */

static int16_t ECOCALLMETHOD CEcoLab2_KDTree_QueryInterface(IKD_TreePtr_t me, const UGUID* riid, void** ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    if (me == 0 || riid == 0 || ppv == 0) return ERR_ECO_POINTER;

    if (IsEqualUGUID(riid, &IID_IKD_Tree) || IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIKD_Tree;
        ++pCMe->m_cRef;
        return ERR_ECO_SUCCESS;
    }

    if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        ++pCMe->m_cRef;
        return ERR_ECO_SUCCESS;
    }

    if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        ++pCMe->m_cRef;
        return ERR_ECO_SUCCESS;
    }

    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD CEcoLab2_KDTree_AddRef(IKD_TreePtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    if (me == 0) return -1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoLab2_KDTree_Release(IKD_TreePtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    if (me == 0) return -1;
    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        IEcoMemoryAllocator1* pIMem = pCMe->m_pIMem;
        /* Release aggregated Lab1 */
        if (pCMe->m_pInnerUnkLab1 != 0) {
            pCMe->m_pInnerUnkLab1->pVTbl->Release(pCMe->m_pInnerUnkLab1);
        }
        /* Release contained calculators */
        if (pCMe->m_pInnerCalculatorY != 0) {
            pCMe->m_pInnerCalculatorY->pVTbl->Release(pCMe->m_pInnerCalculatorY);
        }
        if (pCMe->m_pInnerCalculatorX != 0) {
            pCMe->m_pInnerCalculatorX->pVTbl->Release(pCMe->m_pInnerCalculatorX);
        }
        if (pCMe->m_pISys != 0) pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    return pCMe->m_cRef;
}

/* Delegate KD-tree methods to aggregated Lab1 */
static int16_t ECOCALLMETHOD CEcoLab2_BuildTree(IKD_TreePtr_t me, KDPointPtr_t* points, int32_t count, uint16_t dimension) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->BuildTree(pCMe->m_pInnerLab1, points, count, dimension);
}

static int16_t ECOCALLMETHOD CEcoLab2_SearchKNearest(IKD_TreePtr_t me, KDPointPtr_t point, int16_t k, KDSearchResult* result) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->SearchKNearest(pCMe->m_pInnerLab1, point, k, result);
}

static int16_t ECOCALLMETHOD CEcoLab2_DestroyTree(IKD_TreePtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->DestroyTree(pCMe->m_pInnerLab1);
}

static int16_t ECOCALLMETHOD CEcoLab2_InsertPoint(IKD_TreePtr_t me, KDPointPtr_t point) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->InsertPoint(pCMe->m_pInnerLab1, point);
}

static int16_t ECOCALLMETHOD CEcoLab2_DeletePoint(IKD_TreePtr_t me, KDPointPtr_t point) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->DeletePoint(pCMe->m_pInnerLab1, point);
}

static int16_t ECOCALLMETHOD CEcoLab2_BuildTreeInt(IKD_TreePtr_t me, KDPointPtr_t* points, int32_t count, uint16_t dimension) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->BuildTreeInt(pCMe->m_pInnerLab1, points, count, dimension);
}

static int16_t ECOCALLMETHOD CEcoLab2_BuildTreeLongLong(IKD_TreePtr_t me, KDPointPtr_t* points, int32_t count, uint16_t dimension) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->BuildTreeLongLong(pCMe->m_pInnerLab1, points, count, dimension);
}

static int16_t ECOCALLMETHOD CEcoLab2_BuildTreeFloat(IKD_TreePtr_t me, KDPointPtr_t* points, int32_t count, uint16_t dimension) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->BuildTreeFloat(pCMe->m_pInnerLab1, points, count, dimension);
}

static int16_t ECOCALLMETHOD CEcoLab2_BuildTreeDouble(IKD_TreePtr_t me, KDPointPtr_t* points, int32_t count, uint16_t dimension) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    return pCMe->m_pInnerLab1->pVTbl->BuildTreeDouble(pCMe->m_pInnerLab1, points, count, dimension);
}

static IKD_TreeVTbl g_xCEcoLab2KDTreeVTbl = {
    CEcoLab2_KDTree_QueryInterface,
    CEcoLab2_KDTree_AddRef,
    CEcoLab2_KDTree_Release,
    CEcoLab2_BuildTree,
    CEcoLab2_SearchKNearest,
    CEcoLab2_DestroyTree,
    CEcoLab2_InsertPoint,
    CEcoLab2_DeletePoint,
    CEcoLab2_BuildTreeInt,
    CEcoLab2_BuildTreeLongLong,
    CEcoLab2_BuildTreeFloat,
    CEcoLab2_BuildTreeDouble
};

/* --- Delegating IEcoCalculatorX (containment of component B) --- */

static int16_t ECOCALLMETHOD CEcoLab2_CalcX_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, void** ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorX));
    return CEcoLab2_KDTree_QueryInterface((IKD_Tree*)pCMe, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcX_AddRef(IEcoCalculatorXPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorX));
    return CEcoLab2_KDTree_AddRef((IKD_Tree*)pCMe);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcX_Release(IEcoCalculatorXPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorX));
    return CEcoLab2_KDTree_Release((IKD_Tree*)pCMe);
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcX_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorX->pVTbl->Addition(pCMe->m_pInnerCalculatorX, a, b);
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcX_Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorX->pVTbl->Subtraction(pCMe->m_pInnerCalculatorX, a, b);
}

static IEcoCalculatorXVTbl g_xCEcoLab2CalcXVTbl = {
    CEcoLab2_CalcX_QueryInterface,
    CEcoLab2_CalcX_AddRef,
    CEcoLab2_CalcX_Release,
    CEcoLab2_CalcX_Addition,
    CEcoLab2_CalcX_Subtraction
};

/* --- Delegating IEcoCalculatorY (containment of component E) --- */

static int16_t ECOCALLMETHOD CEcoLab2_CalcY_QueryInterface(IEcoCalculatorYPtr_t me, const UGUID* riid, void** ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorY));
    return CEcoLab2_KDTree_QueryInterface((IKD_Tree*)pCMe, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcY_AddRef(IEcoCalculatorYPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorY));
    return CEcoLab2_KDTree_AddRef((IKD_Tree*)pCMe);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcY_Release(IEcoCalculatorYPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorY));
    return CEcoLab2_KDTree_Release((IKD_Tree*)pCMe);
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcY_Multiplication(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorY));
    return pCMe->m_pInnerCalculatorY->pVTbl->Multiplication(pCMe->m_pInnerCalculatorY, a, b);
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcY_Division(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    CEcoLab2* pCMe = (CEcoLab2*)((char_t*)me - offsetof(CEcoLab2, m_pVTblIEcoCalculatorY));
    return pCMe->m_pInnerCalculatorY->pVTbl->Division(pCMe->m_pInnerCalculatorY, a, b);
}

static IEcoCalculatorYVTbl g_xCEcoLab2CalcYVTbl = {
    CEcoLab2_CalcY_QueryInterface,
    CEcoLab2_CalcY_AddRef,
    CEcoLab2_CalcY_Release,
    CEcoLab2_CalcY_Multiplication,
    CEcoLab2_CalcY_Division
};

/* --- CreateInstance --- */

static int16_t CEcoLab2_GetMemoryAllocator(IEcoSystem1* pISys, IEcoMemoryAllocator1** ppIMem) {
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

int16_t ECOCALLMETHOD CEcoLab2_CreateInstance(IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter, IKD_Tree** ppIKD_Tree) {
    CEcoLab2* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoUnknown* pInnerUnkLab1 = 0;
    IKD_Tree* pInnerLab1 = 0;
    IEcoCalculatorX* pInnerX = 0;
    IEcoCalculatorY* pInnerY = 0;
    IEcoUnknown* pInnerUnkB = 0;
    int16_t result;

    if (ppIKD_Tree == 0 || pIUnkSystem == 0) return ERR_ECO_POINTER;
    *ppIKD_Tree = 0;

    if (pIUnkOuter != 0) return ERR_ECO_NOAGGREGATION;

    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) return ERR_ECO_NOSYSTEM;

    result = CEcoLab2_GetMemoryAllocator(pISys, &pIMem);
    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    pCMe = (CEcoLab2*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab2));
    if (pCMe == 0) {
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    pCMe->m_pVTblIKD_Tree = &g_xCEcoLab2KDTreeVTbl;
    pCMe->m_pVTblIEcoCalculatorX = &g_xCEcoLab2CalcXVTbl;
    pCMe->m_pVTblIEcoCalculatorY = &g_xCEcoLab2CalcYVTbl;
    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_pISys = pISys;
    pCMe->m_pInnerUnkLab1 = 0;
    pCMe->m_pInnerLab1 = 0;
    pCMe->m_pInnerCalculatorX = 0;
    pCMe->m_pInnerCalculatorY = 0;

    /* Aggregate Lab1: pass our IEcoUnknown as pIUnkOuter */
    result = CKD_Tree_B0ABC511_CreateInstanceAggr(pIUnkSystem, (IEcoUnknown*)pCMe, &pInnerUnkLab1);
    if (result != 0 || pInnerUnkLab1 == 0) goto Fail;
    pCMe->m_pInnerUnkLab1 = pInnerUnkLab1;

    /* Get IKD_Tree from the aggregated Lab1's non-delegating QI */
    result = pInnerUnkLab1->pVTbl->QueryInterface(pInnerUnkLab1, &IID_IKD_Tree, (void**)&pInnerLab1);
    if (result != 0 || pInnerLab1 == 0) goto Fail;
    pCMe->m_pInnerLab1 = pInnerLab1;
    /* Balance the QI's AddRef — lifetime is managed by pInnerUnkLab1 alone */
    pInnerUnkLab1->pVTbl->Release(pInnerUnkLab1);

    /* Create contained component B for IEcoCalculatorX (standalone, no aggregation here) */
    result = CEcoCalculatorB_CreateInstance(pIUnkSystem, 0, &pInnerUnkB);
    if (result != 0 || pInnerUnkB == 0) goto Fail;
    result = pInnerUnkB->pVTbl->QueryInterface(pInnerUnkB, &IID_IEcoCalculatorX, (void**)&pInnerX);
    pInnerUnkB->pVTbl->Release(pInnerUnkB);
    if (result != 0 || pInnerX == 0) goto Fail;
    pCMe->m_pInnerCalculatorX = pInnerX;

    /* Create contained component E for IEcoCalculatorY (E aggregates B internally) */
    result = CEcoCalculatorE_CreateInstance(pIUnkSystem, 0, &pInnerY);
    if (result != 0 || pInnerY == 0) goto Fail;
    pCMe->m_pInnerCalculatorY = pInnerY;

    *ppIKD_Tree = (IKD_Tree*)pCMe;
    return ERR_ECO_SUCCESS;

Fail:
    if (pCMe->m_pInnerCalculatorY != 0) pCMe->m_pInnerCalculatorY->pVTbl->Release(pCMe->m_pInnerCalculatorY);
    if (pCMe->m_pInnerCalculatorX != 0) pCMe->m_pInnerCalculatorX->pVTbl->Release(pCMe->m_pInnerCalculatorX);
    if (pCMe->m_pInnerUnkLab1 != 0) pCMe->m_pInnerUnkLab1->pVTbl->Release(pCMe->m_pInnerUnkLab1);
    if (pCMe->m_pISys != 0) pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
    pIMem->pVTbl->Free(pIMem, pCMe);
    pIMem->pVTbl->Release(pIMem);
    return result;
}
