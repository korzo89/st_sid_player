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

//----------------------------------------------

WM_HWIN widget_list_create(const GUI_WIDGET_CREATE_INFO *info, WM_HWIN parent, int x, int y, WM_CALLBACK *cb);

//----------------------------------------------

#endif /* GUI_WIDGETS_WIDGET_LIST_H_ */
