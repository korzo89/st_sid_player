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
#include <string.h>

//----------------------------------------------

#define EVENT_QUEUE_LEN     10

//----------------------------------------------

struct gui_ctx
{
    const struct gui_screen *curr_screen;
    const struct gui_screen *next_screen;
    GUI_HWIN curr_wnd;

    struct gui_event events[EVENT_QUEUE_LEN];
    size_t num_events;
};

static struct gui_ctx ctx;

//----------------------------------------------

void gui_init(void)
{
    memset(&ctx, 0, sizeof(ctx));

    touch_init();

    GUI_Init();

    GUI_SetOrientation(GUI_MIRROR_X | GUI_MIRROR_Y);

    WM_MULTIBUF_Enable(1);

    WM_SelectWindow(WM_HBKWIN);
    WM_SetDesktopColor(GUI_BLACK);

#if 0
    __disable_irq();

    GUI_MEMDEV_Handle memdev = GUI_MEMDEV_Create(0, 0, 100, 100);
    GUI_MEMDEV_Select(memdev);
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetColor(GUI_GREEN);
    GUI_FillCircle(50, 50, 40);
    GUI_MEMDEV_CopyToLCD(memdev);
    asm volatile("nop");

    __enable_irq();
#endif
}

//----------------------------------------------

static void destroy_curr_screen(void)
{
    if (ctx.curr_screen->destroy)
        ctx.curr_screen->destroy();

    WM_DeleteWindow(ctx.curr_wnd);
}

//----------------------------------------------

static void show_next_screen(void)
{
    if (ctx.curr_screen)
        destroy_curr_screen();

    const struct gui_screen *screen = ctx.next_screen;
    if (!screen)
        return;

    ctx.curr_wnd = WINDOW_CreateEx(
            0, 0, LCD_WIDTH, LCD_HEIGHT,
            WM_GetBackgroundWindow(), WM_CF_SHOW,
            0, 0, NULL);

    if (screen->create)
        screen->create(ctx.curr_wnd);

    ctx.curr_screen = screen;
    ctx.next_screen = NULL;
}

//----------------------------------------------

static void process_events(void)
{
    if (!ctx.curr_screen || !ctx.curr_screen->handle_event)
        goto finish;

    size_t i;
    for (i = 0; i < ctx.num_events; i++)
        ctx.curr_screen->handle_event(&ctx.events[i]);

finish:
    ctx.num_events = 0;
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

    process_events();
}

//----------------------------------------------

void gui_show_screen(const struct gui_screen *screen)
{
    ctx.next_screen = screen;
}

//----------------------------------------------

bool gui_send_event(WM_HWIN sender, uint32_t id, void *data)
{
    if (ctx.num_events == EVENT_QUEUE_LEN)
        return false;

    struct gui_event *event = &ctx.events[ctx.num_events];
    event->sender = sender;
    event->id = id;
    event->data = data;

    ctx.num_events++;

    return true;
}
