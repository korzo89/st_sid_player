/*
 * app_context.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "app_context.h"

//----------------------------------------------

static struct app_ctx ctx;

//----------------------------------------------

struct app_ctx* app_ctx_get(void)
{
    return &ctx;
}
