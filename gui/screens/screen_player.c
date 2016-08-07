/*
 * screen_player.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "screens.h"
#include <assert.h>
#include <BUTTON.h>
#include <TEXT.h>
#include <string.h>
#include <sid/sid_player.h>
#include <app/app_context.h>
#include <gui/fonts/fonts.h>
#include <gui/widgets/widget_icon.h>
#include <icons/icons.h>

//----------------------------------------------

#define WINDOW_PADDING  5

#define ACTION_X        WINDOW_PADDING
#define ACTION_Y        WINDOW_PADDING
#define ACTION_WIDTH    45
#define ACTION_HEIGHT   ACTION_WIDTH

#define TITLE_FONT      (&GUI_FontSegoe_UI_Semibold35)
#define TEXT_FONT       (&GUI_FontSegoe_UI_Semibold30)
#define SMALL_FONT      (&GUI_FontSegoe_UI_Semibold15)

struct wnd_ctx
{
    GUI_HWIN action;
};

static struct wnd_ctx *ctx;

//----------------------------------------------

static void create(GUI_HWIN wnd)
{
    GUI_ALLOC_CTX(ctx);

    WINDOW_SetBkColor(wnd, GUI_BLACK);

    ctx->action = widget_icon_create(
                ACTION_X, ACTION_Y,
                ACTION_WIDTH, ACTION_HEIGHT,
                wnd, WM_CF_SHOW);

    widget_icon_set_icon(ctx->action, &icon_back);

    const char *path = app_ctx_get()->path_buffer;

    sid_player_play(path);

    const struct sid_info *info = sid_player_get_info();

    GUI_HWIN text;

    int y = 30;

    text = TEXT_CreateAsChild(
            0, y, LCD_WIDTH, TITLE_FONT->YSize,
            wnd, 0, WM_CF_SHOW,
            info->title, TEXT_CF_HCENTER);
    TEXT_SetFont(text, TITLE_FONT);
    TEXT_SetTextColor(text, GUI_WHITE);

    y += TITLE_FONT->YSize;

    text = TEXT_CreateAsChild(
            0, y, LCD_WIDTH, TEXT_FONT->YSize,
            wnd, 0, WM_CF_SHOW,
            info->author, TEXT_CF_HCENTER);
    TEXT_SetFont(text, TEXT_FONT);
    TEXT_SetTextColor(text, GUI_WHITE);

    y += TEXT_FONT->YSize + 5;

    text = TEXT_CreateAsChild(
            0, y, LCD_WIDTH, TEXT_FONT->YSize,
            wnd, 0, WM_CF_SHOW,
            info->released, TEXT_CF_HCENTER);
    TEXT_SetFont(text, TEXT_FONT);
    TEXT_SetTextColor(text, GUI_WHITE);

    y = LCD_HEIGHT - SMALL_FONT->YSize - 5;

    text = TEXT_CreateAsChild(
            0, y, LCD_WIDTH, SMALL_FONT->YSize,
            wnd, 0, WM_CF_SHOW,
            path, TEXT_CF_HCENTER);
    TEXT_SetFont(text, SMALL_FONT);
    TEXT_SetTextColor(text, GUI_WHITE);
}

//----------------------------------------------

static void destroy(void)
{
    sid_player_stop();

    GUI_FREE_CTX(ctx);
}

//----------------------------------------------

static void handle_event(const struct gui_event *event)
{
    switch (event->id)
    {
    case WIDGET_EVENT_PRESSED:
        if (event->sender == ctx->action)
            gui_show_screen(&screen_file_list);
        break;

    default:
        break;
    }
}

//----------------------------------------------

static void process(void)
{
    sid_player_process();
}

//----------------------------------------------

struct gui_screen screen_player = SIMPLE_SCREEN();
