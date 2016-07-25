/*
 * screen_file_list.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "screens.h"
#include <debug.h>
#include <assert.h>
#include <BUTTON.h>
#include <ff.h>
#include <string.h>
#include <app/app_context.h>
#include <gui/widgets/widget_list.h>
#include <gui/fonts/fonts.h>
#include <icons/icons.h>

//----------------------------------------------

#define WINDOW_PADDING  5

#define ACTION_X        WINDOW_PADDING
#define ACTION_Y        WINDOW_PADDING
#define ACTION_WIDTH    40
#define ACTION_HEIGHT   40

#define HEADER_X        (ACTION_X + ACTION_WIDTH + 10)
#define HEADER_Y        WINDOW_PADDING
#define HEADER_WIDTH    (LCD_WIDTH - WINDOW_PADDING - (ACTION_X + ACTION_WIDTH))
#define HEADER_HEIGHT   40

#define LIST_X          WINDOW_PADDING
#define LIST_Y          (HEADER_Y + HEADER_HEIGHT)
#define LIST_WIDTH      HEADER_WIDTH
#define LIST_HEIGHT     (LCD_HEIGHT - (HEADER_Y + HEADER_HEIGHT) - WINDOW_PADDING)

#define ROOT_DIR    ""

struct wnd_ctx
{
    GUI_HWIN header;
    GUI_HWIN list;

    char curr_path[_MAX_LFN + 1];
    char buffer[_MAX_LFN + 1];
};

static struct wnd_ctx *ctx;

static void list_curr_dir(void);

//----------------------------------------------

static void create(GUI_HWIN wnd)
{
    GUI_ALLOC_CTX(ctx);

    WINDOW_SetBkColor(wnd, GUI_BLACK);

    ctx->header = TEXT_CreateAsChild(
            HEADER_X, HEADER_Y,
            HEADER_WIDTH, HEADER_HEIGHT,
            wnd, 0, WM_CF_SHOW,
            "", GUI_TA_LEFT | GUI_TA_TOP);

    ctx->list = widget_list_create(
            LIST_X, LIST_Y,
            LIST_WIDTH, LIST_HEIGHT,
            wnd, WM_CF_SHOW);

    TEXT_SetTextColor(ctx->header, GUI_WHITE);
    TEXT_SetFont(ctx->header, &GUI_FontSegoe_UI_Semibold35);

    const GUI_FONT *font = &GUI_FontSegoe_UI_Semibold30;

    widget_list_set_font(ctx->list, font);
    widget_list_set_item_height(ctx->list, font->YSize);
    widget_list_set_icon_color(ctx->list, GUI_RGBH(0x3B98C1));

    strcpy(ctx->curr_path, ROOT_DIR);

    list_curr_dir();
}

//----------------------------------------------

static void destroy(void)
{
    GUI_FREE_CTX(ctx);
}

//----------------------------------------------

static void list_curr_dir(void)
{
    strcpy(ctx->buffer, "0:");
    if (ctx->curr_path[0] != '/')
        strcat(ctx->buffer, "/");
    strcat(ctx->buffer, ctx->curr_path);

    TEXT_SetText(ctx->header, ctx->buffer);

    widget_list_clear(ctx->list);
    widget_list_set_scroll(ctx->list, 0);

    if (strcmp(ctx->curr_path, ROOT_DIR) != 0)
        widget_list_add_item(ctx->list, "..", &icon_folder);

    DIR dir;
    FRESULT res = f_opendir(&dir, ctx->curr_path);
    ASSERT_WARN(res == FR_OK);

    strcat(ctx->curr_path, "/");
    DBG_PRINTF("curr_path: %s\n", ctx->curr_path);

    while (1)
    {
        FILINFO info;
        res = f_readdir(&dir, &info);
        ASSERT_WARN(res == FR_OK);

        if (info.fname[0] == '\0')
            break;

        bool is_dir = info.fattrib & AM_DIR;

        if (!is_dir)
        {
            int len = strlen(info.fname);
            if (len < 4)
                continue;

            const char *ext = &info.fname[len - 4];
            if ((strcmp(ext, ".sid") != 0) && (strcmp(ext, ".SID") != 0))
                continue;
        }

        widget_list_add_item(ctx->list, info.fname,
                info.fattrib & AM_DIR ? &icon_folder : &icon_music);
    }

    f_closedir(&dir);
}

//----------------------------------------------

static void move_to_parent_dir(void)
{
    int len = strlen(ctx->curr_path);
    char *c = ctx->curr_path + (len - 2);
    do
    {
        if (*c == '/')
        {
            *c = '\0';
            return;
        }
    }
    while (c-- != ctx->curr_path);
}

//----------------------------------------------

static void list_item_selected(int selected)
{
    const char *item = widget_list_get_item(ctx->list, selected);

    DBG_PRINTF("selected: %s\n", item);

    bool is_dir;
    if (strcmp(item, "..") == 0)
    {
        move_to_parent_dir();
        is_dir = true;
    }
    else
    {
        strcat(ctx->curr_path, item);

        FILINFO info;
        FRESULT res = f_stat(ctx->curr_path, &info);
        ASSERT_WARN(res == FR_OK);

        is_dir = !!(info.fattrib & AM_DIR);
    }

    if (is_dir)
    {
        list_curr_dir();
        return;
    }

    strcpy(app_ctx_get()->path_buffer, ctx->curr_path);

    gui_show_screen(&screen_player);
}

//----------------------------------------------

static void handle_event(const struct gui_event *event)
{
    if ((event->sender == ctx->list) && (event->id == WIDGET_LIST_EVENT_SELECTED))
        list_item_selected((int)event->data);
}

//----------------------------------------------

static void process(void)
{
}

//----------------------------------------------

struct gui_screen screen_file_list = SIMPLE_SCREEN();
