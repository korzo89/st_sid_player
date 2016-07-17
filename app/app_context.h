/*
 * app_context.h
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#ifndef APP_APP_CONTEXT_H_
#define APP_APP_CONTEXT_H_

#include <config.h>
#include <ff.h>

//----------------------------------------------

struct app_ctx
{
    char path_buffer[_MAX_LFN + 1];
};

//----------------------------------------------

struct app_ctx* app_ctx_get(void);

//----------------------------------------------

#endif /* APP_APP_CONTEXT_H_ */
