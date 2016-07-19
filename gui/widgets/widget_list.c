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
#include <string.h>
#include <protothreads/pt.h>
#include <gui/gui.h>

//----------------------------------------------

#define DEFAULT_FONT        GUI_FONT_24_ASCII
#define DEFAULT_BG_COLOR    GUI_BLACK
#define DEFAULT_COLOR       GUI_WHITE

//----------------------------------------------

struct list_item
{
    char *text;
    const GUI_BITMAP *icon;
    struct list_item *next;
};

struct widget_ctx
{
    unsigned short id;

    const GUI_FONT *font;
    int color;
    int bg_color;

    int prev_y;
    int offset;
    int selected_item;

    struct list_item *items;
    int num_items;

    struct pt thread;
};

static struct widget_ctx* get_context(WM_HWIN handle);
static void default_callback(WM_MESSAGE *msg);
static void paint_widget(struct widget_ctx *ctx, WM_MESSAGE *msg);
static void handle_touch(struct widget_ctx *ctx, WM_MESSAGE *msg);
static void destroy_list_items(struct widget_ctx *ctx);

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
    memset(ctx, 0, sizeof(*ctx));

    ctx->id = info->Id;
    ctx->font = DEFAULT_FONT;
    ctx->color = DEFAULT_COLOR;
    ctx->bg_color = DEFAULT_BG_COLOR;
    ctx->selected_item = -1;

    PT_INIT(&ctx->thread);

    WM_SetUserData(handle, &ctx, sizeof(void*));

    return handle;
}

//----------------------------------------------

bool widget_list_add_item(WM_HWIN handle, const char *text, const GUI_BITMAP *icon)
{
    if (!handle || !text)
        return false;

    struct widget_ctx *ctx = get_context(handle);

    struct list_item *last_item = ctx->items;
    while (last_item && last_item->next)
        last_item = last_item->next;

    struct list_item *new_item = malloc(sizeof(*new_item));
    ASSERT_CRIT(new_item != NULL);

    int len = strlen(text) + 1;

    new_item->icon = icon;

    new_item->text = malloc(len);
    ASSERT_CRIT(new_item->text != NULL);

    memcpy(new_item->text, text, len);

    new_item->next = NULL;

    if (last_item)
        last_item->next = new_item;
    else
        ctx->items = new_item;

    ctx->num_items++;

    WM_InvalidateWindow(handle);

    return true;
}

//----------------------------------------------

const char* widget_list_get_item(WM_HWIN handle, int index)
{
    if (!handle || (index < 0))
        return NULL;

    struct widget_ctx *ctx = get_context(handle);

    if (index >= ctx->num_items)
        return NULL;

    struct list_item *item = ctx->items;
    while (index--)
        item = item->next;

    return item ? item->text : NULL;
}

//----------------------------------------------

void widget_list_clear(WM_HWIN handle)
{
    if (!handle)
        return;

    destroy_list_items(get_context(handle));

    WM_InvalidateWindow(handle);
}

//----------------------------------------------

void widget_list_set_font(WM_HWIN handle, const GUI_FONT *font)
{
    if (!handle)
        return;

    get_context(handle)->font = font;

    WM_InvalidateWindow(handle);
}

//----------------------------------------------

void widget_list_set_bg_color(WM_HWIN handle, GUI_COLOR color)
{
    if (!handle)
        return;

    get_context(handle)->color = color;

    WM_InvalidateWindow(handle);
}

//----------------------------------------------

void widget_list_set_color(WM_HWIN handle, GUI_COLOR color)
{
    if (!handle)
        return;

    get_context(handle)->bg_color = color;

    WM_InvalidateWindow(handle);
}

//----------------------------------------------

void widget_list_set_scroll(WM_HWIN handle, int val)
{
    if (!handle)
        return;

    struct widget_ctx *ctx = get_context(handle);

    if (ctx->offset == val)
        return;
    ctx->offset = val;

    WM_InvalidateWindow(handle);
}

//----------------------------------------------

static struct widget_ctx* get_context(WM_HWIN handle)
{
    void *ctx;
    WM_GetUserData(handle, &ctx, sizeof(void*));
    return ctx;
}

//----------------------------------------------

static void destroy_list_items(struct widget_ctx *ctx)
{
    struct list_item *item = ctx->items;
    while (item)
    {
        struct list_item *next = item->next;

        free(item->text);
        free(item);

        item = next;
    }

    ctx->items = NULL;
    ctx->num_items = 0;
}

//----------------------------------------------

static void destroy_widget(struct widget_ctx *ctx, WM_MESSAGE *msg)
{
    destroy_list_items(ctx);
    free(ctx);
}

//----------------------------------------------

static void default_callback(WM_MESSAGE *msg)
{
    struct widget_ctx *ctx = get_context(msg->hWin);

    switch (msg->MsgId)
    {
    case WM_SET_ID:
        ctx->id = msg->Data.v;
        break;
    case WM_GET_ID:
        msg->Data.v = ctx->id;
        break;

    case WM_TOUCH:
        handle_touch(ctx, msg);
        break;

    case WM_DELETE:
        destroy_widget(ctx, msg);
        break;

    case WM_PAINT:
        paint_widget(ctx, msg);
        break;

    default:
        WM_DefaultProc(msg);
        break;
    }
}

//----------------------------------------------

static void paint_widget(struct widget_ctx *ctx, WM_MESSAGE *msg)
{
    GUI_RECT rect;
    WM_GetWindowRectEx(msg->hWin, &rect);

    GUI_MoveRect(&rect, -rect.x0, -rect.y0);

    GUI_SetColor(ctx->bg_color);
    GUI_FillRectEx(&rect);

    rect.y0 = ctx->offset;
    rect.y1 = ctx->offset + ctx->font->YSize;

    GUI_SetFont(ctx->font);

    int i = 0;
    struct list_item *item = ctx->items;
    while (item)
    {
        if (item->icon)
        {
            GUI_DrawBitmap(item->icon, rect.x0, rect.y0);

            GUI_SetColor((i == ctx->selected_item) ? GUI_GREEN : ctx->color);

            int text_offset = item->icon->XSize + 10;
            rect.x0 += text_offset;

            GUI_DispStringInRect(item->text, &rect, GUI_TA_LEFT | GUI_TA_TOP);

            rect.x0 -= text_offset;
        }
        else
        {
            GUI_DispStringInRect(item->text, &rect, GUI_TA_LEFT | GUI_TA_TOP);
        }

        GUI_MoveRect(&rect, 0, ctx->font->YSize);

        item = item->next;
        i++;
    }
}

//----------------------------------------------

static void set_selected_item(WM_HWIN handle, struct widget_ctx *ctx, int selected)
{
    if (selected != ctx->selected_item)
    {
        ctx->selected_item = selected;
        WM_InvalidateWindow(handle);
    }
}

//----------------------------------------------

PT_THREAD(touch_thread(struct pt *pt, WM_HWIN handle, struct widget_ctx *ctx, const GUI_PID_STATE *state))
{
    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, state->Pressed);

    GUI_RECT rect;
    WM_GetWindowRectEx(handle, &rect);

    int selected = (state->y - (rect.y0 + ctx->offset)) / ctx->font->YSize;
    if ((selected < 0) || (selected >= ctx->num_items))
        selected = -1;

    set_selected_item(handle, ctx, selected);

    ctx->prev_y = state->y;

    while (1)
    {
        PT_WAIT_UNTIL(pt, !state->Pressed || (ctx->prev_y != state->y));

        if (!state->Pressed)
        {
            if (ctx->selected_item != -1)
                gui_send_event(handle, WIDGET_LIST_EVENT_SELECTED, (void*)ctx->selected_item);

            set_selected_item(handle, ctx, -1);
            PT_EXIT(pt);
        }

        set_selected_item(handle, ctx, -1);

        ctx->offset += state->y - ctx->prev_y;
        WM_InvalidateWindow(handle);

        ctx->prev_y = state->y;
        PT_YIELD(pt);
    }

    PT_END(pt);
}

//----------------------------------------------

static void handle_touch(struct widget_ctx *ctx, WM_MESSAGE *msg)
{
    const GUI_PID_STATE *state = msg->Data.p;

    touch_thread(&ctx->thread, msg->hWin, ctx, state);
}
