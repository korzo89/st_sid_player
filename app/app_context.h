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

#define APP_ROOT_DIR    "0:"

struct app_ctx
{
    int list_scroll;
    char curr_path[_MAX_LFN + 1];
    char path_buffer[_MAX_LFN + 1];
};

//----------------------------------------------

struct app_ctx* app_ctx_get(void);

//----------------------------------------------

#endif /* APP_APP_CONTEXT_H_ */
