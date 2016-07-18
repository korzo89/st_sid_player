/*
 * widget_list.c
 *
 *  Created on: 18 lip 2016
 *      Author: Korzo
 */

#include "widget_list.h"
#include <debug.h>
#include <assert.h>
#include <stdlib.h>
#include <protothreads/pt.h>

//----------------------------------------------

struct widget_ctx
{
    int color;
    int prev_y;
    struct pt thread;
};

static void default_callback(WM_MESSAGE *msg);
static void paint_widget(WM_MESSAGE *msg);
static void handle_touch(WM_MESSAGE *msg);

//----------------------------------------------

WM_HWIN widget_list_create(const GUI_WIDGET_CREATE_INFO *info, WM_HWIN parent, int x, int y, WM_CALLBACK *cb)
{
    ASSERT_WARN(info != NULL);

    if (!cb)
        cb = default_callback;

    WM_HWIN handle = WM_CreateWindowAsChild(
            info->x0, info->y0,
            info->xSize, info->ySize,
            parent, 0, cb,
            sizeof(void*));

    struct widget_ctx *ctx = malloc(sizeof(*ctx));
    ASSERT_CRIT(ctx != NULL);

    ctx->color = GUI_RED;

    PT_INIT(&ctx->thread);

    WM_SetUserData(handle, &ctx, sizeof(void*));

    return handle;
}

//----------------------------------------------

static struct widget_ctx* get_context(WM_HWIN handle)
{
    void *ctx;
    WM_GetUserData(handle, &ctx, sizeof(void*));
    return ctx;
}

//----------------------------------------------

static void destroy_widget(WM_MESSAGE *msg)
{
    free(get_context(msg->hWin));
}

//----------------------------------------------

static void default_callback(WM_MESSAGE *msg)
{
    switch (msg->MsgId)
    {
    case WM_TOUCH:
        handle_touch(msg);
        break;
    case WM_DELETE:
        destroy_widget(msg);
        break;
    case WM_PAINT:
        paint_widget(msg);
        break;
    default:
        WM_DefaultProc(msg);
        break;
    }
}

//----------------------------------------------

static void paint_widget(WM_MESSAGE *msg)
{
    struct widget_ctx *ctx = get_context(msg->hWin);

    GUI_RECT win_rect;
    WM_GetWindowRectEx(msg->hWin, &win_rect);

    GUI_MoveRect(&win_rect, -win_rect.x0, -win_rect.y0);

    GUI_SetColor(ctx->color);
    GUI_FillRectEx(&win_rect);
}

//----------------------------------------------

PT_THREAD(touch_thread(struct pt *pt, struct widget_ctx *ctx, const GUI_PID_STATE *state))
{
    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, state->Pressed);
    DBG_PRINTF("pressed\n");
    ctx->prev_y = state->y;

    while (1)
    {
        PT_WAIT_UNTIL(pt, !state->Pressed || (ctx->prev_y != state->y));

        if (!state->Pressed)
        {
            DBG_PRINTF("released\n");
            PT_EXIT(pt);
        }

        DBG_PRINTF("moved\n");
        PT_YIELD(pt);
    }

    PT_END(pt);
}

//----------------------------------------------

static void handle_touch(WM_MESSAGE *msg)
{
    struct widget_ctx *ctx = get_context(msg->hWin);

    const GUI_PID_STATE *state = msg->Data.p;

//    WM_InvalidateWindow(msg->hWin);

    touch_thread(&ctx->thread, ctx, state);
}
