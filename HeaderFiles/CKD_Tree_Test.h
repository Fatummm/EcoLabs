/*
 * <summary>
 *   CKD_Tree_Test
 * </summary>
 *
 * <description>
 *   This header describes the tests of the CKD_Tree component.
 *   Every test is self-contained: it creates its own instance of the
 *   component, generates its own data, builds and destroys its own tree,
 *   releases the instance, and can be called on its own in any order.
 *   A test returns 0 on success.
 * </description>
 *
 */

#ifndef __C_KD_TREE_TEST_H__
#define __C_KD_TREE_TEST_H__

#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IEcoStdIOC89.h"

/* 20 points, dimension 3, one test per coordinate type */
int16_t CKD_Tree_Test_Int16_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_Int_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_LongLong_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_Float_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_Double_20(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);

/* int64_t coordinates, dimension 3, different number of points */
int16_t CKD_Tree_Test_LongLong_1(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_LongLong_10(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_LongLong_100(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_LongLong_1000(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_LongLong_10000(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);
int16_t CKD_Tree_Test_LongLong_100000(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);

/* Search load: 100000 points, 10000 queries, int64_t coordinates, dimension 3
   (no timing inside - measure the run with external tools) */
int16_t CKD_Tree_Test_LongLong_100000_Queries(IEcoSystem1* pISys, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO);

#endif /* __C_KD_TREE_TEST_H__ */
