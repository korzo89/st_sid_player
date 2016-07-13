/*
 * debug.h
 *
 *  Created on: 11 lip 2016
 *      Author: Korzo
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <RTT/SEGGER_RTT.h>

//----------------------------------------------

#ifndef CFG_DEBUG_ENABLED
#define CFG_DEBUG_ENABLED       1
#endif

#if CFG_DEBUG_ENABLED
#define DBG_PRINTF(_fmt, ...)   SEGGER_RTT_printf(0, _fmt, ##__VA_ARGS__)
#else
#define DBG_PRINTF(_fmt, ...)   do{}while(0)
#endif

//----------------------------------------------

#endif /* DEBUG_H_ */
