/*
 * assert.h
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#ifndef UTILS_ASSERT_H_
#define UTILS_ASSERT_H_

#include <stdbool.h>

//----------------------------------------------

#define ASSERT_LEVEL_NONE   0
#define ASSERT_LEVEL_CRIT   1
#define ASSERT_LEVEL_WARN   2

#ifndef ASSERT_LEVEL
#define ASSERT_LEVEL        ASSERT_LEVEL_WARN
#endif

#if ASSERT_LEVEL >= ASSERT_LEVEL_WARN
#define ASSERT_WARN(_cond)  assert((_cond), #_cond, __FILE__, __LINE__)
#else
#define ASSERT_WARN(_cond)  UNUSED(_cond)
#endif

#if ASSERT_LEVEL >= ASSERT_LEVEL_CRIT
#define ASSERT_CRIT(_cond)  assert((_cond), #_cond, __FILE__, __LINE__)
#else
#define ASSERT_CRIT(_cond)  UNUSED(_cond)
#endif

//----------------------------------------------

void assert(bool condition, const char *text, const char *file, long line);

//----------------------------------------------

#endif /* UTILS_ASSERT_H_ */
