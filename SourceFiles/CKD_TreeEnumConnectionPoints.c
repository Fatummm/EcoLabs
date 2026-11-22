#include "CKD_TreeEnumConnectionPoints.h"

static int16_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_QueryInterface(/* in */ IEcoEnumConnectionPointsPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    if ( IsEqualUGUID(riid, &IID_IEcoEnumConnectionPoints) || IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = me;
        me->pVTbl->AddRef(me);
        return ERR_ECO_SUCCESS;
    }
    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_AddRef(/* in */ IEcoEnumConnectionPointsPtr_t me) {
    CKD_TreeEnumConnectionPoints* pCMe = (CKD_TreeEnumConnectionPoints*)me;

    if (me == 0 ) {
        return -1;
    }
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_Release(/* in */ IEcoEnumConnectionPointsPtr_t me) {
    CKD_TreeEnumConnectionPoints* pCMe = (CKD_TreeEnumConnectionPoints*)me;
    IEcoMemoryAllocator1* pIMem = 0;
    uint32_t i = 0;

    if (me == 0 ) {
        return -1;
    }
    --pCMe->m_cRef;
    if (pCMe->m_cRef != 0) {
        return pCMe->m_cRef;
    }
    pIMem = pCMe->m_pIMem;
    for (i = 0; i < pCMe->m_cCount; i++) {
        pCMe->m_ppCP[i]->pVTbl->Release(pCMe->m_ppCP[i]);
    }
    if (pCMe->m_ppCP != 0) {
        pIMem->pVTbl->Free(pIMem, pCMe->m_ppCP);
    }
    pIMem->pVTbl->Free(pIMem, pCMe);
    pIMem->pVTbl->Release(pIMem);
    return 0;
}

static int16_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_Next(/* in */ IEcoEnumConnectionPointsPtr_t me, /* in */ uint32_t cConnections, /* out */ IEcoConnectionPointPtr_t* ppCP, /* out */ uint32_t* pcFetched) {
    CKD_TreeEnumConnectionPoints* pCMe = (CKD_TreeEnumConnectionPoints*)me;
    uint32_t fetched = 0;

    if (me == 0 || ppCP == 0 || (pcFetched == 0 && cConnections != 1)) {
        return ERR_ECO_POINTER;
    }
    while (fetched < cConnections && pCMe->m_cIndex < pCMe->m_cCount) {
        ppCP[fetched] = pCMe->m_ppCP[pCMe->m_cIndex];
        ppCP[fetched]->pVTbl->AddRef(ppCP[fetched]);
        fetched++;
        pCMe->m_cIndex++;
    }
    if (pcFetched != 0) {
        *pcFetched = fetched;
    }
    return fetched == cConnections ? ERR_ECO_SUCCESS : ERR_ECO_FAIL;
}

static int16_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_Skip(/* in */ IEcoEnumConnectionPointsPtr_t me, /* in */ uint32_t cConnections) {
    CKD_TreeEnumConnectionPoints* pCMe = (CKD_TreeEnumConnectionPoints*)me;

    if (me == 0 ) {
        return ERR_ECO_POINTER;
    }
    if (cConnections > pCMe->m_cCount - pCMe->m_cIndex) {
        pCMe->m_cIndex = pCMe->m_cCount;
        return ERR_ECO_FAIL;
    }
    pCMe->m_cIndex += cConnections;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_Reset(/* in */ IEcoEnumConnectionPointsPtr_t me) {
    CKD_TreeEnumConnectionPoints* pCMe = (CKD_TreeEnumConnectionPoints*)me;

    if (me == 0 ) {
        return ERR_ECO_POINTER;
    }
    pCMe->m_cIndex = 0;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeEnumConnectionPoints_Clone(/* in */ IEcoEnumConnectionPointsPtr_t me, /* out */ IEcoEnumConnectionPointsPtr_t* ppEnum) {
    CKD_TreeEnumConnectionPoints* pCMe = (CKD_TreeEnumConnectionPoints*)me;

    if (me == 0 || ppEnum == 0) {
        return ERR_ECO_POINTER;
    }
    return createCKD_TreeEnumConnectionPoints(pCMe->m_pIMem, pCMe->m_ppCP, pCMe->m_cCount, pCMe->m_cIndex, ppEnum);
}

IEcoEnumConnectionPointsVTbl g_x0000000400000000C000000000000046VTblECP_B0ABC511 = {
    CKD_TreeEnumConnectionPoints_QueryInterface,
    CKD_TreeEnumConnectionPoints_AddRef,
    CKD_TreeEnumConnectionPoints_Release,
    CKD_TreeEnumConnectionPoints_Next,
    CKD_TreeEnumConnectionPoints_Skip,
    CKD_TreeEnumConnectionPoints_Reset,
    CKD_TreeEnumConnectionPoints_Clone
};

int16_t ECOCALLMETHOD createCKD_TreeEnumConnectionPoints(/* in */ IEcoMemoryAllocator1* pIMem, /* in */ IEcoConnectionPoint** ppCP, /* in */ uint32_t count, /* in */ uint32_t index, /* out */ IEcoEnumConnectionPoints** ppEnum) {
    CKD_TreeEnumConnectionPoints* pCMe = 0;
    uint32_t i = 0;

    if (pIMem == 0 || ppEnum == 0 || (ppCP == 0 && count != 0)) {
        return ERR_ECO_POINTER;
    }
    *ppEnum = 0;

    pCMe = (CKD_TreeEnumConnectionPoints*)pIMem->pVTbl->Alloc(pIMem, sizeof(CKD_TreeEnumConnectionPoints));
    if (pCMe == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    pCMe->m_ppCP = 0;
    if (count != 0) {
        pCMe->m_ppCP = (IEcoConnectionPoint**)pIMem->pVTbl->Alloc(pIMem, count * sizeof(IEcoConnectionPoint*));
        if (pCMe->m_ppCP == 0) {
            pIMem->pVTbl->Free(pIMem, pCMe);
            return ERR_ECO_OUTOFMEMORY;
        }
    }
    for (i = 0; i < count; i++) {
        pCMe->m_ppCP[i] = ppCP[i];
        pCMe->m_ppCP[i]->pVTbl->AddRef(pCMe->m_ppCP[i]);
    }

    pCMe->m_pVTblIECP = &g_x0000000400000000C000000000000046VTblECP_B0ABC511;
    pCMe->m_cRef = 1;
    pCMe->m_cCount = count;
    pCMe->m_cIndex = index > count ? count : index;
    pIMem->pVTbl->AddRef(pIMem);
    pCMe->m_pIMem = pIMem;

    *ppEnum = (IEcoEnumConnectionPoints*)pCMe;
    return ERR_ECO_SUCCESS;
}
