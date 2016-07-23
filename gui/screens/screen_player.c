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

static void create(GUI_HWIN wnd)
{
    GUI_ALLOC_CTX(ctx);

    const char *path = app_ctx_get()->path_buffer;

    sid_player_play(path);

    const struct sid_info *info = sid_player_get_info();

    GUI_HWIN text;

    text = TEXT_CreateAsChild(
            0, 30, LCD_WIDTH, 30,
            wnd, 0, WM_CF_SHOW,
            info->title, TEXT_CF_HCENTER);
    TEXT_SetFont(text, GUI_FONT_24_ASCII);

    text = TEXT_CreateAsChild(
            0, 60, LCD_WIDTH, 30,
            wnd, 0, WM_CF_SHOW,
            info->author, TEXT_CF_HCENTER);
    TEXT_SetFont(text, GUI_FONT_20_ASCII);

    text = TEXT_CreateAsChild(
            0, 90, LCD_WIDTH, 30,
            wnd, 0, WM_CF_SHOW,
            info->released, TEXT_CF_HCENTER);
    TEXT_SetFont(text, GUI_FONT_20_ASCII);

    text = TEXT_CreateAsChild(
            0, LCD_HEIGHT - 20, LCD_WIDTH, 20,
            wnd, 0, WM_CF_SHOW,
            path, TEXT_CF_HCENTER);
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
