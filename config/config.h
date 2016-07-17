/*
 * config.h
 *
 *  Created on: 20 cze 2016
 *      Author: Korzo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stm32f7xx_hal.h>
#include <stm32746g_discovery_audio.h>
#include <stm32746g_discovery_sd.h>
#include <stm32746g_discovery_sdram.h>
#include <stm32746g_discovery_lcd.h>
#include <stm32746g_discovery_ts.h>

//----------------------------------------------

#define LCD_WIDTH           RK043FN48H_WIDTH
#define LCD_HEIGHT          RK043FN48H_HEIGHT
#define LCD_FRAME_BUFFER    SDRAM_DEVICE_ADDR

#ifndef UNUSED
#define UNUSED(_x)          ((void)(_x))
#endif

#define ATTRIBUTE_PACKED    __attribute__((packed))

#define ARRAY_SIZE(_x)      (sizeof(_x) / sizeof((_x)[0]))

//----------------------------------------------

#endif /* CONFIG_H_ */
