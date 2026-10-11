#ifndef __KD_TREE_TYPES_H__
#define __KD_TREE_TYPES_H__

#include "IEcoBase1.h"

/* Coordinate element types supported by the KD-tree */
#define KD_TYPE_INT16       0   /* int16_t   */
#define KD_TYPE_INT         1   /* int       */
#define KD_TYPE_LONGLONG    2   /* int64_t   */
#define KD_TYPE_FLOAT       3   /* float     */
#define KD_TYPE_DOUBLE      4   /* double    */

typedef struct {
    voidptr_t coords;
} KDPoint, *KDPointPtr_t;

typedef struct KDNode {
    KDPoint point;
    struct KDNode* left;
    struct KDNode* right;
} KDNode, *KDNodePtr_t;

typedef struct {
    KDPointPtr_t* points;
    double* distances;
    uint16_t count;
} KDSearchResult, *KDSearchResultPtr_t;

#endif
