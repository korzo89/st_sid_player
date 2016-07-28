/*
 * widget_icon.c
 *
 *  Created on: 28 lip 2016
 *      Author: Korzo
 */

#include "widget_icon.h"
#include <assert.h>
#include <debug.h>

//----------------------------------------------

struct widget_ctx
{
    const GUI_BITMAP *icon;
    int alignment;
    int offset_x;
    int offset_y;
    GUI_COLOR color;
    GUI_COLOR bg_color;
};

static void callback(WM_MESSAGE *msg);

//----------------------------------------------

WM_HWIN widget_icon_create(int x, int y, int w, int h, WM_HWIN parent, int flags)
{
    WM_HWIN handle = widget_create(x, y, w, h, parent, flags, callback);

    struct widget_ctx *ctx = WIDGET_CREATE_CONTEXT(handle);

    ctx->alignment = GUI_TA_HCENTER | GUI_TA_VCENTER;
    ctx->color = GUI_WHITE;
    ctx->bg_color = GUI_BLACK;

    return handle;
}

//----------------------------------------------

WIDGET_VISUAL_PROPERTY_SETTER(widget_icon_set_icon, const GUI_BITMAP*, icon);

WIDGET_VISUAL_PROPERTY_SETTER(widget_icon_set_alignment, int, alignment);

WIDGET_VISUAL_PROPERTY_SETTER2(widget_icon_set_offset, int, offset_x, int, offset_y);

WIDGET_VISUAL_PROPERTY_SETTER(widget_icon_set_color, GUI_COLOR, color);

WIDGET_VISUAL_PROPERTY_SETTER(widget_icon_set_bg_color, GUI_COLOR, bg_color);

//----------------------------------------------

static void destroy_widget(WM_HWIN handle)
{
    widget_destroy_context(handle);
}

//----------------------------------------------

static void paint_widget(struct widget_ctx *ctx, WM_MESSAGE *msg)
{
    GUI_RECT rect;
    WM_GetClientRectEx(msg->hWin, &rect);

    if (ctx->bg_color != GUI_TRANSPARENT)
    {
        GUI_SetColor(ctx->bg_color);
        GUI_FillRectEx(&rect);
    }

    if (!ctx->icon)
        return;

    int w = rect.x1 - rect.x0 + 1;
    int h = rect.y1 - rect.y0 + 1;

    if (ctx->alignment & GUI_TA_HCENTER)
        rect.x0 += (w - ctx->icon->XSize) / 2;
    else if (ctx->alignment & GUI_TA_RIGHT)
        rect.x0 += w - ctx->icon->XSize;

    if (ctx->alignment & GUI_TA_VCENTER)
        rect.y0 += (h - ctx->icon->YSize) / 2;
    else if (ctx->alignment & GUI_TA_BOTTOM)
        rect.y0 += h - ctx->icon->YSize;

    rect.x0 += ctx->offset_x;
    rect.y0 += ctx->offset_y;

    GUI_SetColor(ctx->color);
    GUI_DrawBitmap(ctx->icon, rect.x0, rect.y0);
}

//----------------------------------------------

static void callback(WM_MESSAGE *msg)
{
    struct widget_ctx *ctx = widget_get_context(msg->hWin);

    switch (msg->MsgId)
    {
    case WM_DELETE:
        destroy_widget(msg->hWin);
        break;

    case WM_PAINT:
        paint_widget(ctx, msg);
        break;

    case WM_PID_STATE_CHANGED:
    {
        const WM_PID_STATE_CHANGED_INFO *info = msg->Data.p;
        if (info->State)
            gui_send_event(msg->hWin, WIDGET_EVENT_PRESSED, NULL);
        break;
    }

    default:
        WM_DefaultProc(msg);
        break;
    }
}
