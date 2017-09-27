/*
 * sid_player.h
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#ifndef SID_SID_PLAYER_H_
#define SID_SID_PLAYER_H_

#include <stdbool.h>
#include "sid.h"

//----------------------------------------------

struct audio_sample
{
    uint16_t left;
    uint16_t right;
} ATTRIBUTE_PACKED;

//----------------------------------------------

void sid_player_init(void);

bool sid_player_play(const char *path);

void sid_player_stop(void);

void sid_player_process(void);

const struct sid_info* sid_player_get_info(void);

bool sid_player_is_buffer_ready(void);

uint32_t sid_player_get_buffer_size(void);

const struct audio_sample* sid_player_get_buffer(void);

//----------------------------------------------

#endif /* SID_SID_PLAYER_H_ */
