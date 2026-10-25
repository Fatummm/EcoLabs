/*
 * Lab1 — Entry point and UnitTest
 *
 * Demonstrates:
 *   1) KD-Tree (Lab1) component with containment of calculators A and D
 *   2) Components A, B, C, D, E — all calculator variants
 *   3) Lab2 component with aggregation of Lab1 and containment of B, E
 *   4) QueryInterface navigation between all interfaces
 */

/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoStdIOC89.h"
#include "IEcoStdIOC89.h"
#include "IdKD_Tree.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "CKD_Tree.h"
#include "CEcoCalculatorA.h"
#include "CEcoCalculatorB.h"
#include "CEcoCalculatorC.h"
#include "CEcoCalculatorD.h"
#include "CEcoCalculatorE.h"
#include "CEcoLab2.h"
#include "CKD_Tree_Test.h"

#define TEST_POINT_COUNT 15
#define TEST_DIMENSION 3
#define TEST_K 4

#define CHECK(cond) do { if (!(cond)) { testsFailed++; pIStdIO->pVTbl->printf(pIStdIO, "FAIL: %s (line %d)\n", #cond, __LINE__); } } while(0)

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoStdIOC89* pIStdIO = 0;
    char_t* name = 0;

    IKD_Tree* pIKDTree = 0;
    IEcoCalculatorX* pICalcX = 0;
    IEcoCalculatorY* pICalcY = 0;

    IEcoCalculatorX* pICalcA = 0;
    IEcoUnknown* pIUnkB = 0;
    IEcoCalculatorX* pICalcB = 0;
    IEcoCalculatorX* pICalcC = 0;
    IEcoCalculatorY* pICalcCY = 0;
    IEcoCalculatorY* pICalcD = 0;
    IEcoCalculatorX* pICalcDX = 0;
    IEcoCalculatorY* pICalcE = 0;
    IEcoCalculatorX* pICalcEX = 0;

    IKD_Tree* pILab2 = 0;
    IEcoCalculatorX* pILab2X = 0;
    IEcoCalculatorY* pILab2Y = 0;

    /* Interface navigation temps */
    IEcoCalculatorY* pTempY = 0;
    IEcoCalculatorX* pTempX = 0;
    IKD_Tree* pTempKD = 0;

    int64_t coords[TEST_POINT_COUNT][TEST_DIMENSION] = {
        { 50, 30, 70 }, { 20, 80, 10 }, { 90, 10, 40 },
        { 35, 55, 95 }, { 65, 45, 25 }, { 10, 15, 60 },
        { 75, 85, 55 }, { 40, 20, 30 }, { 85, 60, 80 },
        { 25, 40, 45 }, { 60, 95, 15 }, { 15, 65, 85 },
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
    int16_t testsFailed = 0;

    if (pIUnk == 0) return ERR_ECO_POINTER;

    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
    if (result != 0 || pISys == 0) { pISys = 0; goto Release; }

    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) goto Release;

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    if (result != 0 || pIMem == 0) goto Release;

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoStdIOC89, (IEcoUnknown*)GetIEcoComponentFactoryPtr_00000000000000000000000053494F31);
    if (result != 0) { result = 200; goto Release; }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoStdIOC89, 0, &IID_IEcoStdIOC89, (void**) &pIStdIO);
    if (result != 0 || pIStdIO == 0) goto Release;

    /* ============ Section 1: Standalone components A-E ============ */

    /* Component A */
    result = CEcoCalculatorA_CreateInstance((IEcoUnknown*)pISys, 0, &pICalcA);
    CHECK(result == 0 && pICalcA != 0);
    if (pICalcA != 0) {
        CHECK(pICalcA->pVTbl->Addition(pICalcA, 10, 3) == 13);
        CHECK(pICalcA->pVTbl->Subtraction(pICalcA, 10, 3) == 7);
        pICalcA->pVTbl->Release(pICalcA);
    }

    /* Component B */
    result = CEcoCalculatorB_CreateInstance((IEcoUnknown*)pISys, 0, &pIUnkB);
    CHECK(result == 0 && pIUnkB != 0);
    if (pIUnkB != 0) {
        result = pIUnkB->pVTbl->QueryInterface(pIUnkB, &IID_IEcoCalculatorX, (void**)&pICalcB);
        CHECK(result == 0 && pICalcB != 0);
        if (pICalcB != 0) {
            CHECK(pICalcB->pVTbl->Addition(pICalcB, 20, 7) == 27);
            CHECK(pICalcB->pVTbl->Subtraction(pICalcB, 20, 7) == 13);
            pICalcB->pVTbl->Release(pICalcB);
        }
        pIUnkB->pVTbl->Release(pIUnkB);
    }

    /* Component C */
    result = CEcoCalculatorC_CreateInstance((IEcoUnknown*)pISys, 0, &pICalcC);
    CHECK(result == 0 && pICalcC != 0);
    if (pICalcC != 0) {
        CHECK(pICalcC->pVTbl->Addition(pICalcC, 5, 8) == 13);
        CHECK(pICalcC->pVTbl->Subtraction(pICalcC, 5, 8) == -3);
        result = pICalcC->pVTbl->QueryInterface(pICalcC, &IID_IEcoCalculatorY, (void**)&pICalcCY);
        CHECK(result == 0 && pICalcCY != 0);
        if (pICalcCY != 0) {
            CHECK(pICalcCY->pVTbl->Multiplication(pICalcCY, 6, 7) == 42);
            CHECK(pICalcCY->pVTbl->Division(pICalcCY, 42, 6) == 7);
            pICalcCY->pVTbl->Release(pICalcCY);
        }
        pICalcC->pVTbl->Release(pICalcC);
    }

    /* Component D (containment of A) */
    result = CEcoCalculatorD_CreateInstance((IEcoUnknown*)pISys, 0, &pICalcD);
    CHECK(result == 0 && pICalcD != 0);
    if (pICalcD != 0) {
        CHECK(pICalcD->pVTbl->Multiplication(pICalcD, 9, 4) == 36);
        CHECK(pICalcD->pVTbl->Division(pICalcD, 100, 3) == 33);
        result = pICalcD->pVTbl->QueryInterface(pICalcD, &IID_IEcoCalculatorX, (void**)&pICalcDX);
        CHECK(result == 0 && pICalcDX != 0);
        if (pICalcDX != 0) {
            CHECK(pICalcDX->pVTbl->Addition(pICalcDX, 15, 25) == 40);
            CHECK(pICalcDX->pVTbl->Subtraction(pICalcDX, 15, 25) == -10);
            pICalcDX->pVTbl->Release(pICalcDX);
        }
        pICalcD->pVTbl->Release(pICalcD);
    }

    /* Component E (aggregation of B) */
    result = CEcoCalculatorE_CreateInstance((IEcoUnknown*)pISys, 0, &pICalcE);
    CHECK(result == 0 && pICalcE != 0);
    if (pICalcE != 0) {
        CHECK(pICalcE->pVTbl->Multiplication(pICalcE, 12, 5) == 60);
        CHECK(pICalcE->pVTbl->Division(pICalcE, 99, 10) == 9);
        result = pICalcE->pVTbl->QueryInterface(pICalcE, &IID_IEcoCalculatorX, (void**)&pICalcEX);
        CHECK(result == 0 && pICalcEX != 0);
        if (pICalcEX != 0) {
            CHECK(pICalcEX->pVTbl->Addition(pICalcEX, 100, 200) == 300);
            CHECK(pICalcEX->pVTbl->Subtraction(pICalcEX, 100, 200) == -100);
            pICalcEX->pVTbl->Release(pICalcEX);
        }
        pICalcE->pVTbl->Release(pICalcE);
    }

    /* ============ Section 2: Lab1 (KD-Tree) with containment ============ */

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_KD_Tree, (IEcoUnknown*)GetIEcoComponentFactoryPtr_B0ABC511000000000000000000000000);
    CHECK(result == 0);
    if (result != 0) goto Release;

    result = CKD_Tree_B0ABC511_CreateInstance((IEcoUnknown*)pISys, 0, &pIKDTree);
    CHECK(result == 0 && pIKDTree != 0);
    if (pIKDTree == 0) goto Release;

    /* QI: IKD_Tree -> IEcoCalculatorX (containment of A) */
    result = pIKDTree->pVTbl->QueryInterface(pIKDTree, &IID_IEcoCalculatorX, (void**)&pICalcX);
    CHECK(result == 0 && pICalcX != 0);
    if (pICalcX != 0) {
        CHECK(pICalcX->pVTbl->Addition(pICalcX, 3, 5) == 8);
        CHECK(pICalcX->pVTbl->Subtraction(pICalcX, 10, 4) == 6);
    }

    /* QI: IKD_Tree -> IEcoCalculatorY (containment of D) */
    result = pIKDTree->pVTbl->QueryInterface(pIKDTree, &IID_IEcoCalculatorY, (void**)&pICalcY);
    CHECK(result == 0 && pICalcY != 0);
    if (pICalcY != 0) {
        CHECK(pICalcY->pVTbl->Multiplication(pICalcY, 6, 7) == 42);
        CHECK(pICalcY->pVTbl->Division(pICalcY, 20, 3) == 6);
    }

    /* Interface navigation: any -> any */
    if (pICalcX != 0) {
        result = pICalcX->pVTbl->QueryInterface(pICalcX, &IID_IEcoCalculatorY, (void**)&pTempY);
        CHECK(result == 0 && pTempY != 0);
        if (pTempY != 0) pTempY->pVTbl->Release(pTempY);

        result = pICalcX->pVTbl->QueryInterface(pICalcX, &IID_IKD_Tree, (void**)&pTempKD);
        CHECK(result == 0 && pTempKD != 0);
        if (pTempKD != 0) pTempKD->pVTbl->Release(pTempKD);
    }
    if (pICalcY != 0) {
        result = pICalcY->pVTbl->QueryInterface(pICalcY, &IID_IEcoCalculatorX, (void**)&pTempX);
        CHECK(result == 0 && pTempX != 0);
        if (pTempX != 0) pTempX->pVTbl->Release(pTempX);

        result = pICalcY->pVTbl->QueryInterface(pICalcY, &IID_IKD_Tree, (void**)&pTempKD);
        CHECK(result == 0 && pTempKD != 0);
        if (pTempKD != 0) pTempKD->pVTbl->Release(pTempKD);
    }

    if (pICalcX != 0) { pICalcX->pVTbl->Release(pICalcX); pICalcX = 0; }
    if (pICalcY != 0) { pICalcY->pVTbl->Release(pICalcY); pICalcY = 0; }

    /* KD-Tree build & search */
    for (i = 0; i < TEST_POINT_COUNT; i++) {
        points[i].coords = coords[i];
        pPoints[i] = &points[i];
    }
    result = pIKDTree->pVTbl->BuildTreeLongLong(pIKDTree, pPoints, TEST_POINT_COUNT, TEST_DIMENSION);
    CHECK(result == 0);
    if (result == 0) {
        query.coords = queryCoords;
        result = pIKDTree->pVTbl->SearchKNearest(pIKDTree, &query, TEST_K, &searchResult);
        CHECK(result == 0 && searchResult.count == TEST_K);
        if (searchResult.points != 0) pIMem->pVTbl->Free(pIMem, searchResult.points);
        if (searchResult.distances != 0) pIMem->pVTbl->Free(pIMem, searchResult.distances);
        pIKDTree->pVTbl->DestroyTree(pIKDTree);
    }
    pIKDTree->pVTbl->Release(pIKDTree);
    pIKDTree = 0;

    /* ============ Section 3: Lab2 (aggregation + containment) ============ */

    result = CEcoLab2_CreateInstance((IEcoUnknown*)pISys, 0, &pILab2);
    CHECK(result == 0 && pILab2 != 0);
    if (pILab2 != 0) {
        /* IEcoCalculatorX via contained B */
        result = pILab2->pVTbl->QueryInterface(pILab2, &IID_IEcoCalculatorX, (void**)&pILab2X);
        CHECK(result == 0 && pILab2X != 0);
        if (pILab2X != 0) {
            CHECK(pILab2X->pVTbl->Addition(pILab2X, 50, 25) == 75);
            CHECK(pILab2X->pVTbl->Subtraction(pILab2X, 50, 25) == 25);
            pILab2X->pVTbl->Release(pILab2X);
        }

        /* IEcoCalculatorY via contained E */
        result = pILab2->pVTbl->QueryInterface(pILab2, &IID_IEcoCalculatorY, (void**)&pILab2Y);
        CHECK(result == 0 && pILab2Y != 0);
        if (pILab2Y != 0) {
            CHECK(pILab2Y->pVTbl->Multiplication(pILab2Y, 11, 11) == 121);
            CHECK(pILab2Y->pVTbl->Division(pILab2Y, 100, 7) == 14);
            pILab2Y->pVTbl->Release(pILab2Y);
        }

        /* KD-Tree via aggregated Lab1 */
        for (i = 0; i < TEST_POINT_COUNT; i++) {
            points[i].coords = coords[i];
            pPoints[i] = &points[i];
        }
        result = pILab2->pVTbl->BuildTreeLongLong(pILab2, pPoints, TEST_POINT_COUNT, TEST_DIMENSION);
        CHECK(result == 0);
        if (result == 0) {
            query.coords = queryCoords;
            searchResult.points = 0;
            searchResult.distances = 0;
            searchResult.count = 0;
            result = pILab2->pVTbl->SearchKNearest(pILab2, &query, 2, &searchResult);
            CHECK(result == 0 && searchResult.count == 2);
            if (searchResult.points != 0) pIMem->pVTbl->Free(pIMem, searchResult.points);
            if (searchResult.distances != 0) pIMem->pVTbl->Free(pIMem, searchResult.distances);
            pILab2->pVTbl->DestroyTree(pILab2);
        }

        pILab2->pVTbl->Release(pILab2);
    }

    /* ============ Section 4: KD-Tree unit tests ============ */

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

    /* ============ Result ============ */

    if (testsFailed == 0) {
        pIStdIO->pVTbl->printf(pIStdIO, "All tests passed\n");
    } else {
        pIStdIO->pVTbl->printf(pIStdIO, "Tests failed: %d\n", testsFailed);
    }

    /* Memory block allocation */
    name = (char_t *)pIMem->pVTbl->Alloc(pIMem, 10);
    if (name == 0) {
        result = ERR_ECO_OUTOFMEMORY;
        goto Release;
    }
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
