/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CKD_TreeFactory
 * </summary>
 *
 * <description>
 *   This source code describes the component factory for CKD_Tree
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 Dmitriy Lepa. All rights reserved.
 * </author>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IdKD_Tree.h"
#include "CKD_Tree.h"

typedef struct CKD_TreeFactory {
    IEcoComponentFactoryVTbl* m_pVTbl;
    uint32_t m_cRef;
} CKD_TreeFactory;

static int16_t ECOCALLMETHOD CKD_TreeFactory_QueryInterface(/* in */ IEcoComponentFactoryPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    if (IsEqualUGUID(riid, &IID_IEcoUnknown) || IsEqualUGUID(riid, &IID_IEcoComponentFactory)) {
        *ppv = me;
        me->pVTbl->AddRef(me);
        return ERR_ECO_SUCCESS;
    }

    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD CKD_TreeFactory_AddRef(/* in */ IEcoComponentFactoryPtr_t me) {
    CKD_TreeFactory* pCMe = (CKD_TreeFactory*)me;

    if (me == 0) {
        return -1;
    }

    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CKD_TreeFactory_Release(/* in */ IEcoComponentFactoryPtr_t me) {
    CKD_TreeFactory* pCMe = (CKD_TreeFactory*)me;

    if (me == 0) {
        return -1;
    }

    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        return 0;
    }
    return pCMe->m_cRef;
}

static int16_t ECOCALLMETHOD CKD_TreeFactory_Alloc(/* in */ IEcoComponentFactoryPtr_t me, /* in */ IEcoUnknownPtr_t pISystem, /* in */ IEcoUnknownPtr_t pIUnknownOuter, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    IKD_Tree* pIKDTree = 0;
    int16_t result;

    if (ppv == 0 || pISystem == 0) {
        return ERR_ECO_POINTER;
    }
    *ppv = 0;

    result = CKD_Tree_B0ABC511_CreateInstance(pISystem, pIUnknownOuter, &pIKDTree);
    if (result != 0 || pIKDTree == 0) {
        return result;
    }

    result = pIKDTree->pVTbl->QueryInterface(pIKDTree, riid, ppv);
    pIKDTree->pVTbl->Release(pIKDTree);

    return result;
}

static int16_t ECOCALLMETHOD CKD_TreeFactory_Init(/* in */ IEcoComponentFactoryPtr_t me, /* in */ IEcoUnknownPtr_t pISystem, /* in */ voidptr_t pv) {
    return ERR_ECO_SUCCESS;
}

static char_t* ECOCALLMETHOD CKD_TreeFactory_get_Name(/* in */ IEcoComponentFactoryPtr_t me) {
    return "CKD_Tree";
}

static char_t* ECOCALLMETHOD CKD_TreeFactory_get_Version(/* in */ IEcoComponentFactoryPtr_t me) {
    return "1.0.0";
}

static char_t* ECOCALLMETHOD CKD_TreeFactory_get_Manufacturer(/* in */ IEcoComponentFactoryPtr_t me) {
    return "Dmitriy Lepa";
}

static IEcoComponentFactoryVTbl g_xCKD_TreeFactoryVTbl = {
    CKD_TreeFactory_QueryInterface,
    CKD_TreeFactory_AddRef,
    CKD_TreeFactory_Release,
    CKD_TreeFactory_Alloc,
    CKD_TreeFactory_Init,
    CKD_TreeFactory_get_Name,
    CKD_TreeFactory_get_Version,
    CKD_TreeFactory_get_Manufacturer
};

static CKD_TreeFactory g_xCKD_TreeFactory = {
    &g_xCKD_TreeFactoryVTbl,
    1
};

IEcoComponentFactory* GetIEcoComponentFactoryPtr_B0ABC511000000000000000000000000 = (IEcoComponentFactory*)&g_xCKD_TreeFactory;
