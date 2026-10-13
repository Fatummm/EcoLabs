/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   Lab1
 * </summary>
 *
 * <description>
 *   This source file is the entry point
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 Dmitriy Lepa. All rights reserved.
 * </author>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoStdIOC89.h"
#include "IEcoStdIOC89.h"
#include "IdKD_Tree.h"
#include "CKD_Tree_Test.h"

#define TEST_POINT_COUNT 15
#define TEST_DIMENSION 3
#define TEST_K 4

/*
 *
 * <summary>
 *   EcoMain Function
 * </summary>
 *
 * <description>
 *   EcoMain function - entry point
 * </description>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Pointer to the system interface */
    IEcoSystem1* pISys = 0;
    /* Pointer to the interface for working with the system interface bus */
    IEcoInterfaceBus1* pIBus = 0;
    /* Pointer to the memory management interface */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoStdIOC89* pIStdIO = 0;
    char_t* name = 0;
    /* KD_Tree component — interface pointer only */
    IKD_Tree* pIKDTree = 0;
    /* Test data: 15 points, dimension 3, int64_t coordinates */
    int64_t coords[TEST_POINT_COUNT][TEST_DIMENSION] = {
        { 50, 30, 70 },
        { 20, 80, 10 },
        { 90, 10, 40 },
        { 35, 55, 95 },
        { 65, 45, 25 },
        { 10, 15, 60 },
        { 75, 85, 55 },
        { 40, 20, 30 },
        { 85, 60, 80 },
        { 25, 40, 45 },
        { 60, 95, 15 },
        { 15, 65, 85 },
        { 95, 35, 5 },
        { -5000000000LL, 70, 20 },
        { 5000000000LL, 9000000000LL, -7000000000LL }
    };
    KDPoint points[TEST_POINT_COUNT];
    KDPointPtr_t pPoints[TEST_POINT_COUNT];
    int64_t queryCoords[TEST_DIMENSION] = { 45, 50, 40 };
    KDPoint query;
    KDSearchResult searchResult = { 0, 0, 0 };
    int16_t i = 0;
    uint16_t d = 0;
    int16_t testsFailed = 0;

    /* Pointer Validation */
    if (pIUnk == 0) {
        return ERR_ECO_POINTER;
    }

    /* System interface check and creation */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 || pISys == 0) {
        /* Free the system interface in case of an error */
            pISys = 0;
            goto Release;
        }
    }

    /* Getting the interface for working with the interface bus */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Free in case of an error */
        goto Release;
    }
    /* Getting the memory management interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Check */
    if (result != 0 || pIMem == 0) {
        /* Free the system interface in case of an error */
        goto Release;
    }

    /* Registering the StdIOC89 component on the bus */
    result = pIBus->pVTbl->RegisterComponent(
        pIBus,
        &CID_EcoStdIOC89,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_00000000000000000000000053494F31
    );
    if (result != 0) {
        result = 200;
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoStdIOC89, 0, &IID_IEcoStdIOC89, (void**) &pIStdIO);
    if (result != 0 || pIStdIO == 0) {
        goto Release;
    }

    pIStdIO->pVTbl->printf(pIStdIO, "Hello from EcoMain! Number = %d\n", 42);

    /* Registering the KD_Tree component on the bus */
    result = pIBus->pVTbl->RegisterComponent(
        pIBus,
        &CID_KD_Tree,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_B0ABC511000000000000000000000000
    );
    if (result != 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "KD_Tree: registration failed, result = %d\n", result);
        goto Release;
    }

    /* Creating KD_Tree component through QueryInterface */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_KD_Tree, 0, &IID_IKD_Tree, (void**)&pIKDTree);
    if (result != 0 || pIKDTree == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "KD_Tree: creation failed, result = %d\n", result);
        pIKDTree = 0;
        if (result == 0) {
            result = ERR_ECO_POINTER;
        }
        goto Release;
    }
    pIStdIO->pVTbl->printf(pIStdIO, "KD_Tree: component initialized\n\n");

    /* Input points */
    pIStdIO->pVTbl->printf(pIStdIO, "Input: %d points, dimension %d, type int64_t\n", TEST_POINT_COUNT, TEST_DIMENSION);
    for (i = 0; i < TEST_POINT_COUNT; i++) {
        points[i].coords = coords[i];
        pPoints[i] = &points[i];
        pIStdIO->pVTbl->printf(pIStdIO, "  [%2d] (", i);
        for (d = 0; d < TEST_DIMENSION; d++) {
            pIStdIO->pVTbl->printf(pIStdIO, d == 0 ? "%lld" : ", %lld", coords[i][d]);
        }
        pIStdIO->pVTbl->printf(pIStdIO, ")\n");
    }

    /* Building the tree */
    result = pIKDTree->pVTbl->BuildTreeLongLong(pIKDTree, pPoints, TEST_POINT_COUNT, TEST_DIMENSION);
    pIStdIO->pVTbl->printf(pIStdIO, "\nBuildTreeLongLong: result = %d\n", result);
    if (result != 0) {
        goto Release;
    }

    /* Searching for k nearest neighbours */
    query.coords = queryCoords;
    pIStdIO->pVTbl->printf(pIStdIO, "\nSearchKNearest: k = %d, query = (%lld, %lld, %lld)\n",
        TEST_K, queryCoords[0], queryCoords[1], queryCoords[2]);
    result = pIKDTree->pVTbl->SearchKNearest(pIKDTree, &query, TEST_K, &searchResult);
    pIStdIO->pVTbl->printf(pIStdIO, "SearchKNearest: result = %d\n", result);
    if (result != 0 || searchResult.points == 0 || searchResult.distances == 0) {
        if (result == 0) {
            result = ERR_ECO_POINTER;
        }
        goto Release;
    }
    pIStdIO->pVTbl->printf(pIStdIO, "Found %d neighbours (unordered):\n", searchResult.count);
    for (i = 0; i < (int16_t)searchResult.count; i++) {
        pIStdIO->pVTbl->printf(pIStdIO, "  (");
        for (d = 0; d < TEST_DIMENSION; d++) {
            pIStdIO->pVTbl->printf(pIStdIO, d == 0 ? "%lld" : ", %lld", ((int64_t*)searchResult.points[i]->coords)[d]);
        }
        pIStdIO->pVTbl->printf(pIStdIO, ")  squared distance = %.0f\n", searchResult.distances[i]);
    }
    pIMem->pVTbl->Free(pIMem, searchResult.points);
    pIMem->pVTbl->Free(pIMem, searchResult.distances);
    searchResult.points = 0;
    searchResult.distances = 0;

    /* Destroying the tree */
    result = pIKDTree->pVTbl->DestroyTree(pIKDTree);
    pIStdIO->pVTbl->printf(pIStdIO, "\nDestroyTree: result = %d\n", result);

    /* KD_Tree tests (UnitTestFiles/CKD_Tree_Test.c) */
    testsFailed += CKD_Tree_Test_Int16_20(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_Int_20(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_20(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_Float_20(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_Double_20(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_1(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_10(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_100(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_1000(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_10000(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_100000(pISys, pIMem, pIStdIO);
    testsFailed += CKD_Tree_Test_LongLong_100000_Queries(pISys, pIMem, pIStdIO);
    pIStdIO->pVTbl->printf(pIStdIO, "\nKD_Tree tests failed: %d of 12\n", testsFailed);

    /* Memory block allocation */
    name = (char_t *)pIMem->pVTbl->Alloc(pIMem, 10);
    if (name == 0) {
        result = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }

    /* Fill the memory block */
    pIMem->pVTbl->Fill(pIMem, name, 'a', 9);


    /* Free the memory block */
    pIMem->pVTbl->Free(pIMem, name);

Release:

    /* Free the KD_Tree component (the last Release also frees its tree) */
    if (pIKDTree != 0) {
        pIKDTree->pVTbl->Release(pIKDTree);
    }

    /* Free the interface for working with the interface bus */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Free the memory management interface */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }


    /* Free the system interface */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    if (pIStdIO != 0) {
        pIStdIO->pVTbl->Release(pIStdIO);
    }

    return result;
}
