/*
 * widget_icon.h
 *
 *  Created on: 28 lip 2016
 *      Author: Korzo
 */

#ifndef GUI_WIDGETS_WIDGET_ICON_H_
#define GUI_WIDGETS_WIDGET_ICON_H_

#include <gui/widget.h>

//----------------------------------------------

WM_HWIN widget_icon_create(int x, int y, int w, int h, WM_HWIN parent, int flags);

void widget_icon_set_icon(WM_HWIN handle, const GUI_BITMAP *icon);

void widget_icon_set_alignment(WM_HWIN handle, int alignment);

void widget_icon_set_offset(WM_HWIN handle, int x, int y);

void widget_icon_set_color(WM_HWIN handle, GUI_COLOR color);

void widget_icon_set_bg_color(WM_HWIN handle, GUI_COLOR color);

//----------------------------------------------

#endif /* GUI_WIDGETS_WIDGET_ICON_H_ */
