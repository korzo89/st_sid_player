/*
 * widget.c
 *
 *  Created on: 25 lip 2016
 *      Author: Korzo
 */

#include "widget.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//----------------------------------------------

WM_HWIN widget_create(int x, int y, int w, int h, WM_HWIN parent, int flags, widget_callback_t cb)
{
    return WM_CreateWindowAsChild(
            x, y, w, h,
            parent, flags,
            cb,
            sizeof(void*));
}

//----------------------------------------------

void* widget_create_context(WM_HWIN handle, size_t size)
{
    ASSERT_WARN(handle);

    void *ctx = malloc(size);
    ASSERT_CRIT(ctx != NULL);
    memset(ctx, 0, size);

    WM_SetUserData(handle, &ctx, sizeof(void*));

    return ctx;
}

//----------------------------------------------

void widget_destroy_context(WM_HWIN handle)
{
    ASSERT_WARN(handle);

    free(widget_get_context(handle));
}

//----------------------------------------------

void* widget_get_context(WM_HWIN handle)
{
    ASSERT_WARN(handle);

    void *ctx;
    WM_GetUserData(handle, &ctx, sizeof(void*));
    return ctx;
}
