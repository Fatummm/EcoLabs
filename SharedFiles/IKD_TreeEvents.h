#ifndef __I_KD_TREE_EVENTS_H__
#define __I_KD_TREE_EVENTS_H__

#include "IKD_Tree.h"

/* IKD_TreeEvents IID = {64B52D4D-6614-4F92-AA85-FDE2920FF722} */
static const UGUID IID_IKD_TreeEvents = {0x01, 0x10,
    {0x64, 0xB5, 0x2D, 0x4D, 0x66, 0x14, 0x4F, 0x92, 0xAA, 0x85, 0xFD, 0xE2, 0x92, 0x0F, 0xF7, 0x22}};

typedef struct IKD_TreeEvents* IKD_TreeEventsPtr_t;

/* source is borrowed for the duration of the synchronous callback. */
typedef struct IKD_TreeEventsVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IKD_TreeEventsPtr_t me, const UGUID* riid, voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(IKD_TreeEventsPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IKD_TreeEventsPtr_t me);

    /* A node was created at the median of the half-open range [left, right). */
    int16_t (ECOCALLMETHOD *OnBuildNode)(IKD_TreeEventsPtr_t me, IKD_Tree* source,
        uint16_t axis, int32_t left, int32_t median, int32_t right);
    /* The search processed a node and updated its candidate set. */
    int16_t (ECOCALLMETHOD *OnSearchVisit)(IKD_TreeEventsPtr_t me, IKD_Tree* source,
        uint16_t axis, double squaredDistance, uint16_t candidateCount);
    /* The search skipped a nonempty subtree beyond the splitting plane. */
    int16_t (ECOCALLMETHOD *OnSearchPrune)(IKD_TreeEventsPtr_t me, IKD_Tree* source,
        uint16_t axis, double squaredPlaneDistance, double squaredWorstDistance);
} IKD_TreeEventsVTbl;

interface IKD_TreeEvents {
    IKD_TreeEventsVTbl* pVTbl;
} IKD_TreeEvents;

#endif
