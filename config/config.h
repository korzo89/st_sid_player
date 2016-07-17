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

//----------------------------------------------

#define LCD_FRAME_BUFFER    SDRAM_DEVICE_ADDR

#define ATTRIBUTE_PACKED    __attribute__((packed))

//----------------------------------------------

#endif /* CONFIG_H_ */
