/*
 * widget.h
 *
 *  Created on: 25 lip 2016
 *      Author: Korzo
 */

#ifndef GUI_WIDGETS_WIDGET_H_
#define GUI_WIDGETS_WIDGET_H_

#include <WM.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

//----------------------------------------------

#define WIDGET_VISUAL_PROPERTY_SETTER(_name, _type, _prop)      \
    void _name(WM_HWIN handle, _type val)                       \
    {                                                           \
        if (!handle) return;                                    \
        struct widget_ctx *ctx = widget_get_context(handle);    \
        if (ctx->_prop == val) return;                          \
        ctx->_prop = val;                                       \
        WM_InvalidateWindow(handle);                            \
    }

#define WIDGET_CREATE_CONTEXT(_handle)  \
        widget_create_context((_handle), sizeof(struct widget_ctx))

typedef void (*widget_callback_t)(WM_MESSAGE *msg);

//----------------------------------------------

WM_HWIN widget_create(int x, int y, int w, int h, WM_HWIN parent, int flags, widget_callback_t cb);

void* widget_create_context(WM_HWIN handle, size_t size);

void widget_destroy_context(WM_HWIN handle);

void* widget_get_context(WM_HWIN handle);

//----------------------------------------------

#endif /* GUI_WIDGETS_WIDGET_H_ */
