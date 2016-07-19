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

//----------------------------------------------

struct wnd_ctx
{
};

static struct wnd_ctx *ctx;

//----------------------------------------------

#define TITLE_ID        WID(1)
#define AUTHOR_ID       WID(2)
#define RELEASED_ID     WID(3)
#define FILE_ID         WID(4)

static const GUI_WIDGET_CREATE_INFO resources[] = {
    { WINDOW_CreateIndirect, "Window", 0, 0, 0, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0 },
    { TEXT_CreateIndirect, "", TITLE_ID, 0, 30, LCD_WIDTH, 30 },
    { TEXT_CreateIndirect, "", AUTHOR_ID, 1, 60, LCD_WIDTH, 30 },
    { TEXT_CreateIndirect, "", RELEASED_ID, 0, 90, LCD_WIDTH, 30 },
    { TEXT_CreateIndirect, "", FILE_ID, 0, LCD_HEIGHT - 20, LCD_WIDTH, 20 },
};

//----------------------------------------------

static void create(GUI_HWIN wnd)
{
    GUI_ALLOC_CTX(ctx);

    const char *path = app_ctx_get()->path_buffer;

    sid_player_play(path);

    const struct sid_info *info = sid_player_get_info();

    GUI_HWIN text = WM_GetDialogItem(wnd, TITLE_ID);
    TEXT_SetFont(text, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(text, TEXT_CF_HCENTER);
    TEXT_SetText(text, info->title);

    text = WM_GetDialogItem(wnd, AUTHOR_ID);
    TEXT_SetFont(text, GUI_FONT_20_ASCII);
    TEXT_SetTextAlign(text, TEXT_CF_HCENTER);
    TEXT_SetText(text, info->author);

    text = WM_GetDialogItem(wnd, RELEASED_ID);
    TEXT_SetFont(text, GUI_FONT_20_ASCII);
    TEXT_SetTextAlign(text, TEXT_CF_HCENTER);
    TEXT_SetText(text, info->released);

    text = WM_GetDialogItem(wnd, FILE_ID);
    TEXT_SetTextAlign(text, TEXT_CF_HCENTER);
    TEXT_SetText(text, path);
}

//----------------------------------------------

static void destroy(void)
{
    GUI_FREE_CTX(ctx);
}

//----------------------------------------------

static void handle_event(const struct gui_event *event)
{
}

//----------------------------------------------

static void process(void)
{
    sid_player_process();
}

//----------------------------------------------

struct gui_screen screen_player = SIMPLE_SCREEN();
