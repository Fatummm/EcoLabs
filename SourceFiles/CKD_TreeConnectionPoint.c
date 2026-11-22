#include "CKD_TreeConnectionPoint.h"
#include "CKD_TreeEnumConnections.h"

static int16_t ECOCALLMETHOD CKD_TreeConnectionPoint_QueryInterface(/* in */ IEcoConnectionPointPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;

    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    if ( IsEqualUGUID(riid, &IID_IEcoConnectionPoint) || IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblICP;
        pCMe->m_pICPC->pVTbl->AddRef(pCMe->m_pICPC);
        return ERR_ECO_SUCCESS;
    }
    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD CKD_TreeConnectionPoint_AddRef(/* in */ IEcoConnectionPointPtr_t me) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;

    if (me == 0 ) {
        return -1;
    }
    return pCMe->m_pICPC->pVTbl->AddRef(pCMe->m_pICPC);
}

static uint32_t ECOCALLMETHOD CKD_TreeConnectionPoint_Release(/* in */ IEcoConnectionPointPtr_t me) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;

    if (me == 0 ) {
        return -1;
    }
    return pCMe->m_pICPC->pVTbl->Release(pCMe->m_pICPC);
}

static int16_t ECOCALLMETHOD CKD_TreeConnectionPoint_GetConnectionInterface(/* in */ IEcoConnectionPointPtr_t me, /* out */ UGUID* pIID) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;

    if (me == 0 || pIID == 0) {
        return ERR_ECO_POINTER;
    }
    *pIID = pCMe->m_iid;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeConnectionPoint_GetConnectionPointContainer(/* in */ IEcoConnectionPointPtr_t me, /* out */ IEcoConnectionPointContainerPtr_t* ppCPC) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;

    if (me == 0 || ppCPC == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe->m_pICPC->pVTbl->AddRef(pCMe->m_pICPC);
    *ppCPC = pCMe->m_pICPC;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeConnectionPoint_Advise(/* in */ IEcoConnectionPointPtr_t me, /* in */ IEcoUnknownPtr_t pUnkSink, /* out */ uint32_t* pcCookie) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;
    EcoConnectionData* pCD = 0;
    IEcoUnknown* pISink = 0;
    uint32_t count;
    int16_t result = 0;

    if (me == 0 || pUnkSink == 0 || pcCookie == 0) {
        return ERR_ECO_POINTER;
    }
    *pcCookie = 0;
    if (pCMe->m_cNextCookie == (uint32_t)-1) {
        return ERR_ECO_FAIL;
    }

    result = pUnkSink->pVTbl->QueryInterface(pUnkSink, &pCMe->m_iid, (voidptr_t*)&pISink);
    if (result != 0 || pISink == 0) {
        return ERR_ECO_NOINTERFACE;
    }

    pCD = (EcoConnectionData*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, sizeof(EcoConnectionData));
    if (pCD == 0) {
        pISink->pVTbl->Release(pISink);
        return ERR_ECO_OUTOFMEMORY;
    }
    pCD->pUnk = pISink;
    pCD->cCookie = pCMe->m_cNextCookie + 1;
    count = CKD_TreeConnectionPoint_Count(pCMe);
    pCMe->m_pSinkList->pVTbl->Add(pCMe->m_pSinkList, pCD);
    if (CKD_TreeConnectionPoint_Count(pCMe) == count) {
        pISink->pVTbl->Release(pISink);
        pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCD);
        return ERR_ECO_OUTOFMEMORY;
    }
    pCMe->m_cNextCookie = pCD->cCookie;
    *pcCookie = pCD->cCookie;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeConnectionPoint_Unadvise(/* in */ IEcoConnectionPointPtr_t me, /* in */ uint32_t cCookie) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;
    EcoConnectionData* pCD = 0;
    uint32_t count = 0;
    uint32_t i = 0;

    if (me == 0 ) {
        return ERR_ECO_POINTER;
    }

    count = pCMe->m_pSinkList->pVTbl->Count(pCMe->m_pSinkList);
    for (i = 0; i < count; i++) {
        pCD = (EcoConnectionData*)pCMe->m_pSinkList->pVTbl->Item(pCMe->m_pSinkList, i);
        if (pCD != 0 && pCD->cCookie == cCookie) {
            pCMe->m_pSinkList->pVTbl->RemoveAt(pCMe->m_pSinkList, i);
            if (CKD_TreeConnectionPoint_Count(pCMe) == count) {
                return ERR_ECO_FAIL;
            }
            pCD->pUnk->pVTbl->Release(pCD->pUnk);
            pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCD);
            return ERR_ECO_SUCCESS;
        }
    }
    return ERR_ECO_FAIL;
}

static int16_t ECOCALLMETHOD CKD_TreeConnectionPoint_EnumConnections(/* in */ IEcoConnectionPointPtr_t me, /* out */ IEcoEnumConnectionsPtr_t* ppEnum) {
    CKD_TreeConnectionPoint* pCMe = (CKD_TreeConnectionPoint*)me;

    if (me == 0 || ppEnum == 0) {
        return ERR_ECO_POINTER;
    }
    return createCKD_TreeEnumConnections(pCMe->m_pIMem, pCMe->m_pSinkList, 0, 0, ppEnum);
}

IEcoConnectionPointVTbl g_x0000000300000000C000000000000046VTblCP_B0ABC511 = {
    CKD_TreeConnectionPoint_QueryInterface,
    CKD_TreeConnectionPoint_AddRef,
    CKD_TreeConnectionPoint_Release,
    CKD_TreeConnectionPoint_GetConnectionInterface,
    CKD_TreeConnectionPoint_GetConnectionPointContainer,
    CKD_TreeConnectionPoint_Advise,
    CKD_TreeConnectionPoint_Unadvise,
    CKD_TreeConnectionPoint_EnumConnections
};

uint32_t ECOCALLMETHOD CKD_TreeConnectionPoint_Count(/* in */ CKD_TreeConnectionPoint* pCMe) {
    if (pCMe == 0 || pCMe->m_pSinkList == 0) {
        return 0;
    }
    return pCMe->m_pSinkList->pVTbl->Count(pCMe->m_pSinkList);
}

int16_t ECOCALLMETHOD createCKD_TreeConnectionPoint(/* in */ IEcoMemoryAllocator1* pIMem, /* in */ IEcoInterfaceBus1* pIBus, /* in */ IEcoConnectionPointContainer* pICPC, /* in */ const UGUID* riid, /* out */ CKD_TreeConnectionPoint** ppCP) {
    CKD_TreeConnectionPoint* pCMe = 0;
    IEcoList1* pIList = 0;
    int16_t result = 0;

    if (pIMem == 0 || pIBus == 0 || pICPC == 0 || riid == 0 || ppCP == 0) {
        return ERR_ECO_POINTER;
    }
    *ppCP = 0;

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoList1, 0, &IID_IEcoList1, (voidptr_t*)&pIList);
    if (result != 0 || pIList == 0) {
        return ERR_ECO_COMPONENT_NOTFOUND;
    }

    pCMe = (CKD_TreeConnectionPoint*)pIMem->pVTbl->Alloc(pIMem, sizeof(CKD_TreeConnectionPoint));
    if (pCMe == 0) {
        pIList->pVTbl->Release(pIList);
        return ERR_ECO_OUTOFMEMORY;
    }

    pCMe->m_pVTblICP = &g_x0000000300000000C000000000000046VTblCP_B0ABC511;
    pCMe->m_pICPC = pICPC;
    pCMe->m_iid = *riid;
    pCMe->m_cNextCookie = 0;
    pCMe->m_pSinkList = pIList;
    pIMem->pVTbl->AddRef(pIMem);
    pCMe->m_pIMem = pIMem;

    *ppCP = pCMe;
    return ERR_ECO_SUCCESS;
}

void ECOCALLMETHOD deleteCKD_TreeConnectionPoint(/* in */ CKD_TreeConnectionPoint* pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;
    EcoConnectionData* pCD = 0;
    uint32_t count = 0;
    uint32_t i = 0;

    if (pCMe == 0) {
        return;
    }
    pIMem = pCMe->m_pIMem;
    count = pCMe->m_pSinkList->pVTbl->Count(pCMe->m_pSinkList);
    for (i = 0; i < count; i++) {
        pCD = (EcoConnectionData*)pCMe->m_pSinkList->pVTbl->Item(pCMe->m_pSinkList, i);
        if (pCD != 0) {
            pCD->pUnk->pVTbl->Release(pCD->pUnk);
            pIMem->pVTbl->Free(pIMem, pCD);
        }
    }
    pCMe->m_pSinkList->pVTbl->Clear(pCMe->m_pSinkList);
    pCMe->m_pSinkList->pVTbl->Release(pCMe->m_pSinkList);
    pIMem->pVTbl->Free(pIMem, pCMe);
    pIMem->pVTbl->Release(pIMem);
}
