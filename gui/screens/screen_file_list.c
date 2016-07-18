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

//----------------------------------------------

#define ROOT_DIR    ""

struct wnd_ctx
{
    GUI_HWIN list;
    GUI_HWIN open;

    char buffer[_MAX_LFN + 1];
    char curr_path[_MAX_LFN + 1];
};

static struct wnd_ctx *ctx;

static void list_curr_dir(void);

//----------------------------------------------

#define LIST_ID     WID(1)
#define OPEN_ID     WID(2)

static const GUI_WIDGET_CREATE_INFO resources[] = {
    { WINDOW_CreateIndirect, "Window", 0, 0, 0, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0 },
    { LISTBOX_CreateIndirect, "Files", LIST_ID, 10, 10, 300, LCD_HEIGHT - 20, 0 },
    { BUTTON_CreateIndirect, "OPEN", OPEN_ID, LCD_WIDTH - 110, LCD_HEIGHT - 50, 100, 40 },
//    { widget_list_create, "", 0, 80, 10, 300, LCD_HEIGHT - 20, 0 },
};

//----------------------------------------------

static void create(GUI_HWIN wnd)
{
    GUI_ALLOC_CTX(ctx);

    ctx->list = WM_GetDialogItem(wnd, LIST_ID);
    ctx->open = WM_GetDialogItem(wnd, OPEN_ID);

    LISTBOX_SetFont(ctx->list, &GUI_Font32_ASCII);
    LISTBOX_SetAutoScrollV(ctx->list, 1);

    strcpy(ctx->curr_path, ROOT_DIR);

    list_curr_dir();
}

//----------------------------------------------

static void destroy(void)
{
    GUI_FREE_CTX(ctx);
}

//----------------------------------------------

static void clear_list(void)
{
    int num = LISTBOX_GetNumItems(ctx->list);
    while (num--)
        LISTBOX_DeleteItem(ctx->list, 0);
}

//----------------------------------------------

static void list_curr_dir(void)
{
    clear_list();

    if (strcmp(ctx->curr_path, ROOT_DIR) != 0)
        LISTBOX_AddString(ctx->list, "..");

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

        LISTBOX_AddString(ctx->list, info.fname);
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

static void open_selected_item(void)
{
    int selected = LISTBOX_GetSel(ctx->list);

    LISTBOX_GetItemText(ctx->list, selected, ctx->buffer, sizeof(ctx->buffer));

    DBG_PRINTF("selected: %s\n", ctx->buffer);

    bool is_dir;
    if (strcmp(ctx->buffer, "..") == 0)
    {
        move_to_parent_dir();
        is_dir = true;
    }
    else
    {
        strcat(ctx->curr_path, ctx->buffer);

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

static void handle_event(GUI_HWIN sender, int event)
{
    if ((sender == ctx->open) && (event == WM_NOTIFICATION_RELEASED))
        open_selected_item();
}

//----------------------------------------------

static void process(void)
{
}

//----------------------------------------------

struct gui_screen screen_file_list = SIMPLE_SCREEN();
