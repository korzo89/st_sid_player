/*
 * gui.h
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#ifndef GUI_GUI_H_
#define GUI_GUI_H_

#include <config.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "STemWin/inc/GUI.h"
#include <WM.h>
#include <DIALOG.h>

//----------------------------------------------

struct gui_event
{
    WM_HWIN sender;
    uint32_t id;
    void *data;
};

struct gui_screen
{
    void (*create)(GUI_HWIN wnd);
    void (*destroy)(void);
    void (*handle_event)(const struct gui_event *event);
    void (*process)(void);
};

#define DEF_SCREEN(_create, _destroy, _event, _proc)    \
    {                                                   \
        .create = (_create),                            \
        .destroy = (_destroy),                          \
        .handle_event = (_event),                       \
        .process = (_proc)                              \
    }

#define SIMPLE_SCREEN()     DEF_SCREEN(create, destroy, handle_event, process)

#define GUI_ALLOC_CTX(_ptr)                 \
    do {                                    \
        malloc(sizeof(*(_ptr)));            \
        memset((_ptr), 0, sizeof(*(_ptr))); \
    } while(0)

#define GUI_FREE_CTX(_ptr)  \
    do {                    \
        free(_ptr);         \
        (_ptr) = NULL;      \
    } while(0)

#define WID(_x)             (GUI_ID_USER + (_x))

#define GUI_RGBH(_x)        ((((_x) & 0xFF0000) >> 16) | ((_x) & 0xFF00) | (((_x) & 0xFF) << 16))

//----------------------------------------------

void gui_init(void);

void gui_process(void);

void gui_show_screen(const struct gui_screen *screen);

bool gui_send_event(WM_HWIN sender, uint32_t id, void *data);

//----------------------------------------------

#endif /* GUI_GUI_H_ */
