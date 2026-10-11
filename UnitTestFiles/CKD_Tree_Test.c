/*
 * <summary>
 *   CKD_Tree_Test
 * </summary>
 *
 * <description>
 *   This source code describes the tests of the CKD_Tree component.
 *   Every test builds a tree from pseudo-random points, checks the k nearest
 *   neighbours of several query points against a brute-force search, then
 *   checks InsertPoint and DeletePoint and destroys the tree.
 * </description>
 *
 */

#include "CKD_Tree.h"
#include "CKD_Tree_Test.h"

#define KD_TEST_DIMENSION   3
#define KD_TEST_K           5
#define KD_TEST_QUERIES     5

/* Coordinate range for the tests with 20 points and for the tests by size */
#define KD_TEST_RANGE_SMALL 200
#define KD_TEST_RANGE_LARGE 2000000

/* Search speed test */
#define KD_TEST_SPEED_POINTS    100000
#define KD_TEST_SPEED_QUERIES   10000
/* Every KD_TEST_SPEED_VERIFY-th query is checked against brute force */
#define KD_TEST_SPEED_VERIFY    100


/* ============================================================ */
/*                    Helper Functions                           */
/* ============================================================ */

/*
 *
 * <summary>
 *   CKD_Tree_Test_Random Function
 * </summary>
 *
 * <description>
 *   Linear congruential generator. Returns a value in [-range/2, range/2).
 *   The same seed always gives the same sequence, so tests are repeatable.
 * </description>
 *
 */
static int32_t CKD_Tree_Test_Random(uint32_t* pSeed, int32_t range) {
    *pSeed = (*pSeed * 1103515245UL + 12345UL) & 0x7FFFFFFFUL;
    return (int32_t)((*pSeed >> 8) % (uint32_t)range) - range / 2;
}

/*
 *
 * <summary>
 *   CKD_Tree_Test_ElemSize Function
 * </summary>
 *
 * <description>
 *   Returns the size in bytes of one coordinate of the given type
 * </description>
 *
 */
static uint32_t CKD_Tree_Test_ElemSize(int16_t type) {
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
 *   CKD_Tree_Test_SetCoord Function
 * </summary>
 *
 * <description>
 *   Stores a generated value into coords[index] of the given type.
 *   int64_t values are scaled beyond the 32-bit range, float and double
 *   values get a fractional part.
 * </description>
 *
 */
static void CKD_Tree_Test_SetCoord(voidptr_t coords, uint32_t index, int16_t type, int32_t value) {
    switch (type) {
        case KD_TYPE_INT:
            ((int*)coords)[index] = (int)value;
            break;
        case KD_TYPE_LONGLONG:
            ((int64_t*)coords)[index] = (int64_t)value * 100000;
            break;
        case KD_TYPE_FLOAT:
            ((float*)coords)[index] = (float)value / 4.0f;
            break;
        case KD_TYPE_DOUBLE:
            ((double*)coords)[index] = (double)value / 8.0;
            break;
        default:
            ((int16_t*)coords)[index] = (int16_t)value;
            break;
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_Test_GetCoord Function
 * </summary>
 *
 * <description>
 *   Reads coords[index] of the given type as double
 * </description>
 *
 */
static double CKD_Tree_Test_GetCoord(voidptr_t coords, uint32_t index, int16_t type) {
    switch (type) {
        case KD_TYPE_INT:
            return (double)((int*)coords)[index];
        case KD_TYPE_LONGLONG:
            return (double)((int64_t*)coords)[index];
        case KD_TYPE_FLOAT:
            return (double)((float*)coords)[index];
        case KD_TYPE_DOUBLE:
            return ((double*)coords)[index];
        default:
            return (double)((int16_t*)coords)[index];
    }
}

/*
 *
 * <summary>
 *   CKD_Tree_Test_PrintPoint Function
 * </summary>
 *
 * <description>
 *   Prints coordinates of a point as (x, y, z)
 * </description>
 *
 */
static void CKD_Tree_Test_PrintPoint(IEcoStdIOC89* pIStdIO, voidptr_t coords, int16_t type) {
    uint32_t i;
    const char_t* format = (type == KD_TYPE_FLOAT || type == KD_TYPE_DOUBLE) ? "%s%.3f" : "%s%.0f";

    pIStdIO->pVTbl->printf(pIStdIO, "(");
    for (i = 0; i < KD_TEST_DIMENSION; i++) {
        pIStdIO->pVTbl->printf(pIStdIO, format, i == 0 ? "" : ", ", CKD_Tree_Test_GetCoord(coords, i, type));
    }
    pIStdIO->pVTbl->printf(pIStdIO, ")");
}

/*
 *
 * <summary>
 *   CKD_Tree_Test_BruteForce Function
 * </summary>
 *
 * <description>
 *   Reference search: scans all points and writes the k smallest squared
 *   distances to the query into best[] in ascending order.
 *   Returns the number of distances written (min(k, count)).
 * </description>
 *
 */
static int16_t CKD_Tree_Test_BruteForce(voidptr_t coords, int32_t count, voidptr_t query, int16_t type, int16_t k, double* best) {
    uint32_t elemSize = CKD_Tree_Test_ElemSize(type);
    int16_t found = 0;
    int16_t j;
    int32_t i;
    uint32_t d;
    double diff;
    double dist;
    voidptr_t point;

    for (i = 0; i < count; i++) {
        point = (char_t*)coords + (uint32_t)i * KD_TEST_DIMENSION * elemSize;
        dist = 0.0;
        for (d = 0; d < KD_TEST_DIMENSION; d++) {
            diff = CKD_Tree_Test_GetCoord(query, d, type) - CKD_Tree_Test_GetCoord(point, d, type);
            dist += diff * diff;
        }
        if (found == k && dist >= best[k - 1]) {
            continue;
        }
        /* Insert into the sorted list of the best distances */
        j = (found < k) ? found++ : (int16_t)(k - 1);
        while (j > 0 && best[j - 1] > dist) {
            best[j] = best[j - 1];
            j--;
        }
        best[j] = dist;
    }

    return found;
}

/*
 *
 * <summary>
 *   CKD_Tree_Test_CheckSearch Function
 * </summary>
 *
 * <description>
 *   Runs SearchKNearest and compares the returned distances with the
 *   brute-force reference. Returns 0 if they match.
 * </description>
 *
 */
static int16_t CKD_Tree_Test_CheckSearch(
    IKD_Tree* pIKDTree,
    IEcoMemoryAllocator1* pIMem,
    IEcoStdIOC89* pIStdIO,
    voidptr_t coords,
    int32_t count,
    voidptr_t queryCoords,
    int16_t type,
    int16_t k,
    bool_t print)
{
    KDSearchResult searchResult = { 0, 0, 0 };
    KDPoint query;
    KDPointPtr_t tempPoint;
    double best[KD_TEST_K];
    double tempDist;
    double diff;
    int16_t expected;
    int16_t result;
    int16_t failed = 0;
    int16_t i;
    int16_t j;

    query.coords = queryCoords;
    result = pIKDTree->pVTbl->SearchKNearest(pIKDTree, &query, k, &searchResult);
    if (result != 0 || searchResult.points == 0 || searchResult.distances == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "    SearchKNearest failed, result = %d\n", result);
        return 1;
    }

    expected = CKD_Tree_Test_BruteForce(coords, count, queryCoords, type, k, best);

    /* The component returns neighbours unordered: sort them by distance */
    for (i = 0; i < (int16_t)searchResult.count; i++) {
        for (j = i + 1; j < (int16_t)searchResult.count; j++) {
            if (searchResult.distances[j] < searchResult.distances[i]) {
                tempDist = searchResult.distances[i];
                searchResult.distances[i] = searchResult.distances[j];
                searchResult.distances[j] = tempDist;
                tempPoint = searchResult.points[i];
                searchResult.points[i] = searchResult.points[j];
                searchResult.points[j] = tempPoint;
            }
        }
    }

    if ((int16_t)searchResult.count != expected) {
        failed = 1;
    }
    for (i = 0; failed == 0 && i < expected; i++) {
        diff = searchResult.distances[i] - best[i];
        if (diff < 0.0) {
            diff = -diff;
        }
        if (diff > best[i] * 1e-9) {
            failed = 1;
        }
    }

    if (print || failed) {
        pIStdIO->pVTbl->printf(pIStdIO, "    query ");
        CKD_Tree_Test_PrintPoint(pIStdIO, queryCoords, type);
        pIStdIO->pVTbl->printf(pIStdIO, ": found %d, expected %d\n", (int)searchResult.count, (int)expected);
        for (i = 0; i < (int16_t)searchResult.count; i++) {
            pIStdIO->pVTbl->printf(pIStdIO, "      ");
            CKD_Tree_Test_PrintPoint(pIStdIO, searchResult.points[i]->coords, type);
            pIStdIO->pVTbl->printf(pIStdIO, "  squared distance = %.3f", searchResult.distances[i]);
            if (i < expected) {
                pIStdIO->pVTbl->printf(pIStdIO, "  (brute force: %.3f)", best[i]);
            }
            pIStdIO->pVTbl->printf(pIStdIO, "\n");
        }
    }

    pIMem->pVTbl->Free(pIMem, searchResult.points);
    pIMem->pVTbl->Free(pIMem, searchResult.distances);

    return failed;
}

/*
 *
 * <summary>
 *   CKD_Tree_Test_Run Function
 * </summary>
 *
 * <description>
 *   Common body of the tests: `count` points of the given coordinate type.
 *   Returns 0 if all checks passed.
 * </description>
 *
 */
static int16_t CKD_Tree_Test_Run(
    IEcoSystem1* pISys,
    IEcoMemoryAllocator1* pIMem,
    IEcoStdIOC89* pIStdIO,
    const char_t* name,
    int16_t type,
    int32_t count,
    int32_t range)
{
    CKD_Tree_B0ABC511* pCKDTree = 0;
    IKD_Tree* pIKDTree = 0;
    uint32_t elemSize = CKD_Tree_Test_ElemSize(type);
    uint32_t pointSize = KD_TEST_DIMENSION * elemSize;
    uint32_t seed = 20261011UL + (uint32_t)type * 7919UL + (uint32_t)count;
    voidptr_t coords = 0;
    voidptr_t queryCoords = 0;
    KDPoint* points = 0;
    KDPointPtr_t* pPoints = 0;
    KDPoint query;
    int16_t k = (count < KD_TEST_K) ? (int16_t)count : KD_TEST_K;
    int16_t result = 0;
    int16_t failed = 0;
    int16_t mismatches = 0;
    int16_t q;
    int32_t i;
    uint32_t d;
    uint32_t refs;

    /* Pointer Validation */
    if (pISys == 0 || pIMem == 0 || pIStdIO == 0 || name == 0 || count <= 0) {
        return 1;
    }

    pIStdIO->pVTbl->printf(pIStdIO, "\n[TEST] %s: %d points, dimension %d\n", name, (int)count, KD_TEST_DIMENSION);

    /* Every test works with its own instance of the component */
    result = CKD_Tree_B0ABC511_CreateInstance((IEcoUnknown*)pISys, 0, &pIKDTree);
    if (result != 0 || pIKDTree == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "  component creation failed, result = %d\n", result);
        pIKDTree = 0;
        failed = 1;
        goto Done;
    }
    pCKDTree = (CKD_Tree_B0ABC511*)pIKDTree;

    /* Test data */
    coords = pIMem->pVTbl->Alloc(pIMem, (uint32_t)count * pointSize);
    points = (KDPoint*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)count * sizeof(KDPoint));
    pPoints = (KDPointPtr_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)count * sizeof(KDPointPtr_t));
    queryCoords = pIMem->pVTbl->Alloc(pIMem, pointSize);
    if (coords == 0 || points == 0 || pPoints == 0 || queryCoords == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "  out of memory for test data\n");
        failed = 1;
        goto Done;
    }
    for (i = 0; i < count; i++) {
        points[i].coords = (char_t*)coords + (uint32_t)i * pointSize;
        pPoints[i] = &points[i];
        for (d = 0; d < KD_TEST_DIMENSION; d++) {
            CKD_Tree_Test_SetCoord(points[i].coords, d, type, CKD_Tree_Test_Random(&seed, range));
        }
    }

    /* 1. Building the tree */
    switch (type) {
        case KD_TYPE_INT:
            result = pIKDTree->pVTbl->BuildTreeInt(pIKDTree, pPoints, count, KD_TEST_DIMENSION);
            break;
        case KD_TYPE_LONGLONG:
            result = pIKDTree->pVTbl->BuildTreeLongLong(pIKDTree, pPoints, count, KD_TEST_DIMENSION);
            break;
        case KD_TYPE_FLOAT:
            result = pIKDTree->pVTbl->BuildTreeFloat(pIKDTree, pPoints, count, KD_TEST_DIMENSION);
            break;
        case KD_TYPE_DOUBLE:
            result = pIKDTree->pVTbl->BuildTreeDouble(pIKDTree, pPoints, count, KD_TEST_DIMENSION);
            break;
        default:
            result = pIKDTree->pVTbl->BuildTree(pIKDTree, pPoints, count, KD_TEST_DIMENSION);
            break;
    }
    pIStdIO->pVTbl->printf(pIStdIO, "  build: result = %d, points in tree = %d\n", result, (int)pCKDTree->m_pointCount);
    if (result != 0 || pCKDTree->m_pointCount != (uint32_t)count) {
        failed = 1;
        goto Done;
    }

    /* 2. k nearest neighbours of random query points against brute force */
    for (q = 0; q < KD_TEST_QUERIES; q++) {
        for (d = 0; d < KD_TEST_DIMENSION; d++) {
            CKD_Tree_Test_SetCoord(queryCoords, d, type, CKD_Tree_Test_Random(&seed, range));
        }
        /* Neighbours are printed for the first query only */
        mismatches += CKD_Tree_Test_CheckSearch(pIKDTree, pIMem, pIStdIO, coords, count, queryCoords, type, k, (bool_t)(q == 0));
    }
    pIStdIO->pVTbl->printf(pIStdIO, "  search: %d queries, k = %d, mismatches = %d\n", KD_TEST_QUERIES, (int)k, (int)mismatches);
    if (mismatches != 0) {
        failed = 1;
    }

    /* 3. Insert the last query point: it must become its own nearest neighbour */
    query.coords = queryCoords;
    result = pIKDTree->pVTbl->InsertPoint(pIKDTree, &query);
    {
        KDSearchResult searchResult;
        double nearest = -1.0;

        if (result == 0 && pIKDTree->pVTbl->SearchKNearest(pIKDTree, &query, 1, &searchResult) == 0) {
            if (searchResult.count == 1 && searchResult.distances != 0) {
                nearest = searchResult.distances[0];
            }
            pIMem->pVTbl->Free(pIMem, searchResult.points);
            pIMem->pVTbl->Free(pIMem, searchResult.distances);
        }
        pIStdIO->pVTbl->printf(pIStdIO, "  insert: result = %d, points in tree = %d, distance to inserted point = %.3f\n",
            result, (int)pCKDTree->m_pointCount, nearest);
        if (result != 0 || pCKDTree->m_pointCount != (uint32_t)count + 1 || nearest != 0.0) {
            failed = 1;
        }
    }

    /* 4. Delete it: the tree must again match the original set of points */
    result = pIKDTree->pVTbl->DeletePoint(pIKDTree, &query);
    mismatches = CKD_Tree_Test_CheckSearch(pIKDTree, pIMem, pIStdIO, coords, count, queryCoords, type, k, 0);
    pIStdIO->pVTbl->printf(pIStdIO, "  delete: result = %d, points in tree = %d, mismatches = %d\n",
        result, (int)pCKDTree->m_pointCount, (int)mismatches);
    if (result != 0 || pCKDTree->m_pointCount != (uint32_t)count || mismatches != 0) {
        failed = 1;
    }

Done:
    /* 5. Destroying the tree, releasing the component and freeing the test data */
    if (pIKDTree != 0) {
        result = pIKDTree->pVTbl->DestroyTree(pIKDTree);
        refs = pIKDTree->pVTbl->Release(pIKDTree);
        pIStdIO->pVTbl->printf(pIStdIO, "  destroy: result = %d, references left after Release = %d\n", result, (int)refs);
        if (result != 0 || refs != 0) {
            failed = 1;
        }
    }
    if (coords != 0) {
        pIMem->pVTbl->Free(pIMem, coords);
    }
    if (points != 0) {
        pIMem->pVTbl->Free(pIMem, points);
    }
    if (pPoints != 0) {
        pIMem->pVTbl->Free(pIMem, pPoints);
    }
    if (queryCoords != 0) {
        pIMem->pVTbl->Free(pIMem, queryCoords);
    }

    pIStdIO->pVTbl->printf(pIStdIO, "[%s] %s\n", failed ? "FAILED" : "PASSED", name);

    return failed;
}


/* ============================================================ */
/*          Tests by coordinate type: 20 points each             */
/* ============================================================ */

int16_t CKD_Tree_Test_Int16_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int16_t, 20 points", KD_TYPE_INT16, 20, KD_TEST_RANGE_SMALL);
}

int16_t CKD_Tree_Test_Int_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int, 20 points", KD_TYPE_INT, 20, KD_TEST_RANGE_SMALL);
}

int16_t CKD_Tree_Test_LongLong_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 20 points", KD_TYPE_LONGLONG, 20, KD_TEST_RANGE_SMALL);
}

int16_t CKD_Tree_Test_Float_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "float, 20 points", KD_TYPE_FLOAT, 20, KD_TEST_RANGE_SMALL);
}

int16_t CKD_Tree_Test_Double_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "double, 20 points", KD_TYPE_DOUBLE, 20, KD_TEST_RANGE_SMALL);
}


/* ============================================================ */
/*        Tests by number of points: int64_t coordinates         */
/* ============================================================ */

int16_t CKD_Tree_Test_LongLong_1(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 1 point", KD_TYPE_LONGLONG, 1, KD_TEST_RANGE_LARGE);
}

int16_t CKD_Tree_Test_LongLong_10(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 10 points", KD_TYPE_LONGLONG, 10, KD_TEST_RANGE_LARGE);
}

int16_t CKD_Tree_Test_LongLong_100(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 100 points", KD_TYPE_LONGLONG, 100, KD_TEST_RANGE_LARGE);
}

int16_t CKD_Tree_Test_LongLong_1000(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 1000 points", KD_TYPE_LONGLONG, 1000, KD_TEST_RANGE_LARGE);
}

int16_t CKD_Tree_Test_LongLong_10000(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 10000 points", KD_TYPE_LONGLONG, 10000, KD_TEST_RANGE_LARGE);
}

int16_t CKD_Tree_Test_LongLong_100000(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    return CKD_Tree_Test_Run(pISys, pIMem, pIStdIO, "int64_t, 100000 points", KD_TYPE_LONGLONG, 100000, KD_TEST_RANGE_LARGE);
}


/* ============================================================ */
/*                      Search speed test                        */
/* ============================================================ */

/*
 *
 * <summary>
 *   CKD_Tree_Test_LongLong_100000_Queries Function
 * </summary>
 *
 * <description>
 *   Builds a tree of 100000 points with int64_t coordinates and runs 10000
 *   k nearest neighbour queries; every 100th query is also checked against
 *   brute force. The test does not measure time itself: time the run with
 *   external tools. Returns 0 if all checks passed.
 * </description>
 *
 */
int16_t CKD_Tree_Test_LongLong_100000_Queries(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    const int16_t type = KD_TYPE_LONGLONG;
    const uint32_t pointSize = KD_TEST_DIMENSION * sizeof(int64_t);
    IKD_Tree* pIKDTree = 0;
    voidptr_t coords = 0;
    voidptr_t queries = 0;
    voidptr_t queryCoords = 0;
    KDPoint* points = 0;
    KDPointPtr_t* pPoints = 0;
    KDPoint query;
    KDSearchResult searchResult = { 0, 0, 0 };
    uint32_t seed = 20261011UL;
    uint32_t refs;
    uint32_t d;
    int32_t i;
    int32_t found = 0;
    int32_t verified = 0;
    int16_t mismatches = 0;
    int16_t result = 0;
    int16_t failed = 0;

    /* Pointer Validation */
    if (pISys == 0 || pIMem == 0 || pIStdIO == 0) {
        return 1;
    }

    pIStdIO->pVTbl->printf(pIStdIO, "\n[TEST] int64_t, search speed: %d points, %d queries, k = %d, dimension %d\n",
        KD_TEST_SPEED_POINTS, KD_TEST_SPEED_QUERIES, KD_TEST_K, KD_TEST_DIMENSION);

    result = CKD_Tree_B0ABC511_CreateInstance((IEcoUnknown*)pISys, 0, &pIKDTree);
    if (result != 0 || pIKDTree == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "  component creation failed, result = %d\n", result);
        pIKDTree = 0;
        failed = 1;
        goto Done;
    }

    /* Test data: points and queries */
    coords = pIMem->pVTbl->Alloc(pIMem, KD_TEST_SPEED_POINTS * pointSize);
    points = (KDPoint*)pIMem->pVTbl->Alloc(pIMem, KD_TEST_SPEED_POINTS * sizeof(KDPoint));
    pPoints = (KDPointPtr_t*)pIMem->pVTbl->Alloc(pIMem, KD_TEST_SPEED_POINTS * sizeof(KDPointPtr_t));
    queries = pIMem->pVTbl->Alloc(pIMem, KD_TEST_SPEED_QUERIES * pointSize);
    if (coords == 0 || points == 0 || pPoints == 0 || queries == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "  out of memory for test data\n");
        failed = 1;
        goto Done;
    }
    for (i = 0; i < KD_TEST_SPEED_POINTS; i++) {
        points[i].coords = (char_t*)coords + (uint32_t)i * pointSize;
        pPoints[i] = &points[i];
        for (d = 0; d < KD_TEST_DIMENSION; d++) {
            CKD_Tree_Test_SetCoord(points[i].coords, d, type, CKD_Tree_Test_Random(&seed, KD_TEST_RANGE_LARGE));
        }
    }
    for (i = 0; i < KD_TEST_SPEED_QUERIES; i++) {
        for (d = 0; d < KD_TEST_DIMENSION; d++) {
            CKD_Tree_Test_SetCoord((char_t*)queries + (uint32_t)i * pointSize, d, type, CKD_Tree_Test_Random(&seed, KD_TEST_RANGE_LARGE));
        }
    }

    /* 1. Building the tree */
    result = pIKDTree->pVTbl->BuildTreeLongLong(pIKDTree, pPoints, KD_TEST_SPEED_POINTS, KD_TEST_DIMENSION);
    pIStdIO->pVTbl->printf(pIStdIO, "  build: result = %d\n", result);
    if (result != 0) {
        failed = 1;
        goto Done;
    }

    /* 2. All queries through the tree */
    for (i = 0; i < KD_TEST_SPEED_QUERIES; i++) {
        query.coords = (char_t*)queries + (uint32_t)i * pointSize;
        result = pIKDTree->pVTbl->SearchKNearest(pIKDTree, &query, KD_TEST_K, &searchResult);
        if (result != 0) {
            break;
        }
        found += searchResult.count;
        pIMem->pVTbl->Free(pIMem, searchResult.points);
        pIMem->pVTbl->Free(pIMem, searchResult.distances);
    }
    pIStdIO->pVTbl->printf(pIStdIO, "  search: %d queries, neighbours found = %d\n", KD_TEST_SPEED_QUERIES, (int)found);
    if (result != 0 || found != (int32_t)KD_TEST_SPEED_QUERIES * KD_TEST_K) {
        pIStdIO->pVTbl->printf(pIStdIO, "  SearchKNearest failed, result = %d\n", result);
        failed = 1;
        goto Done;
    }

    /* 3. Every KD_TEST_SPEED_VERIFY-th query: tree results against brute force */
    for (i = 0; i < KD_TEST_SPEED_QUERIES; i += KD_TEST_SPEED_VERIFY) {
        queryCoords = (char_t*)queries + (uint32_t)i * pointSize;
        mismatches += CKD_Tree_Test_CheckSearch(pIKDTree, pIMem, pIStdIO, coords, KD_TEST_SPEED_POINTS, queryCoords, type, KD_TEST_K, 0);
        verified++;
    }
    pIStdIO->pVTbl->printf(pIStdIO, "  verify: %d queries checked against brute force, mismatches = %d\n", (int)verified, (int)mismatches);
    if (mismatches != 0) {
        failed = 1;
    }
Done:
    /* 4. Destroying the tree, releasing the component and freeing the test data */
    if (pIKDTree != 0) {
        result = pIKDTree->pVTbl->DestroyTree(pIKDTree);
        refs = pIKDTree->pVTbl->Release(pIKDTree);
        pIStdIO->pVTbl->printf(pIStdIO, "  destroy: result = %d, references left after Release = %d\n", result, (int)refs);
        if (result != 0 || refs != 0) {
            failed = 1;
        }
    }
    if (coords != 0) {
        pIMem->pVTbl->Free(pIMem, coords);
    }
    if (points != 0) {
        pIMem->pVTbl->Free(pIMem, points);
    }
    if (pPoints != 0) {
        pIMem->pVTbl->Free(pIMem, pPoints);
    }
    if (queries != 0) {
        pIMem->pVTbl->Free(pIMem, queries);
    }

    pIStdIO->pVTbl->printf(pIStdIO, "[%s] int64_t, search speed\n", failed ? "FAILED" : "PASSED");

    return failed;
}
