/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdKD_Tree
 * </summary>
 *
 * <description>
 *   This header describes the interface IKD_Tree
 * </description>
 *
 * <reference>
 *
 * </reference>
 *
 * <author>
 *   Copyright (c) 2018 Dmitriy Lepa. All rights reserved.
 * </author>
 *
 */

#ifndef __I_KD_TREE_H__
#define __I_KD_TREE_H__

#include "IEcoBase1.h"
#include "KDTreeTypes.h"

/* IKD_Tree IID = {A1711225-CF70-45B6-985E-CA20A33CA463} */
#ifndef __IID_IKD_Tree
static const UGUID IID_IKD_Tree = {0x01, 0x10, {0xA1, 0x71, 0x12, 0x25, 0xCF, 0x70, 0x45, 0xB6, 0x98, 0x5E, 0xCA, 0x20, 0xA3, 0x3C, 0xA4, 0x63}};
#endif /* __IID_IKD_Tree */

typedef struct IKD_Tree* IKD_TreePtr_t;

typedef struct IKD_TreeVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IKD_TreePtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IKD_TreePtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IKD_TreePtr_t me);

    /* IKD_Tree */
    int16_t (ECOCALLMETHOD *BuildTree)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t* points, 
        /* in  */ int32_t count,
        /* in  */ uint16_t dimension
    );

    int16_t (ECOCALLMETHOD *SearchKNearest)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t point, 
        /* in  */ int16_t k,
        /* out */ KDSearchResult* result
    );

    int16_t (ECOCALLMETHOD *DestroyTree)(
        /* in  */ IKD_TreePtr_t me
    );

    int16_t (ECOCALLMETHOD *InsertPoint)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t point
    );

    int16_t (ECOCALLMETHOD *DeletePoint)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t point
    );

    int16_t (ECOCALLMETHOD *BuildTreeInt)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t* points, 
        /* in  */ int32_t count,
        /* in  */ uint16_t dimension
    );

    int16_t (ECOCALLMETHOD *BuildTreeLongLong)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t* points, 
        /* in  */ int32_t count,
        /* in  */ uint16_t dimension
    );

    int16_t (ECOCALLMETHOD *BuildTreeFloat)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t* points, 
        /* in  */ int32_t count,
        /* in  */ uint16_t dimension
    );

    int16_t (ECOCALLMETHOD *BuildTreeDouble)(
        /* in  */ IKD_TreePtr_t me, 
        /* in  */ KDPointPtr_t* points, 
        /* in  */ int32_t count,
        /* in  */ uint16_t dimension
    );

} IKD_TreeVTbl, *IKD_TreeVTblPtr_t;

interface IKD_Tree {
    struct IKD_TreeVTbl *pVTbl;
} IKD_Tree;


#endif /* __I_KD_TREE_H__ */

