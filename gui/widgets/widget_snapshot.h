/*
 * widget_snapshot.h
 *
 *  Created on: 25 lip 2016
 *      Author: Korzo
 */

#ifndef GUI_WIDGETS_WIDGET_SNAPSHOT_H_
#define GUI_WIDGETS_WIDGET_SNAPSHOT_H_

#include <gui/widget.h>

//----------------------------------------------

WM_HWIN widget_snapshot_create(int x, int y, int w, int h, WM_HWIN parent, int flags);

void widget_snapshot_capture(WM_HWIN handle, int x0, int y0);

//----------------------------------------------

#endif /* GUI_WIDGETS_WIDGET_SNAPSHOT_H_ */
