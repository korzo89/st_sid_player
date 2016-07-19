/*
 * widget_list.h
 *
 *  Created on: 18 lip 2016
 *      Author: Korzo
 */

#ifndef GUI_WIDGETS_WIDGET_LIST_H_
#define GUI_WIDGETS_WIDGET_LIST_H_

#include <WM.h>
#include <DIALOG.h>
#include <stdbool.h>
#include <stdint.h>

//----------------------------------------------

enum widget_list_event
{
    WIDGET_LIST_EVENT_SELECTED,
};

//----------------------------------------------

WM_HWIN widget_list_create(const GUI_WIDGET_CREATE_INFO *info, WM_HWIN parent, int x, int y, WM_CALLBACK *cb);

bool widget_list_add_item(WM_HWIN handle, const char *text, const GUI_BITMAP *icon);

const char* widget_list_get_item(WM_HWIN handle, int index);

void widget_list_clear(WM_HWIN handle);

void widget_list_set_font(WM_HWIN handle, const GUI_FONT *font);

void widget_list_set_bg_color(WM_HWIN handle, GUI_COLOR color);

void widget_list_set_color(WM_HWIN handle, GUI_COLOR color);

void widget_list_set_scroll(WM_HWIN handle, int val);

//----------------------------------------------

#endif /* GUI_WIDGETS_WIDGET_LIST_H_ */
