/*
 * sid_player.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "sid_player.h"
#include <debug.h>
#include <ff.h>
#include <stm32746g_discovery_audio.h>
#include <protothreads/pt.h>

//----------------------------------------------

#define SAMPLE_RATE         AUDIO_FREQUENCY_48K
#define CHUNK_SIZE          (10 * 1024)
#define FILE_BUFFER_SIZE    (10 * 1024)

enum audio_state
{
    AUDIO_STATE_BUSY,
    AUDIO_STATE_HALF_COMPLETE,
    AUDIO_STATE_COMPLETE
};

struct audio_sample
{
    uint16_t left;
    uint16_t right;
} ATTRIBUTE_PACKED;

struct sid_ctx
{
    enum audio_state state;

    struct sid_info sid;

    uint16_t samples[CHUNK_SIZE];
    struct audio_sample audio_buffer[CHUNK_SIZE];

    uint8_t file_buffer[FILE_BUFFER_SIZE];

    struct pt thread;
};

struct sid_ctx ctx;

static void generate_samples(size_t offset, size_t length);

//----------------------------------------------

void sid_player_init(void)
{
    PT_INIT(&ctx.thread);
}

//----------------------------------------------

const struct sid_info* sid_player_get_info(void)
{
    return &ctx.sid;
}

//----------------------------------------------

bool sid_player_play(const char *path)
{
    FILINFO file_info;
    FRESULT res = f_stat(path, &file_info);
    if (res != FR_OK)
        return false;

    if (file_info.fsize > FILE_BUFFER_SIZE)
        return false;

    FIL file;
    res = f_open(&file, path, FA_READ);
    if (res != FR_OK)
        return false;

    UINT read_size;
    res = f_read(&file, ctx.file_buffer, file_info.fsize, &read_size);
    if (read_size != file_info.fsize)
        return false;

    f_close(&file);

    c64Init(SAMPLE_RATE);

    sid_load_from_memory(ctx.file_buffer, file_info.fsize, &ctx.sid);

    sidPoke(24, 15);
    cpuJSR(ctx.sid.init_addr, 0);

    generate_samples(0, CHUNK_SIZE);

    uint8_t audio_res = BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 100, SAMPLE_RATE);
    if (audio_res != AUDIO_OK)
        return false;

    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

    audio_res = BSP_AUDIO_OUT_Play((uint16_t*)ctx.audio_buffer, sizeof(ctx.audio_buffer));
    if (audio_res != AUDIO_OK)
        return false;

    return true;
}

//----------------------------------------------

static void generate_samples(size_t offset, size_t length)
{
    DBG_PRINTF("generate_samples, offset %u length %u\n", offset, length);

    int samples_rendered = 0;
    int samples_to_render = 0;

    while (samples_rendered < length)
    {
        if (samples_to_render == 0)
        {
            cpuJSR(ctx.sid.play_addr, 0);

            /* Find out if cia timing is used and how many samples
             have to be calculated for each cpujsr */
            int n_refresh_cia = (int)(20000 * (memory[0xdc04] | (memory[0xdc05] << 8)) / 0x4c00);
            if ((n_refresh_cia == 0) || (ctx.sid.speed == 0))
                n_refresh_cia = 20000;

            samples_to_render = SAMPLE_RATE * n_refresh_cia / 1000000;
        }
        if (samples_rendered + samples_to_render > CHUNK_SIZE)
        {
            sid_synth_render(ctx.samples + samples_rendered, CHUNK_SIZE - samples_rendered);
            samples_to_render -= CHUNK_SIZE - samples_rendered;
            samples_rendered = CHUNK_SIZE;
        }
        else
        {
            sid_synth_render(ctx.samples + samples_rendered, samples_to_render);
            samples_rendered += samples_to_render;
            samples_to_render = 0;
        }
    }

    size_t i, j;
    for (i = 0, j = offset; i < length; i++, j++)
    {
        ctx.audio_buffer[j].left = ctx.samples[i];
        ctx.audio_buffer[j].right = ctx.samples[i];
    }
}

//----------------------------------------------

static PT_THREAD(audio_thread(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1)
    {
        ctx.state = AUDIO_STATE_BUSY;
        PT_WAIT_UNTIL(pt, ctx.state == AUDIO_STATE_HALF_COMPLETE);

        generate_samples(0, CHUNK_SIZE / 2);
        PT_WAIT_UNTIL(pt, ctx.state == AUDIO_STATE_COMPLETE);

        generate_samples(CHUNK_SIZE / 2, CHUNK_SIZE / 2);
    }

    PT_END(pt);
}

//----------------------------------------------

void sid_player_process(void)
{
    audio_thread(&ctx.thread);
}

//----------------------------------------------

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
    if (ctx.state == AUDIO_STATE_HALF_COMPLETE)
        ctx.state = AUDIO_STATE_COMPLETE;
}

//----------------------------------------------

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
    if (ctx.state == AUDIO_STATE_BUSY)
        ctx.state = AUDIO_STATE_HALF_COMPLETE;
}
