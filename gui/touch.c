/*
 * touch.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "touch.h"
#include <assert.h>
#include <GUI.h>

//----------------------------------------------

void touch_init(void)
{
    uint8_t res = BSP_TS_Init(LCD_WIDTH, LCD_HEIGHT);
    ASSERT_CRIT(res == TS_OK);
}

void touch_process(void)
{
    TS_StateTypeDef ts_state;
    uint8_t res = BSP_TS_GetState(&ts_state);
    ASSERT_WARN(res == TS_OK);

    GUI_PID_STATE pid_state = {
        .x       = LCD_WIDTH - ts_state.touchX[0],
        .y       = LCD_HEIGHT - ts_state.touchY[0],
        .Pressed = !!ts_state.touchDetected,
        .Layer   = 0
    };

    GUI_PID_StoreState(&pid_state);
}
