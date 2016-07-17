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

void sid_player_init(void);

bool sid_player_play(const char *path);

void sid_player_process(void);

const struct sid_info* sid_player_get_info(void);

//----------------------------------------------

#endif /* SID_SID_PLAYER_H_ */
