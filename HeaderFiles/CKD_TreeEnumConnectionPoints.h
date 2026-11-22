#ifndef __C_ECOLAB1_ENUM_CONNECTION_POINTS_H__
#define __C_ECOLAB1_ENUM_CONNECTION_POINTS_H__

#include "IEcoEnumConnectionPoints.h"
#include "IdEcoMemoryManager1.h"

typedef struct CKD_TreeEnumConnectionPoints* CKD_TreeEnumConnectionPointsPtr_t;

typedef struct CKD_TreeEnumConnectionPoints {

    IEcoEnumConnectionPointsVTbl* m_pVTblIECP;

    uint32_t m_cRef;
    IEcoConnectionPoint** m_ppCP;
    uint32_t m_cCount;
    uint32_t m_cIndex;
    IEcoMemoryAllocator1* m_pIMem;

} CKD_TreeEnumConnectionPoints;

int16_t ECOCALLMETHOD createCKD_TreeEnumConnectionPoints(/* in */ IEcoMemoryAllocator1* pIMem, /* in */ IEcoConnectionPoint** ppCP, /* in */ uint32_t count, /* in */ uint32_t index, /* out */ IEcoEnumConnectionPoints** ppEnum);

#endif
