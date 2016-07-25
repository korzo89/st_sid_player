/*
 * widget_snapshot.c
 *
 *  Created on: 25 lip 2016
 *      Author: Korzo
 */

#include "widget_snapshot.h"
#include <assert.h>

//----------------------------------------------

struct widget_ctx
{
    GUI_MEMDEV_Handle memdev;
};

static void callback(WM_MESSAGE *msg);

//----------------------------------------------

WM_HWIN widget_snapshot_create(int x, int y, int w, int h, WM_HWIN parent, int flags)
{
    WM_HWIN handle = widget_create(x, y, w, h, parent, flags, callback);

    struct widget_ctx *ctx = WIDGET_CREATE_CONTEXT(handle);

    ctx->memdev = GUI_MEMDEV_CreateFixed32(0, 0, w, h);
    ASSERT_CRIT(ctx->memdev);

    return handle;
}

//----------------------------------------------

void widget_snapshot_capture(WM_HWIN handle, int x0, int y0)
{
    struct widget_ctx *ctx = widget_get_context(handle);

    GUI_MEMDEV_SetOrg(ctx->memdev, x0, y0);
    GUI_MEMDEV_CopyFromLCD(ctx->memdev);

    WM_InvalidateWindow(handle);
}

//----------------------------------------------

static void destroy_widget(WM_HWIN handle, struct widget_ctx *ctx)
{
    GUI_MEMDEV_Delete(ctx->memdev);
    widget_destroy_context(handle);
}

//----------------------------------------------

static void paint_widget(struct widget_ctx *ctx, WM_MESSAGE *msg)
{
    GUI_RECT rect;
    WM_GetWindowRectEx(msg->hWin, &rect);

    GUI_MEMDEV_WriteAt(ctx->memdev, rect.x0, rect.y0);
}

//----------------------------------------------

static void callback(WM_MESSAGE *msg)
{
    struct widget_ctx *ctx = widget_get_context(msg->hWin);

    switch (msg->MsgId)
    {
    case WM_DELETE:
        destroy_widget(msg->hWin, ctx);
        break;

    case WM_PAINT:
        paint_widget(ctx, msg);
        break;

    default:
        WM_DefaultProc(msg);
        break;
    }
}
