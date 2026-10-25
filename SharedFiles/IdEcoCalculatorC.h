#ifndef __ID_ECO_CALCULATOR_C_H__
#define __ID_ECO_CALCULATOR_C_H__

#include "IEcoBase1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"

/* EcoCalculatorC CID = {AB34CC55-1234-5678-9ABC-DEF012345678} */
#ifndef __CID_EcoCalculatorC
static const UGUID CID_EcoCalculatorC = {0x01, 0x10, {0xAB, 0x34, 0xCC, 0x55, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78}};
#endif

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_AB34CC551234567899ABCDEF01234567;
#endif

#endif
