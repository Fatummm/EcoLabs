/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CKD_Tree_B0ABC511
 * </summary>
 *
 * <description>
 *   This source code describes the implementation of the interfaces for CKD_Tree_B0ABC511
 * </description>
 *
 * <author>
 *   Copyright (c) 2018 Dmitriy Lepa. All rights reserved.
 * </author>
 *
 */


#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CKD_Tree.h"
#include "CEcoCalculatorA.h"
#include "CEcoCalculatorD.h"

#include <stddef.h>

/*
 *
 * <summary>
 *   CKD_Tree_ElemSize Function
 * </summary>
 *
 * <description>
 *   Returns the size in bytes of one coordinate of the given type
 * </description>
 *
 */
static uint32_t CKD_Tree_ElemSize(
    int16_t type)
{
    switch (type) {
        case KD_TYPE_INT:
            return sizeof(int);
        case KD_TYPE_LONGLONG:
            return sizeof(int64_t);
        case KD_TYPE_FLOAT:
            return sizeof(float);
        case KD_TYPE_DOUBLE:
            return sizeof(double);
        default:
            return sizeof(int16_t);
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_AxisLess Function
 * </summary>
 *
 * <description>
 *   Checks if coords1[axis] < coords2[axis] for the given coordinate type
 * </description>
 *
 */
static int16_t CKD_Tree_AxisLess(
    voidptr_t coords1,
    voidptr_t coords2,
    int16_t axis,
    int16_t type)
{
    switch (type) {
        case KD_TYPE_INT:
            return ((int*)coords1)[axis] < ((int*)coords2)[axis];
        case KD_TYPE_LONGLONG:
            return ((int64_t*)coords1)[axis] < ((int64_t*)coords2)[axis];
        case KD_TYPE_FLOAT:
            return ((float*)coords1)[axis] < ((float*)coords2)[axis];
        case KD_TYPE_DOUBLE:
            return ((double*)coords1)[axis] < ((double*)coords2)[axis];
        default:
            return ((int16_t*)coords1)[axis] < ((int16_t*)coords2)[axis];
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_AxisEqual Function
 * </summary>
 *
 * <description>
 *   Checks if coords1[axis] == coords2[axis] for the given coordinate type
 * </description>
 *
 */
static int16_t CKD_Tree_AxisEqual(
    voidptr_t coords1,
    voidptr_t coords2,
    int16_t axis,
    int16_t type)
{
    switch (type) {
        case KD_TYPE_INT:
            return ((int*)coords1)[axis] == ((int*)coords2)[axis];
        case KD_TYPE_LONGLONG:
            return ((int64_t*)coords1)[axis] == ((int64_t*)coords2)[axis];
        case KD_TYPE_FLOAT:
            return ((float*)coords1)[axis] == ((float*)coords2)[axis];
        case KD_TYPE_DOUBLE:
            return ((double*)coords1)[axis] == ((double*)coords2)[axis];
        default:
            return ((int16_t*)coords1)[axis] == ((int16_t*)coords2)[axis];
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_AxisDiff Function
 * </summary>
 *
 * <description>
 *   Computes coords1[axis] - coords2[axis] as double.
 *   Integer differences are taken in a wider type to avoid overflow.
 * </description>
 *
 */
static double CKD_Tree_AxisDiff(
    voidptr_t coords1,
    voidptr_t coords2,
    int16_t axis,
    int16_t type)
{
    int64_t a;
    int64_t b;

    switch (type) {
        case KD_TYPE_INT:
            return (double)((int64_t)((int*)coords1)[axis] - (int64_t)((int*)coords2)[axis]);
        case KD_TYPE_LONGLONG:
            a = ((int64_t*)coords1)[axis];
            b = ((int64_t*)coords2)[axis];
            /* Unsigned subtraction: the difference may not fit into int64_t */
            if (a >= b) {
                return (double)((uint64_t)a - (uint64_t)b);
            }
            return -(double)((uint64_t)b - (uint64_t)a);
        case KD_TYPE_FLOAT:
            return (double)((float*)coords1)[axis] - (double)((float*)coords2)[axis];
        case KD_TYPE_DOUBLE:
            return ((double*)coords1)[axis] - ((double*)coords2)[axis];
        default:
            return (double)((int)((int16_t*)coords1)[axis] - (int)((int16_t*)coords2)[axis]);
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_SquaredDistance Function
 * </summary>
 *
 * <description>
 *   Computes squared Euclidean distance between two coordinate arrays.
 *   Squared distance preserves ordering and avoids sqrt dependency.
 * </description>
 *
 */
static double CKD_Tree_SquaredDistance(
    voidptr_t coords1,
    voidptr_t coords2,
    uint16_t dimension,
    int16_t type)
{
    double dist = 0.0;
    uint16_t i;
    double diff;

    for (i = 0; i < dimension; i++) {
        diff = CKD_Tree_AxisDiff(coords1, coords2, (int16_t)i, type);
        dist += diff * diff;
    }

    return dist;
}

/*
 *
 * <summary>
 *   CKD_Tree_CoordsEqual Function
 * </summary>
 *
 * <description>
 *   Checks if two coordinate arrays are equal
 * </description>
 *
 */
static int16_t CKD_Tree_CoordsEqual(
    voidptr_t coords1,
    voidptr_t coords2,
    uint16_t dimension,
    int16_t type)
{
    uint16_t i;

    for (i = 0; i < dimension; i++) {
        if (!CKD_Tree_AxisEqual(coords1, coords2, (int16_t)i, type)) {
            return 0;
        }
    }
    return 1;
}

/*
 *
 * <summary>
 *   CKD_Tree_QuickSelect Function
 * </summary>
 *
 * <description>
 *   Finds k-th smallest element by specified axis in O(n) average time.
 *   Partitions array in place. Range: [left, right)
 * </description>
 *
 */
static int32_t CKD_Tree_QuickSelect(
    KDPointPtr_t* points,
    int32_t left,
    int32_t right,
    int32_t k,
    int16_t axis,
    int16_t type)
{
    int32_t pivot_idx;
    KDPointPtr_t pivot;
    KDPointPtr_t temp;
    int32_t store_idx;
    int32_t i;

    if (left == right - 1) {
        return left;
    }

    /* Choose pivot at middle */
    pivot_idx = left + (right - left) / 2;
    pivot = points[pivot_idx];

    /* Move pivot to end */
    temp = points[pivot_idx];
    points[pivot_idx] = points[right - 1];
    points[right - 1] = temp;

    /* Partition */
    store_idx = left;
    for (i = left; i < right - 1; i++) {
        if (CKD_Tree_AxisLess(points[i]->coords, pivot->coords, axis, type)) {
            temp = points[i];
            points[i] = points[store_idx];
            points[store_idx] = temp;
            store_idx++;
        }
    }

    /* Move pivot to final position */
    temp = points[right - 1];
    points[right - 1] = points[store_idx];
    points[store_idx] = temp;
    pivot_idx = store_idx;

    /* Recurse only to relevant side */
    if (k == pivot_idx) {
        return pivot_idx;
    } else if (k < pivot_idx) {
        return CKD_Tree_QuickSelect(points, left, pivot_idx, k, axis, type);
    } else {
        return CKD_Tree_QuickSelect(points, pivot_idx + 1, right, k, axis, type);
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_FindMedianIndex Function
 * </summary>
 *
 * <description>
 *   Finds the index of median element in range [left, right) by specified axis.
 *   Uses QuickSelect for O(n) average time complexity.
 * </description>
 *
 */
static int32_t CKD_Tree_FindMedianIndex(
    KDPointPtr_t* points,
    int32_t left,
    int32_t right,
    int16_t axis,
    int16_t type)
{
    int32_t count;
    int32_t median_idx;

    if (left >= right) {
        return left;
    }

    count = right - left;
    median_idx = left + count / 2;

    return CKD_Tree_QuickSelect(points, left, right, median_idx, axis, type);
}

/*
 *
 * <summary>
 *   CKD_Tree_CreateNode Function
 * </summary>
 *
 * <description>
 *   Allocates a leaf node holding a copy of the given coordinates.
 *   Returns 0 if memory allocation fails.
 * </description>
 *
 */
static KDNodePtr_t CKD_Tree_CreateNode(
    voidptr_t coords,
    uint16_t dimension,
    int16_t type,
    IEcoMemoryAllocator1* pIMem)
{
    KDNodePtr_t node;
    uint32_t size = dimension * CKD_Tree_ElemSize(type);

    node = (KDNodePtr_t)pIMem->pVTbl->Alloc(pIMem, sizeof(KDNode));
    if (node == 0) {
        return 0;
    }

    /* Allocate and copy coordinates */
    node->point.coords = pIMem->pVTbl->Alloc(pIMem, size);
    if (node->point.coords == 0) {
        pIMem->pVTbl->Free(pIMem, node);
        return 0;
    }
    pIMem->pVTbl->Copy(pIMem, node->point.coords, coords, size);
    node->left = 0;
    node->right = 0;

    return node;
}

/*
 *
 * <summary>
 *   CKD_Tree_BuildTreeRecursive Function
 * </summary>
 *
 * <description>
 *   Recursively builds KD-tree from points in range [left, right).
 *   Returns pointer to root node of the built subtree.
 *   Sets *pFailed to 1 if a node could not be allocated; the caller
 *   must then free the partially built tree.
 * </description>
 *
 */
static KDNodePtr_t CKD_Tree_BuildTreeRecursive(
    KDPointPtr_t* points,
    int32_t left,
    int32_t right,
    int16_t axis,
    uint16_t dimension,
    int16_t type,
    IEcoMemoryAllocator1* pIMem,
    int16_t* pFailed)
{
    KDNodePtr_t node;
    int32_t median_idx;
    int16_t next_axis;

    /* Base case: empty range */
    if (left >= right) {
        return 0;
    }

    /* Find median by current axis */
    median_idx = CKD_Tree_FindMedianIndex(points, left, right, axis, type);

    /* Allocate node */
    node = CKD_Tree_CreateNode(points[median_idx]->coords, dimension, type, pIMem);
    if (node == 0) {
        *pFailed = 1;
        return 0;
    }

    /* Next splitting axis */
    next_axis = (axis + 1) % (int16_t)dimension;

    /* Recursively build subtrees */
    node->left = CKD_Tree_BuildTreeRecursive(points, left, median_idx, next_axis, dimension, type, pIMem, pFailed);
    if (*pFailed == 0) {
        node->right = CKD_Tree_BuildTreeRecursive(points, median_idx + 1, right, next_axis, dimension, type, pIMem, pFailed);
    }

    return node;
}

/*
 *
 * <summary>
 *   CKD_Tree_FreeNodeRecursive Function
 * </summary>
 *
 * <description>
 *   Recursively frees all nodes of the tree (post-order traversal)
 * </description>
 *
 */
static void CKD_Tree_FreeNodeRecursive(
    KDNodePtr_t node,
    IEcoMemoryAllocator1* pIMem)
{
    if (node == 0) {
        return;
    }

    CKD_Tree_FreeNodeRecursive(node->left, pIMem);
    CKD_Tree_FreeNodeRecursive(node->right, pIMem);

    if (node->point.coords != 0) {
        pIMem->pVTbl->Free(pIMem, node->point.coords);
    }
    pIMem->pVTbl->Free(pIMem, node);
}

/*
 *
 * <summary>
 *   CKD_Tree_SearchRecursive Function
 * </summary>
 *
 * <description>
 *   Recursively searches for k nearest neighbors with backtracking.
 *   Distances stored in result are squared Euclidean distances.
 * </description>
 *
 */
static void CKD_Tree_SearchRecursive(
    KDNodePtr_t node,
    voidptr_t queryCoords,
    int16_t k,
    uint16_t dimension,
    int16_t axis,
    int16_t type,
    KDSearchResult* result)
{
    double dist;
    double axis_dist;
    double worst_dist;
    int16_t next_axis;
    int16_t worst_idx;
    int16_t i;
    KDNodePtr_t first;
    KDNodePtr_t second;

    if (node == 0) {
        return;
    }

    /* Compute squared distance from query to this node */
    dist = CKD_Tree_SquaredDistance(queryCoords, node->point.coords, dimension, type);

    /* Try to add this node to results */
    if (result->count < (uint16_t)k) {
        /* Still have room */
        result->points[result->count] = &node->point;
        result->distances[result->count] = dist;
        result->count++;
    } else {
        /* Find the worst (farthest) in current results */
        worst_idx = 0;
        worst_dist = result->distances[0];
        for (i = 1; i < k; i++) {
            if (result->distances[i] > worst_dist) {
                worst_dist = result->distances[i];
                worst_idx = i;
            }
        }
        /* Replace if this node is closer */
        if (dist < worst_dist) {
            result->points[worst_idx] = &node->point;
            result->distances[worst_idx] = dist;
        }
    }

    /* Decide which subtree to visit first */
    next_axis = (axis + 1) % (int16_t)dimension;
    if (CKD_Tree_AxisLess(queryCoords, node->point.coords, axis, type)) {
        first = node->left;
        second = node->right;
    } else {
        first = node->right;
        second = node->left;
    }

    /* Visit closer subtree first */
    CKD_Tree_SearchRecursive(first, queryCoords, k, dimension, next_axis, type, result);

    /* Check if we need to visit the other subtree (pruning) */
    axis_dist = CKD_Tree_AxisDiff(queryCoords, node->point.coords, axis, type);
    axis_dist = axis_dist * axis_dist;

    if (result->count < (uint16_t)k) {
        /* Haven't found k neighbors yet, must check other side */
        CKD_Tree_SearchRecursive(second, queryCoords, k, dimension, next_axis, type, result);
    } else {
        /* Find current worst distance */
        worst_dist = result->distances[0];
        for (i = 1; i < k; i++) {
            if (result->distances[i] > worst_dist) {
                worst_dist = result->distances[i];
            }
        }
        /* Only visit other subtree if splitting plane is closer than worst */
        if (axis_dist < worst_dist) {
            CKD_Tree_SearchRecursive(second, queryCoords, k, dimension, next_axis, type, result);
        }
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_InsertRecursive Function
 * </summary>
 *
 * <description>
 *   Recursively inserts a point into the KD-tree.
 *   Returns the root of the modified subtree.
 *   Sets *pInserted to 1 if the new node was allocated and linked.
 * </description>
 *
 */
static KDNodePtr_t CKD_Tree_InsertRecursive(
    KDNodePtr_t node,
    voidptr_t coords,
    int16_t axis,
    uint16_t dimension,
    int16_t type,
    IEcoMemoryAllocator1* pIMem,
    int16_t* pInserted)
{
    int16_t next_axis;

    if (node == 0) {
        /* Create new leaf node */
        node = CKD_Tree_CreateNode(coords, dimension, type, pIMem);
        if (node != 0) {
            *pInserted = 1;
        }
        return node;
    }

    next_axis = (axis + 1) % (int16_t)dimension;

    if (CKD_Tree_AxisLess(coords, node->point.coords, axis, type)) {
        node->left = CKD_Tree_InsertRecursive(node->left, coords, next_axis, dimension, type, pIMem, pInserted);
    } else {
        node->right = CKD_Tree_InsertRecursive(node->right, coords, next_axis, dimension, type, pIMem, pInserted);
    }

    return node;
}

/*
 *
 * <summary>
 *   CKD_Tree_FindMinNode Function
 * </summary>
 *
 * <description>
 *   Finds the node with minimum coordinate along target axis in subtree.
 *   Required for the delete operation.
 * </description>
 *
 */
static KDNodePtr_t CKD_Tree_FindMinNode(
    KDNodePtr_t node,
    int16_t targetAxis,
    int16_t currentAxis,
    uint16_t dimension,
    int16_t type)
{
    KDNodePtr_t leftMin;
    KDNodePtr_t rightMin;
    KDNodePtr_t minNode;
    int16_t next_axis;

    if (node == 0) {
        return 0;
    }

    next_axis = (currentAxis + 1) % (int16_t)dimension;

    /* If current splitting axis matches target axis,
       minimum can only be in left subtree or current node */
    if (currentAxis == targetAxis) {
        if (node->left == 0) {
            return node;
        }
        leftMin = CKD_Tree_FindMinNode(node->left, targetAxis, next_axis, dimension, type);
        if (leftMin != 0 && CKD_Tree_AxisLess(leftMin->point.coords, node->point.coords, targetAxis, type)) {
            return leftMin;
        }
        return node;
    }

    /* Otherwise minimum could be in either subtree */
    leftMin = CKD_Tree_FindMinNode(node->left, targetAxis, next_axis, dimension, type);
    rightMin = CKD_Tree_FindMinNode(node->right, targetAxis, next_axis, dimension, type);

    minNode = node;
    if (leftMin != 0 && CKD_Tree_AxisLess(leftMin->point.coords, minNode->point.coords, targetAxis, type)) {
        minNode = leftMin;
    }
    if (rightMin != 0 && CKD_Tree_AxisLess(rightMin->point.coords, minNode->point.coords, targetAxis, type)) {
        minNode = rightMin;
    }

    return minNode;
}

/*
 *
 * <summary>
 *   CKD_Tree_DeleteRecursive Function
 * </summary>
 *
 * <description>
 *   Recursively deletes a point from the KD-tree.
 *   Returns the new root of the modified subtree.
 *   Sets *pDeleted to 1 if a matching point was found and removed.
 * </description>
 *
 */
static KDNodePtr_t CKD_Tree_DeleteRecursive(
    KDNodePtr_t node,
    voidptr_t coords,
    int16_t axis,
    uint16_t dimension,
    int16_t type,
    IEcoMemoryAllocator1* pIMem,
    int16_t* pDeleted)
{
    KDNodePtr_t minNode;
    int16_t next_axis;
    int16_t axisEqual;
    int16_t minDeleted = 0;
    uint32_t size = dimension * CKD_Tree_ElemSize(type);

    if (node == 0) {
        return 0;
    }

    next_axis = (axis + 1) % (int16_t)dimension;

    /* Check if this is the node to delete */
    if (CKD_Tree_CoordsEqual(node->point.coords, coords, dimension, type)) {
        *pDeleted = 1;
        /* Has right subtree: replace with min along splitting axis from right */
        if (node->right != 0) {
            minNode = CKD_Tree_FindMinNode(node->right, axis, next_axis, dimension, type);
            pIMem->pVTbl->Copy(pIMem, node->point.coords, minNode->point.coords, size);
            node->right = CKD_Tree_DeleteRecursive(node->right, minNode->point.coords, next_axis, dimension, type, pIMem, &minDeleted);
        }
        /* Has only left subtree: move left to right, then replace with min */
        else if (node->left != 0) {
            minNode = CKD_Tree_FindMinNode(node->left, axis, next_axis, dimension, type);
            pIMem->pVTbl->Copy(pIMem, node->point.coords, minNode->point.coords, size);
            node->right = node->left;
            node->left = 0;
            node->right = CKD_Tree_DeleteRecursive(node->right, minNode->point.coords, next_axis, dimension, type, pIMem, &minDeleted);
        }
        /* Leaf node: free and return null */
        else {
            pIMem->pVTbl->Free(pIMem, node->point.coords);
            pIMem->pVTbl->Free(pIMem, node);
            return 0;
        }
        return node;
    }

    /* Recurse to appropriate subtree */
    if (CKD_Tree_AxisLess(coords, node->point.coords, axis, type)) {
        node->left = CKD_Tree_DeleteRecursive(node->left, coords, next_axis, dimension, type, pIMem, pDeleted);
    } else {
        axisEqual = CKD_Tree_AxisEqual(coords, node->point.coords, axis, type);
        node->right = CKD_Tree_DeleteRecursive(node->right, coords, next_axis, dimension, type, pIMem, pDeleted);
        /* BuildTree may leave points equal along the splitting axis in the left subtree */
        if (*pDeleted == 0 && axisEqual) {
            node->left = CKD_Tree_DeleteRecursive(node->left, coords, next_axis, dimension, type, pIMem, pDeleted);
        }
    }

    return node;
}

/*
 *
 * <summary>
 *   QueryInterface Function
 * </summary>
 *
 * <description>
 *   QueryInterface function for the IKD_Tree interface
 * </description>
 *
 */
/* --- Non-delegating IEcoUnknown (for aggregation support) --- */

static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_NondelegatingQI(/* in */ IEcoUnknownPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblINondelegatingUnk));

    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    if ( IsEqualUGUID(riid, &IID_IKD_Tree) ) {
        *ppv = &pCMe->m_pVTblIKD_Tree;
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblINondelegatingUnk;
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoCalculatorX) ) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoCalculatorY) ) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    ++pCMe->m_cRef;
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CKD_Tree_B0ABC511_NondelegatingAddRef(/* in */ IEcoUnknownPtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblINondelegatingUnk));
    if (me == 0) return -1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CKD_Tree_B0ABC511_NondelegatingRelease(/* in */ IEcoUnknownPtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblINondelegatingUnk));
    if (me == 0) return -1;
    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        pCMe->Delete(pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

static IEcoUnknownVTbl g_xCKD_Tree_NondelegatingUnkVTbl = {
    CKD_Tree_B0ABC511_NondelegatingQI,
    CKD_Tree_B0ABC511_NondelegatingAddRef,
    CKD_Tree_B0ABC511_NondelegatingRelease
};

/* --- Delegating IKD_Tree QI/AddRef/Release (delegates to pIUnkOuter when aggregated) --- */

static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_QueryInterface(/* in */ IKD_TreePtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;

    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

/*
 *
 * <summary>
 *   AddRef Function
 * </summary>
 *
 * <description>
 *   AddRef function for the IKD_Tree interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CKD_Tree_B0ABC511_AddRef(/* in */ IKD_TreePtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;
    if (me == 0) return -1;
    return pCMe->m_pIUnkOuter->pVTbl->AddRef(pCMe->m_pIUnkOuter);
}

/*
 *
 * <summary>
 *   Release Function
 * </summary>
 *
 * <description>
 *   Release function for the IKD_Tree interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CKD_Tree_B0ABC511_Release(/* in */ IKD_TreePtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;
    if (me == 0) return -1;
    return pCMe->m_pIUnkOuter->pVTbl->Release(pCMe->m_pIUnkOuter);
}

/* --- Delegating IEcoCalculatorX (containment of component A) --- */

static int16_t ECOCALLMETHOD CKD_Tree_ICalcX_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, void** ppv) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorX));
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CKD_Tree_ICalcX_AddRef(IEcoCalculatorXPtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorX));
    return pCMe->m_pIUnkOuter->pVTbl->AddRef(pCMe->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CKD_Tree_ICalcX_Release(IEcoCalculatorXPtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorX));
    return pCMe->m_pIUnkOuter->pVTbl->Release(pCMe->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CKD_Tree_ICalcX_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorX->pVTbl->Addition(pCMe->m_pInnerCalculatorX, a, b);
}

static int32_t ECOCALLMETHOD CKD_Tree_ICalcX_Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorX->pVTbl->Subtraction(pCMe->m_pInnerCalculatorX, a, b);
}

static IEcoCalculatorXVTbl g_xCKD_Tree_ICalcXVTbl = {
    CKD_Tree_ICalcX_QueryInterface,
    CKD_Tree_ICalcX_AddRef,
    CKD_Tree_ICalcX_Release,
    CKD_Tree_ICalcX_Addition,
    CKD_Tree_ICalcX_Subtraction
};

/* --- Delegating IEcoCalculatorY (containment of component D) --- */

static int16_t ECOCALLMETHOD CKD_Tree_ICalcY_QueryInterface(IEcoCalculatorYPtr_t me, const UGUID* riid, void** ppv) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorY));
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CKD_Tree_ICalcY_AddRef(IEcoCalculatorYPtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorY));
    return pCMe->m_pIUnkOuter->pVTbl->AddRef(pCMe->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CKD_Tree_ICalcY_Release(IEcoCalculatorYPtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorY));
    return pCMe->m_pIUnkOuter->pVTbl->Release(pCMe->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CKD_Tree_ICalcY_Multiplication(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorY));
    return pCMe->m_pInnerCalculatorY->pVTbl->Multiplication(pCMe->m_pInnerCalculatorY, a, b);
}

static int32_t ECOCALLMETHOD CKD_Tree_ICalcY_Division(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)((char_t*)me - offsetof(CKD_Tree_B0ABC511, m_pVTblIEcoCalculatorY));
    return pCMe->m_pInnerCalculatorY->pVTbl->Division(pCMe->m_pInnerCalculatorY, a, b);
}

static IEcoCalculatorYVTbl g_xCKD_Tree_ICalcYVTbl = {
    CKD_Tree_ICalcY_QueryInterface,
    CKD_Tree_ICalcY_AddRef,
    CKD_Tree_ICalcY_Release,
    CKD_Tree_ICalcY_Multiplication,
    CKD_Tree_ICalcY_Division
};

/*
 *
 * <summary>
 *   CKD_Tree_BuildTreeTyped Function
 * </summary>
 *
 * <description>
 *   Common implementation of the BuildTree methods for all coordinate types.
 *   The points array may be reordered during construction.
 * </description>
 *
 */
static int16_t CKD_Tree_BuildTreeTyped(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t* points, /* in */ int32_t count, /* in */ uint16_t dimension, /* in */ int16_t type) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;
    int16_t failed = 0;
    int32_t i;

    /* Pointer Validation */
    if (me == 0 || points == 0 || pCMe->m_pIMem == 0) {
        return ERR_ECO_POINTER;
    }
    if (count <= 0 || dimension == 0) {
        return ERR_ECO_POINTER;
    }
    for (i = 0; i < count; i++) {
        if (points[i] == 0 || points[i]->coords == 0) {
            return ERR_ECO_POINTER;
        }
    }

    /* Destroy existing tree if any */
    if (pCMe->m_pRoot != 0) {
        CKD_Tree_FreeNodeRecursive(pCMe->m_pRoot, pCMe->m_pIMem);
        pCMe->m_pRoot = 0;
    }

    pCMe->m_dimension = dimension;
    pCMe->m_pointCount = (uint32_t)count;
    pCMe->m_type = type;

    /* Build tree recursively */
    pCMe->m_pRoot = CKD_Tree_BuildTreeRecursive(points, 0, count, 0, dimension, type, pCMe->m_pIMem, &failed);
    if (failed || pCMe->m_pRoot == 0) {
        /* Do not leave a partially built tree behind */
        CKD_Tree_FreeNodeRecursive(pCMe->m_pRoot, pCMe->m_pIMem);
        pCMe->m_pRoot = 0;
        pCMe->m_dimension = 0;
        pCMe->m_pointCount = 0;
        return ERR_ECO_OUTOFMEMORY;
    }

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   BuildTree Function
 * </summary>
 *
 * <description>
 *   Builds a KD-tree from an array of points with int16_t coordinates.
 *   The points array may be reordered during construction.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_BuildTree(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t* points, /* in */ int32_t count, /* in */ uint16_t dimension) {
    return CKD_Tree_BuildTreeTyped(me, points, count, dimension, KD_TYPE_INT16);
}

/*
 *
 * <summary>
 *   BuildTreeInt Function
 * </summary>
 *
 * <description>
 *   Builds a KD-tree from an array of points with int coordinates.
 *   The points array may be reordered during construction.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_BuildTreeInt(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t* points, /* in */ int32_t count, /* in */ uint16_t dimension) {
    return CKD_Tree_BuildTreeTyped(me, points, count, dimension, KD_TYPE_INT);
}

/*
 *
 * <summary>
 *   BuildTreeLongLong Function
 * </summary>
 *
 * <description>
 *   Builds a KD-tree from an array of points with int64_t coordinates.
 *   The points array may be reordered during construction.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_BuildTreeLongLong(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t* points, /* in */ int32_t count, /* in */ uint16_t dimension) {
    return CKD_Tree_BuildTreeTyped(me, points, count, dimension, KD_TYPE_LONGLONG);
}

/*
 *
 * <summary>
 *   BuildTreeFloat Function
 * </summary>
 *
 * <description>
 *   Builds a KD-tree from an array of points with float coordinates.
 *   The points array may be reordered during construction.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_BuildTreeFloat(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t* points, /* in */ int32_t count, /* in */ uint16_t dimension) {
    return CKD_Tree_BuildTreeTyped(me, points, count, dimension, KD_TYPE_FLOAT);
}

/*
 *
 * <summary>
 *   BuildTreeDouble Function
 * </summary>
 *
 * <description>
 *   Builds a KD-tree from an array of points with double coordinates.
 *   The points array may be reordered during construction.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_BuildTreeDouble(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t* points, /* in */ int32_t count, /* in */ uint16_t dimension) {
    return CKD_Tree_BuildTreeTyped(me, points, count, dimension, KD_TYPE_DOUBLE);
}

/*
 *
 * <summary>
 *   SearchKNearest Function
 * </summary>
 *
 * <description>
 *   Searches for k nearest neighbors of the given point.
 *   Point coordinates must have the type the tree was built with.
 *   Allocates result arrays internally; caller must free them.
 *   Distances in result are squared Euclidean distances.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_SearchKNearest(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t point, /* in */ int16_t k, /* out */ KDSearchResult* result) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;

    /* Pointer Validation */
    if (me == 0 || point == 0 || point->coords == 0 || result == 0) {
        return ERR_ECO_POINTER;
    }
    result->points = 0;
    result->distances = 0;
    result->count = 0;
    if (k <= 0 || pCMe->m_pRoot == 0 || pCMe->m_pIMem == 0) {
        return ERR_ECO_POINTER;
    }

    /* Allocate result arrays */
    result->points = (KDPointPtr_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, (uint16_t)k * sizeof(KDPointPtr_t));
    if (result->points == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    result->distances = (double*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, (uint16_t)k * sizeof(double));
    if (result->distances == 0) {
        pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, result->points);
        result->points = 0;
        return ERR_ECO_OUTOFMEMORY;
    }
    result->count = 0;

    /* Run recursive search */
    CKD_Tree_SearchRecursive(pCMe->m_pRoot, point->coords, k, pCMe->m_dimension, 0, pCMe->m_type, result);

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   DestroyTree Function
 * </summary>
 *
 * <description>
 *   Frees all nodes and resets tree state
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_DestroyTree(/* in */ IKD_TreePtr_t me) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;

    /* Pointer Validation */
    if (me == 0) {
        return ERR_ECO_POINTER;
    }

    if (pCMe->m_pRoot != 0 && pCMe->m_pIMem != 0) {
        CKD_Tree_FreeNodeRecursive(pCMe->m_pRoot, pCMe->m_pIMem);
        pCMe->m_pRoot = 0;
    }
    pCMe->m_dimension = 0;
    pCMe->m_pointCount = 0;
    pCMe->m_type = KD_TYPE_INT16;

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   InsertPoint Function
 * </summary>
 *
 * <description>
 *   Inserts a single point into the existing KD-tree.
 *   Tree must be built first with one of the BuildTree methods;
 *   point coordinates must have the type the tree was built with.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_InsertPoint(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t point) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;
    int16_t inserted = 0;

    /* Pointer Validation */
    if (me == 0 || point == 0 || point->coords == 0) {
        return ERR_ECO_POINTER;
    }
    if (pCMe->m_dimension == 0 || pCMe->m_pIMem == 0) {
        return ERR_ECO_POINTER;
    }

    pCMe->m_pRoot = CKD_Tree_InsertRecursive(pCMe->m_pRoot, point->coords, 0, pCMe->m_dimension, pCMe->m_type, pCMe->m_pIMem, &inserted);
    if (inserted == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    pCMe->m_pointCount++;

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   DeletePoint Function
 * </summary>
 *
 * <description>
 *   Deletes a point with matching coordinates from the KD-tree.
 *   If the point is not found, the tree remains unchanged.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_DeletePoint(/* in */ IKD_TreePtr_t me, /* in */ KDPointPtr_t point) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;
    int16_t deleted = 0;

    /* Pointer Validation */
    if (me == 0 || point == 0 || point->coords == 0) {
        return ERR_ECO_POINTER;
    }
    if (pCMe->m_pRoot == 0 || pCMe->m_pIMem == 0) {
        return ERR_ECO_POINTER;
    }

    pCMe->m_pRoot = CKD_Tree_DeleteRecursive(pCMe->m_pRoot, point->coords, 0, pCMe->m_dimension, pCMe->m_type, pCMe->m_pIMem, &deleted);
    if (deleted && pCMe->m_pointCount > 0) {
        pCMe->m_pointCount--;
    }

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   CKD_Tree_GetMemoryAllocator Function
 * </summary>
 *
 * <description>
 *   Gets the memory allocator interface through the interface bus.
 *   On success the caller owns one reference to *ppIMem.
 * </description>
 *
 */
static int16_t CKD_Tree_GetMemoryAllocator(/* in */ IEcoSystem1* pISys, /* out */ IEcoMemoryAllocator1** ppIMem) {
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    int16_t result = ERR_ECO_POINTER;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    /* Pointer Validation */
    if (pISys == 0 || ppIMem == 0) {
        return result;
    }
    *ppIMem = 0;

    /* Getting the interface for working with the interface bus */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        return ERR_ECO_NOBUS;
    }

    /* Getting the component ID for working with memory */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**)ppIMem);
    /* Check */
    if (result != 0 || *ppIMem == 0) {
        *ppIMem = 0;
        result = ERR_ECO_GET_MEMORY_ALLOCATOR;
    }

    /* Freeing */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/*
 *
 * <summary>
 *   Init Function
 * </summary>
 *
 * <description>
 *   Instance initialization function.
 *   Takes its own references to the system interface and to the memory
 *   allocator (unless already set); they are released in Delete.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD initCKD_Tree_B0ABC511(/*in*/ CKD_Tree_B0ABC511Ptr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CKD_Tree_B0ABC511* pCMe = (CKD_Tree_B0ABC511*)me;
    int16_t result = ERR_ECO_POINTER;

    /* Pointer Validation */
    if (me == 0 || pIUnkSystem == 0) {
        return result;
    }

    /* Storing the pointer to the system interface */
    if (pCMe->m_pISys == 0) {
        result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void **)&pCMe->m_pISys);
        if (result != 0 || pCMe->m_pISys == 0) {
            pCMe->m_pISys = 0;
            return ERR_ECO_NOSYSTEM;
        }
    }

    /* Getting the memory allocator interface */
    if (pCMe->m_pIMem == 0) {
        return CKD_Tree_GetMemoryAllocator(pCMe->m_pISys, &pCMe->m_pIMem);
    }

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   Create Function
 * </summary>
 *
 * <description>
 *   Instance creation function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD createCKD_Tree_B0ABC511(/* in */ CKD_Tree_B0ABC511Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
    int16_t result = ERR_ECO_POINTER;

    /* Pointer Validation */
    if (pCMe == 0) {
        return result; /* ERR_ECO_POINTER */
    }

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   Delete Function
 * </summary>
 *
 * <description>
 *   Instance freeing function
 * </description>
 *
 */
static void ECOCALLMETHOD deleteCKD_Tree_B0ABC511(/* in */ CKD_Tree_B0ABC511Ptr_t pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;

    if (pCMe != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Release contained calculator components */
        if (pCMe->m_pInnerCalculatorY != 0) {
            pCMe->m_pInnerCalculatorY->pVTbl->Release(pCMe->m_pInnerCalculatorY);
            pCMe->m_pInnerCalculatorY = 0;
        }
        if (pCMe->m_pInnerCalculatorX != 0) {
            pCMe->m_pInnerCalculatorX->pVTbl->Release(pCMe->m_pInnerCalculatorX);
            pCMe->m_pInnerCalculatorX = 0;
        }
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
            pCMe->m_pISys = 0;
        }
        if ( pIMem == 0 ) {
            return;
        }
        if ( pCMe->m_pRoot != 0 ) {
            CKD_Tree_FreeNodeRecursive(pCMe->m_pRoot, pIMem);
        }
        if ( pCMe->m_Name != 0 ) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}

/* IKD_Tree Virtual Table */
IKD_TreeVTbl g_xA1711225CF7045B6985ECA20A33CA463VTbl_B0ABC511 = {
    CKD_Tree_B0ABC511_QueryInterface,
    CKD_Tree_B0ABC511_AddRef,
    CKD_Tree_B0ABC511_Release,
    CKD_Tree_B0ABC511_BuildTree,
    CKD_Tree_B0ABC511_SearchKNearest,
    CKD_Tree_B0ABC511_DestroyTree,
    CKD_Tree_B0ABC511_InsertPoint,
    CKD_Tree_B0ABC511_DeletePoint,
    CKD_Tree_B0ABC511_BuildTreeInt,
    CKD_Tree_B0ABC511_BuildTreeLongLong,
    CKD_Tree_B0ABC511_BuildTreeFloat,
    CKD_Tree_B0ABC511_BuildTreeDouble
};

/* Object Instance (template copied into every created instance) */
CKD_Tree_B0ABC511 g_xCKD_Tree_B0ABC511 = {
    &g_xA1711225CF7045B6985ECA20A33CA463VTbl_B0ABC511,
    &g_xCKD_Tree_NondelegatingUnkVTbl,
    &g_xCKD_Tree_ICalcXVTbl,
    &g_xCKD_Tree_ICalcYVTbl,
    initCKD_Tree_B0ABC511,
    createCKD_Tree_B0ABC511,
    deleteCKD_Tree_B0ABC511,
    1,  /* m_cRef */
    0,  /* m_pIMem */
    0,  /* m_pISys */
    0,  /* m_pIUnkOuter */
    0,  /* m_Name */
    0,  /* m_dimension */
    0,  /* m_pRoot */
    0,  /* m_pointCount */
    KD_TYPE_INT16,  /* m_type */
    0,  /* m_pInnerCalculatorX */
    0   /* m_pInnerCalculatorY */
};

/*
 *
 * <summary>
 *   CreateInstance Function
 * </summary>
 *
 * <description>
 *   Allocates and initializes a new instance of the component and returns
 *   its IKD_Tree interface with reference count 1. The instance is freed
 *   by the last Release.
 * </description>
 *
 */
static int16_t CKD_Tree_B0ABC511_CreateCommon(IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter, CKD_Tree_B0ABC511** ppCMe) {
    CKD_Tree_B0ABC511* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoCalculatorX* pInnerX = 0;
    IEcoCalculatorY* pInnerY = 0;
    int16_t result;

    if (ppCMe == 0 || pIUnkSystem == 0) return ERR_ECO_POINTER;
    *ppCMe = 0;

    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void **)&pISys);
    if (result != 0 || pISys == 0) return ERR_ECO_NOSYSTEM;

    result = CKD_Tree_GetMemoryAllocator(pISys, &pIMem);
    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Create contained component A (IEcoCalculatorX) */
    result = CEcoCalculatorA_CreateInstance(pIUnkSystem, 0, &pInnerX);
    if (result != 0 || pInnerX == 0) {
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Create contained component D (IEcoCalculatorY, D contains A internally) */
    result = CEcoCalculatorD_CreateInstance(pIUnkSystem, 0, &pInnerY);
    if (result != 0 || pInnerY == 0) {
        pInnerX->pVTbl->Release(pInnerX);
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return result;
    }

    pCMe = (CKD_Tree_B0ABC511*)pIMem->pVTbl->Alloc(pIMem, sizeof(CKD_Tree_B0ABC511));
    if (pCMe == 0) {
        pInnerY->pVTbl->Release(pInnerY);
        pInnerX->pVTbl->Release(pInnerX);
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }
    pIMem->pVTbl->Copy(pIMem, pCMe, &g_xCKD_Tree_B0ABC511, sizeof(CKD_Tree_B0ABC511));

    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_pISys = pISys;
    pCMe->m_pInnerCalculatorX = pInnerX;
    pCMe->m_pInnerCalculatorY = pInnerY;

    /* Set up aggregation: if pIUnkOuter is provided, delegate to it */
    if (pIUnkOuter != 0) {
        pCMe->m_pIUnkOuter = pIUnkOuter;
    } else {
        pCMe->m_pIUnkOuter = (IEcoUnknown*)&pCMe->m_pVTblINondelegatingUnk;
    }

    result = pCMe->Create(pCMe, pIUnkSystem, pIUnkOuter);
    if (result == 0) {
        result = pCMe->Init(pCMe, pIUnkSystem);
    }
    if (result != 0) {
        pCMe->Delete(pCMe);
        return result;
    }

    *ppCMe = pCMe;
    return ERR_ECO_SUCCESS;
}

int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_CreateInstance(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IKD_Tree** ppIKD_Tree) {
    CKD_Tree_B0ABC511* pCMe = 0;
    int16_t result;

    if (ppIKD_Tree == 0) return ERR_ECO_POINTER;
    *ppIKD_Tree = 0;

    if (pIUnkOuter != 0) return ERR_ECO_NOAGGREGATION;

    result = CKD_Tree_B0ABC511_CreateCommon(pIUnkSystem, 0, &pCMe);
    if (result != 0) return result;

    *ppIKD_Tree = (IKD_Tree*)pCMe;
    return ERR_ECO_SUCCESS;
}

int16_t ECOCALLMETHOD CKD_Tree_B0ABC511_CreateInstanceAggr(/* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter, /* out */ IEcoUnknown** ppIUnk) {
    CKD_Tree_B0ABC511* pCMe = 0;
    int16_t result;

    if (ppIUnk == 0) return ERR_ECO_POINTER;
    *ppIUnk = 0;

    result = CKD_Tree_B0ABC511_CreateCommon(pIUnkSystem, pIUnkOuter, &pCMe);
    if (result != 0) return result;

    /* Return non-delegating IEcoUnknown */
    *ppIUnk = (IEcoUnknown*)&pCMe->m_pVTblINondelegatingUnk;
    return ERR_ECO_SUCCESS;
}
