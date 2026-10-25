#ifndef __ID_ECO_LAB2_H__
#define __ID_ECO_LAB2_H__

#include "IEcoBase1.h"
#include "IKD_Tree.h"

/* EcoLab2 CID = {A2789348-D5FC-4F4F-B515-7E1B2412BF98} */
#ifndef __CID_EcoLab2
static const UGUID CID_EcoLab2 = {0x01, 0x10, {0xA2, 0x78, 0x93, 0x48, 0xD5, 0xFC, 0x4F, 0x4F, 0xB5, 0x15, 0x7E, 0x1B, 0x24, 0x12, 0xBF, 0x98}};
#endif

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_A2789348D5FC4F4FB5157E1B2412BF98;
#endif

#endif
