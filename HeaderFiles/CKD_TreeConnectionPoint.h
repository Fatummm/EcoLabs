#ifndef __C_ECOLAB1_CONNECTION_POINT_H__
#define __C_ECOLAB1_CONNECTION_POINT_H__

#include "IEcoConnectionPoint.h"
#include "IEcoConnectionPointContainer.h"
#include "IEcoEnumConnections.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoList1.h"
#include "IdEcoList1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CKD_TreeConnectionPoint* CKD_TreeConnectionPointPtr_t;

typedef struct CKD_TreeConnectionPoint {

    IEcoConnectionPointVTbl* m_pVTblICP;

    IEcoConnectionPointContainer* m_pICPC;
    UGUID m_iid;
    uint32_t m_cNextCookie;
    IEcoList1* m_pSinkList;
    IEcoMemoryAllocator1* m_pIMem;

} CKD_TreeConnectionPoint;

int16_t ECOCALLMETHOD createCKD_TreeConnectionPoint(/* in */ IEcoMemoryAllocator1* pIMem, /* in */ IEcoInterfaceBus1* pIBus, /* in */ IEcoConnectionPointContainer* pICPC, /* in */ const UGUID* riid, /* out */ CKD_TreeConnectionPoint** ppCP);
void ECOCALLMETHOD deleteCKD_TreeConnectionPoint(/* in */ CKD_TreeConnectionPoint* pCMe);
uint32_t ECOCALLMETHOD CKD_TreeConnectionPoint_Count(/* in */ CKD_TreeConnectionPoint* pCMe);

#endif
