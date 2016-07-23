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
#include <utils/tween.h>

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
    const GUI_FONT *font;
    int text_color;
    int icon_color;
    int bg_color;
    int item_height;

    int prev_y;
    int offset;
    int selected_item;

    struct list_item *items;
    int num_items;

    struct pt pt_touch;
};

static struct widget_ctx* get_context(WM_HWIN handle);
static void default_callback(WM_MESSAGE *msg);
static void paint_widget(struct widget_ctx *ctx, WM_MESSAGE *msg);
static void handle_touch(struct widget_ctx *ctx, WM_MESSAGE *msg);
static void destroy_list_items(struct widget_ctx *ctx);

#define VISUAL_PROPERTY_SETTER(_name, _type, _prop)     \
    void _name(WM_HWIN handle, _type val)               \
    {                                                   \
        if (!handle) return;                            \
        struct widget_ctx *ctx = get_context(handle);   \
        if (ctx->_prop == val) return;                  \
        ctx->_prop = val;                               \
        WM_InvalidateWindow(handle);                    \
    }

//----------------------------------------------

WM_HWIN widget_list_create(int x, int y, int w, int h, WM_HWIN parent, int flags)
{
    WM_HWIN handle = WM_CreateWindowAsChild(
            x, y, w, h,
            parent, flags,
            default_callback,
            sizeof(void*));

    struct widget_ctx *ctx = malloc(sizeof(*ctx));
    ASSERT_CRIT(ctx != NULL);
    memset(ctx, 0, sizeof(*ctx));

    ctx->font = DEFAULT_FONT;
    ctx->text_color = DEFAULT_COLOR;
    ctx->icon_color = DEFAULT_COLOR;
    ctx->bg_color = DEFAULT_BG_COLOR;
    ctx->selected_item = -1;

    PT_INIT(&ctx->pt_touch);

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

VISUAL_PROPERTY_SETTER(widget_list_set_font, const GUI_FONT*, font);

VISUAL_PROPERTY_SETTER(widget_list_set_bg_color, GUI_COLOR, bg_color);

VISUAL_PROPERTY_SETTER(widget_list_set_text_color, GUI_COLOR, text_color);

VISUAL_PROPERTY_SETTER(widget_list_set_icon_color, GUI_COLOR, icon_color);

VISUAL_PROPERTY_SETTER(widget_list_set_scroll, int, offset);

VISUAL_PROPERTY_SETTER(widget_list_set_item_height, int, item_height);

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

static int position_to_index(struct widget_ctx *ctx, const GUI_RECT *win_rect, int y)
{
    if (ctx->item_height != 0)
        return (y - (win_rect->y0 + ctx->offset)) / ctx->item_height;
    else
        return -1;
}

//----------------------------------------------

static void draw_item(struct list_item *item, struct widget_ctx *ctx, int x, int y, int w, bool selected)
{
    if (item->icon)
    {
        GUI_SetColor(ctx->icon_color);
        GUI_DrawBitmap(item->icon, x, y);

        x += item->icon->XSize + 10;
    }

    GUI_SetColor(ctx->text_color);
    GUI_DispStringAt(item->text, x, y);
}

//----------------------------------------------

static void paint_widget(struct widget_ctx *ctx, WM_MESSAGE *msg)
{
    GUI_RECT rect;
    WM_GetWindowRectEx(msg->hWin, &rect);

    int w = rect.x1 - rect.x0;

    int first_index = position_to_index(ctx, &rect, rect.y0);
    int last_index = position_to_index(ctx, &rect, rect.y1);

    GUI_MoveRect(&rect, -rect.x0, -rect.y0);

    GUI_SetColor(ctx->bg_color);
    GUI_FillRectEx(&rect);

    GUI_SetFont(ctx->font);

    int x = 0;
    int y = ctx->offset;

    int i = 0;
    struct list_item *item = ctx->items;
    while (item)
    {
        if (i > last_index)
            break;

        if (i >= first_index)
            draw_item(item, ctx, x, y, w, i == ctx->selected_item);

        y += ctx->item_height;

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
    WM_GetClientRectEx(handle, &rect);

    int selected = position_to_index(ctx, &rect, state->y);
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

    touch_thread(&ctx->pt_touch, msg->hWin, ctx, state);
}
