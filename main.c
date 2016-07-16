#include <config.h>
#include <RTT/SEGGER_RTT.h>
#include <stm32f7xx_hal.h>
#include <stm32746g_discovery.h>
#include <stm32746g_discovery_lcd.h>
#include <stm32746g_discovery_ts.h>
#include <fatfs.h>
#include <ff.h>
#include <debug.h>
#include <stdlib.h>
#include <sid/sid.h>
#include <protothreads/pt.h>
#include <string.h>
#include <stdbool.h>

//----------------------------------------------

#define SAMPLE_RATE         AUDIO_FREQUENCY_48K
#define CHUNK_SIZE          (10 * 1024)
#define AUDIO_BUFFER_SIZE   (2 * CHUNK_SIZE)

static void SystemClock_Config(void);

enum audio_state
{
    AUDIO_STATE_BUSY,
    AUDIO_STATE_HALF_COMPLETE,
    AUDIO_STATE_COMPLETE
};

static enum audio_state state = AUDIO_STATE_BUSY;

static unsigned short load_addr, init_addr, play_addr;
static unsigned char sub_songs_max, sub_song, song_speed;

static int nSamplesRendered = 0;
static int nSamplesPerCall = 882;  /* This is PAL SID single speed (44100/50Hz) */
static int nSamplesToRender = 0;

static int32_t samples[CHUNK_SIZE];

static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

//----------------------------------------------

static void raise_error(const char *message)
{
    DBG_PRINTF("ERROR: %s\n", message);
    asm volatile("bkpt 1");
    while (1)
    {
    }
}

//----------------------------------------------

static inline uint16_t map_sample(int32_t x)
{
    return (x + 32768) >> 4;
}

static void generate_samples(size_t offset, size_t length)
{
    DBG_PRINTF("generate_samples, offset %u length %u\n", offset, length);
    nSamplesRendered = 0;
    while (nSamplesRendered < length)
    {
        if (nSamplesToRender == 0)
        {
            cpuJSR(play_addr, 0);

            /* Find out if cia timing is used and how many samples
               have to be calculated for each cpujsr */
            int nRefreshCIA = (int)(20000*(memory[0xdc04]|(memory[0xdc05]<<8))/0x4c00);
            if ((nRefreshCIA==0) || (song_speed == 0))
                nRefreshCIA = 20000;
            nSamplesPerCall = SAMPLE_RATE*nRefreshCIA/1000000;

            nSamplesToRender = nSamplesPerCall;
        }
        if (nSamplesRendered + nSamplesToRender > CHUNK_SIZE)
        {
            synth_render(samples+nSamplesRendered, CHUNK_SIZE-nSamplesRendered);
            nSamplesToRender -= CHUNK_SIZE-nSamplesRendered;
            nSamplesRendered = CHUNK_SIZE;
        }
        else
        {
            synth_render(samples+nSamplesRendered, nSamplesToRender);
            nSamplesRendered += nSamplesToRender;
            nSamplesToRender = 0;
        }
    }

    uint32_t i, j;
    for (i = 0, j = offset * 2; i < length; i += 1, j += 2)
    {
        audio_buffer[j] = map_sample(samples[i]);
        audio_buffer[j + 1] = audio_buffer[j];
    }
}

//----------------------------------------------

static PT_THREAD(test_thread(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1)
    {
        state = AUDIO_STATE_BUSY;
        PT_WAIT_UNTIL(pt, state == AUDIO_STATE_HALF_COMPLETE);
        generate_samples(0, CHUNK_SIZE / 2);
        PT_WAIT_UNTIL(pt, state == AUDIO_STATE_COMPLETE);
        generate_samples(CHUNK_SIZE / 2, CHUNK_SIZE / 2);
    }

    PT_END(pt);
}

//----------------------------------------------

static void general_task(void *p)
{
    BSP_LED_Init(LED1);

    /* Configure the User Button in GPIO Mode */
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);

    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

    /* Clear the LCD */
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    /* Set the LCD Text Color */
    BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

    BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

    MX_FATFS_Init();

    FATFS fs;
    FRESULT res;

    res = f_mount(&fs, SD_Path, 0);
    if (res != FR_OK)
        raise_error("f_mount failed\n");

#if 1
    char curr_path[_MAX_LFN + 1] = "";

    #define MAX_ENTRIES 11
    static char entries[MAX_ENTRIES][13];
    size_t num_entries = 0;

    bool has_file = false;
    while (!has_file)
    {
        DBG_PRINTF("curr_path: %s/\n", curr_path);

        DIR dir;
        res = f_opendir(&dir, curr_path);
        if (res != FR_OK)
            raise_error("f_opendir failed\n");

        strcat(curr_path, "/");

        BSP_LCD_Clear(LCD_COLOR_WHITE);

        uint16_t line = 0;
        for (num_entries = 0; num_entries < MAX_ENTRIES; num_entries++)
        {
            FILINFO info;
            if (f_readdir(&dir, &info) != FR_OK)
                raise_error("f_readdir failed\n");

            if (info.fname[0] == '\0')
                break;

            strcpy(entries[num_entries], info.fname);

            BSP_LCD_DisplayStringAtLine(line++, (uint8_t*)info.fname);
        }

        while (1)
        {
            TS_StateTypeDef ts;
            BSP_TS_GetState(&ts);
            if (ts.touchDetected)
            {
                uint16_t ts_line = ts.touchY[0] / BSP_LCD_GetFont()->Height;
                if (ts_line < num_entries)
                {
                    while (ts.touchDetected)
                        BSP_TS_GetState(&ts);

                    strcat(curr_path, entries[ts_line]);

                    FILINFO info;
                    if (f_stat(curr_path, &info) != FR_OK)
                        raise_error("f_stat failed\n");

                    if (!(info.fattrib & AM_DIR))
                        has_file = true;

                    f_closedir(&dir);
                    break;
                }
            }
        }
    }
#endif

//    const char *file_path = "/MUSICI~1/B/BRANDIS/FADE_T~1.SID";
//    const char *file_path = "/MUSICI~1/B/BRANDIS/AXEL_F~1.SID";

    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAtLine(0, (uint8_t*)curr_path);

    FILINFO file_info;
    res = f_stat(curr_path, &file_info);
    if (res != FR_OK)
        raise_error("f_stat failed\n");

    DBG_PRINTF("size %d\n", file_info.fsize);

    FIL file;
    res = f_open(&file, curr_path, FA_READ);
    if (res != FR_OK)
        raise_error("f_open failed\n");

    uint8_t *buffer = malloc(file_info.fsize);
    if (res != FR_OK)
        raise_error("malloc failed\n");

    UINT read_size;
    res = f_read(&file, buffer, file_info.fsize, &read_size);
    if (read_size != file_info.fsize)
        raise_error("read_size != file_info.fsize\n");

    f_close(&file);

    c64Init(SAMPLE_RATE);
    LoadSIDFromMemory(buffer, &load_addr, &init_addr, &play_addr,
            &sub_songs_max, &sub_song, &song_speed, file_info.fsize);
    sidPoke(24, 15);
    cpuJSR(init_addr, 0);

    generate_samples(0, CHUNK_SIZE);

    if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 100, SAMPLE_RATE) != AUDIO_OK)
    {
        SEGGER_RTT_printf(0, "BSP_AUDIO_OUT_Init failed");
        while(1) {};
    }

    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

    BSP_AUDIO_OUT_Play(audio_buffer, sizeof(audio_buffer));

    static struct pt child_pt;
    PT_INIT(&child_pt);

    while (1)
    {
        test_thread(&child_pt);
    }
}

//----------------------------------------------

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
    if (state == AUDIO_STATE_HALF_COMPLETE)
        state = AUDIO_STATE_COMPLETE;
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
    if (state == AUDIO_STATE_BUSY)
        state = AUDIO_STATE_HALF_COMPLETE;
}

//----------------------------------------------

int main(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    SEGGER_RTT_Init();

    HAL_Init();

    /* Configure the system clock to 200 Mhz */
    SystemClock_Config();

    general_task(NULL);

    while (1)
    {
    }
}

//----------------------------------------------

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        raise_error("HAL_RCC_OscConfig failed\n");
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        raise_error("HAL_PWREx_EnableOverDrive failed\n");
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        raise_error("HAL_RCC_ClockConfig failed\n");
      }

      PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI2;
      PeriphClkInitStruct.PLLSAI.PLLSAIN = 100;
      PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
      PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
      PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
      PeriphClkInitStruct.PLLSAIDivQ = 1;
      PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
      PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
      {
          raise_error("HAL_RCCEx_PeriphCLKConfig failed\n");
      }

      HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

      HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

      /* SysTick_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
