/*
 * sid.h
 *
 *  Created on: 14 lip 2016
 *      Author: Korzo
 */

#ifndef SID_SID_H_
#define SID_SID_H_

void synth_init(unsigned long mixfrq);

void synth_render (int32_t *buffer, unsigned long len);

void sidPoke(int reg, unsigned char val);

void cpuReset(void);

void cpuResetTo(unsigned short npc, unsigned char na);

void cpuJSR(unsigned short npc, unsigned char na);

void c64Init(int nSampleRate);

unsigned short LoadSIDFromMemory(void *pSidData, unsigned short *load_addr,
                       unsigned short *init_addr, unsigned short *play_addr, unsigned char *subsongs, unsigned char *startsong, unsigned char *speed, unsigned short size);

extern unsigned char memory[];

#endif /* SID_SID_H_ */
