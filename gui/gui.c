/*
 * gui.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "gui.h"
#include "touch.h"
#include "STemWin/inc/GUI.h"
#include <assert.h>

//----------------------------------------------

struct gui_ctx
{
    const struct gui_screen *curr_screen;
    const struct gui_screen *next_screen;
    GUI_HWIN curr_wnd;
};

static struct gui_ctx ctx;

//----------------------------------------------

void gui_init(void)
{
    touch_init();
    GUI_Init();
}

//----------------------------------------------

static void process_notify_parent(const WM_MESSAGE *msg)
{
    if (!ctx.curr_screen || !ctx.curr_screen->handle_event)
        return;

    ctx.curr_screen->handle_event(msg->hWinSrc, msg->Data.v);
}

//----------------------------------------------

static void wnd_callback(WM_MESSAGE *msg)
{
    switch (msg->MsgId)
    {
    case WM_NOTIFY_PARENT:
        process_notify_parent(msg);
        break;

    default:
        WM_DefaultProc(msg);
        break;
    }
}

//----------------------------------------------

static void destroy_curr_screen(void)
{
    if (ctx.curr_screen->destroy)
        ctx.curr_screen->destroy();

    GUI_EndDialog(ctx.curr_wnd, 0);
}

//----------------------------------------------

static void show_next_screen(void)
{
    if (ctx.curr_screen)
        destroy_curr_screen();

    const struct gui_screen *screen = ctx.next_screen;
    if (!screen)
        return;

    ASSERT_WARN(screen->resources != NULL);

    ctx.curr_wnd = GUI_CreateDialogBox(
            screen->resources, screen->resources_len,
            wnd_callback, WM_HBKWIN, 0, 0);

    if (screen->create)
        screen->create(ctx.curr_wnd);

    ctx.curr_screen = screen;
    ctx.next_screen = NULL;
}

//----------------------------------------------

void gui_process(void)
{
    if (ctx.next_screen)
        show_next_screen();

    if (ctx.curr_screen && ctx.curr_screen->process)
        ctx.curr_screen->process();

    touch_process();
    GUI_Exec();
}

//----------------------------------------------

void gui_show_screen(const struct gui_screen *screen)
{
    ctx.next_screen = screen;
}
