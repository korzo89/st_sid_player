/*
 * widget_list.h
 *
 *  Created on: 18 lip 2016
 *      Author: Korzo
 */

#ifndef GUI_WIDGETS_WIDGET_LIST_H_
#define GUI_WIDGETS_WIDGET_LIST_H_

#include <gui/widget.h>

//----------------------------------------------

enum widget_list_event
{
    WIDGET_LIST_EVENT_SELECTED = WIDGET_EVENT_CUSTOM,
};

//----------------------------------------------

WM_HWIN widget_list_create(int x, int y, int w, int h, WM_HWIN parent, int flags);

bool widget_list_add_item(WM_HWIN handle, const char *text, const GUI_BITMAP *icon);

const char* widget_list_get_item(WM_HWIN handle, int index);

void widget_list_clear(WM_HWIN handle);

void widget_list_set_font(WM_HWIN handle, const GUI_FONT *font);

void widget_list_set_bg_color(WM_HWIN handle, GUI_COLOR color);

void widget_list_set_text_color(WM_HWIN handle, GUI_COLOR color);

void widget_list_set_icon_color(WM_HWIN handle, GUI_COLOR color);

void widget_list_set_item_height(WM_HWIN handle, int val);

void widget_list_set_scroll(WM_HWIN handle, int val);

int widget_list_get_scroll(WM_HWIN handle);

//----------------------------------------------

#endif /* GUI_WIDGETS_WIDGET_LIST_H_ */
