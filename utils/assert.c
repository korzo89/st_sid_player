/*
 * assert.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "assert.h"
#include <debug.h>

//----------------------------------------------

void assert(bool condition, const char *text, const char *file, long line)
{
#if ASSERT_LEVEL > ASSERT_LEVEL_NONE
    if (!condition)
    {
        DBG_PRINTF("ASSERT FAILED in %s:%d: %s\n", file, line, text);

        asm volatile("bkpt 1");
        while (1)
        {
        }
    }
#endif
}
