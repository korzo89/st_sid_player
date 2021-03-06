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
#include <gui/widgets/widget_icon.h>
#include <gui/fonts/fonts.h>
#include <icons/icons.h>

//----------------------------------------------

#define WINDOW_PADDING  5

#define ACTION_X        WINDOW_PADDING
#define ACTION_Y        WINDOW_PADDING
#define ACTION_WIDTH    45
#define ACTION_HEIGHT   ACTION_WIDTH

#define HEADER_X        (ACTION_X + ACTION_WIDTH + 10)
#define HEADER_Y        WINDOW_PADDING
#define HEADER_WIDTH    (LCD_WIDTH - WINDOW_PADDING - HEADER_X)
#define HEADER_HEIGHT   ACTION_HEIGHT

#define LIST_X          WINDOW_PADDING
#define LIST_Y          (HEADER_Y + HEADER_HEIGHT + 5)
#define LIST_WIDTH      HEADER_WIDTH
#define LIST_HEIGHT     (LCD_HEIGHT - LIST_Y - WINDOW_PADDING)

struct wnd_ctx
{
    struct app_ctx *app;

    GUI_HWIN action;
    GUI_HWIN header;
    GUI_HWIN list;

    bool is_at_root;
};

static struct wnd_ctx *ctx;

static void list_curr_dir(void);

//----------------------------------------------

static void create(GUI_HWIN wnd)
{
    GUI_ALLOC_CTX(ctx);

    ctx->app = app_ctx_get();

    WINDOW_SetBkColor(wnd, GUI_BLACK);

    ctx->action = widget_icon_create(
            ACTION_X, ACTION_Y,
            ACTION_WIDTH, ACTION_HEIGHT,
            wnd, WM_CF_SHOW);

    ctx->header = TEXT_CreateAsChild(
            HEADER_X, HEADER_Y,
            HEADER_WIDTH, HEADER_HEIGHT,
            wnd, 0, WM_CF_SHOW,
            "", GUI_TA_LEFT | GUI_TA_TOP);

    TEXT_SetTextColor(ctx->header, GUI_WHITE);
    TEXT_SetTextAlign(ctx->header, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(ctx->header, &GUI_FontSegoe_UI_Semibold35);

    ctx->list = widget_list_create(
            LIST_X, LIST_Y,
            LIST_WIDTH, LIST_HEIGHT,
            wnd, WM_CF_SHOW);

    const GUI_FONT *font = &GUI_FontSegoe_UI_Semibold30;

    widget_list_set_font(ctx->list, font);
    widget_list_set_item_height(ctx->list, font->YSize);
    widget_list_set_icon_color(ctx->list, GUI_RGBH(0x3B98C1));

    ctx->is_at_root = true;
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
    int len = strlen(ctx->app->curr_path);
    if (ctx->app->curr_path[len - 1] == ':')
    {
        ctx->app->curr_path[len] = '/';
        ctx->app->curr_path[len + 1] = '\0';
        TEXT_SetText(ctx->header, ctx->app->curr_path);
        ctx->app->curr_path[len] = '\0';
    }
    else
    {
        TEXT_SetText(ctx->header, ctx->app->curr_path);
    }

    widget_list_clear(ctx->list);
    widget_list_set_scroll(ctx->list, 0);

    if (strcmp(ctx->app->curr_path, APP_ROOT_DIR) == 0)
    {
        ctx->is_at_root = true;
        widget_icon_set_icon(ctx->action, &icon_card);
    }
    else
    {
        ctx->is_at_root = false;
        widget_icon_set_icon(ctx->action, &icon_folder_up);
    }

    DIR dir;
    FRESULT res = f_opendir(&dir, ctx->app->curr_path);
    ASSERT_WARN(res == FR_OK);

    DBG_PRINTF("curr_path: %s\n", ctx->app->curr_path);

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

static void calculate_parent_dir(void)
{
    if (ctx->is_at_root)
        return;

    int len = strlen(ctx->app->curr_path);
    char *c = ctx->app->curr_path + (len - 2);
    do
    {
        if (*c == '/')
        {
            *c = '\0';
            break;
        }
    }
    while (c-- != ctx->app->curr_path);
}

//----------------------------------------------

static void move_to_parent_dir(void)
{
    calculate_parent_dir();

    list_curr_dir();
}

//----------------------------------------------

static void list_item_selected(int selected)
{
    const char *item = widget_list_get_item(ctx->list, selected);

    DBG_PRINTF("selected: %s\n", item);

    strcat(ctx->app->curr_path, "/");
    strcat(ctx->app->curr_path, item);

    FILINFO info;
    FRESULT res = f_stat(ctx->app->curr_path, &info);
    ASSERT_WARN(res == FR_OK);

    if (info.fattrib & AM_DIR)
    {
        list_curr_dir();
        return;
    }

    strcpy(ctx->app->path_buffer, ctx->app->curr_path);
    calculate_parent_dir();

    gui_show_screen(&screen_player);
}

//----------------------------------------------

static void handle_event(const struct gui_event *event)
{
    switch (event->id)
    {
    case WIDGET_LIST_EVENT_SELECTED:
        if (event->sender == ctx->list)
            list_item_selected((int)event->data);
        break;

    case WIDGET_EVENT_PRESSED:
        if (event->sender == ctx->action)
            move_to_parent_dir();
        break;

    default:
        break;
    }
}

//----------------------------------------------

static void process(void)
{
}

//----------------------------------------------

struct gui_screen screen_file_list = SIMPLE_SCREEN();
