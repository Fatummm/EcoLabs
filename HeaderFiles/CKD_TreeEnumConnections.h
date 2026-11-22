#ifndef __C_ECOLAB1_ENUM_CONNECTIONS_H__
#define __C_ECOLAB1_ENUM_CONNECTIONS_H__

#include "IEcoEnumConnections.h"
#include "IEcoList1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CKD_TreeEnumConnections* CKD_TreeEnumConnectionsPtr_t;

typedef struct CKD_TreeEnumConnections {

    IEcoEnumConnectionsVTbl* m_pVTblIEC;

    uint32_t m_cRef;
    EcoConnectionData* m_pData;
    uint32_t m_cCount;
    uint32_t m_cIndex;
    IEcoMemoryAllocator1* m_pIMem;

} CKD_TreeEnumConnections;

int16_t ECOCALLMETHOD createCKD_TreeEnumConnections(/* in */ IEcoMemoryAllocator1* pIMem, /* in */ IEcoList1* pIList, /* in */ const EcoConnectionData* pData, /* in */ uint32_t count, /* out */ IEcoEnumConnections** ppEnum);

#endif
