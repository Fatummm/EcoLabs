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
 *   This header describes the component identifier CID_KD_Tree
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 Dmitriy Lepa. All rights reserved.
 * </author>
 *
 */

#ifndef __ID_KD_TREE_H__
#define __ID_KD_TREE_H__

#include "IEcoBase1.h"
#include "IKD_Tree.h"

/* KD_Tree CID = {B0ABC511-0000-0000-0000-000000000000} */
#ifndef __CID_KD_Tree
static const UGUID CID_KD_Tree = {0x01, 0x10, {0xB0, 0xAB, 0xC5, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#endif /* __CID_KD_Tree */

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_B0ABC511000000000000000000000000;
#endif

#endif /* __ID_KD_TREE_H__ */
