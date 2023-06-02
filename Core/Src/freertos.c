/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "bsp_key.h"
#include "u8g2.h"
#include "stm32_u8g2.h"
#include "usbd_customhid.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SPD_H_POS             0
#define HDG_H_POS             52

#define ALT_H_POS             0
#define VS_H_POS              72

#define USE_USB_PAGE          1

#define KEY_DEBUG_PRINTF(...) printf(__VA_ARGS__)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint32_t usb_send_buf;
extern panel_state_t fcu_state;
extern uint8_t fcu_update;
extern USBD_HandleTypeDef hUsbDeviceFS;
static uint8_t usb_inited = 0;

static inline int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
/* Definitions for printfMutex */
osMutexId_t printfMutexHandle;
const osMutexAttr_t printfMutex_attributes = {
    .name = "printfMutex"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t displayTaskHandle;
const osThreadAttr_t displayTask_attributes = {
    .name       = "displayTask",
    .stack_size = 512 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
osThreadId_t keyScanTaskHandle;
const osThreadAttr_t keyScanTask_attributes = {
    .name       = "keyScanTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
osThreadId_t autopilotBeepTaskHandle;
const osThreadAttr_t autopilotBeepTask_attributes = {
    .name       = "autopilotBeepTask",
    .stack_size = 128 * 1,
    .priority   = (osPriority_t)osPriorityLow,
};
osThreadId_t usbTransmitTaskHandle;
const osThreadAttr_t usbTransmitTask_attributes = {
    .name       = "usbTransmitTask",
    .stack_size = 128 * 1,
    .priority   = (osPriority_t)osPriorityRealtime,
};
void StartDisplayTask(void *argument);
void StartKeyScanTask(void *argument);
void StartAutopilotBeepTask(void *argument);
void StartUSBTransmitTask(void *argument);

void thread_safe_printf(char *format, ...);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */
    /* Create the mutex(es) */
    /* creation of printfMutex */
    printfMutexHandle = osMutexNew(&printfMutex_attributes);

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    displayTaskHandle       = osThreadNew(StartDisplayTask, NULL, &displayTask_attributes);
    keyScanTaskHandle       = osThreadNew(StartKeyScanTask, NULL, &keyScanTask_attributes);
    autopilotBeepTaskHandle = osThreadNew(StartAutopilotBeepTask, NULL, &autopilotBeepTask_attributes);
    usbTransmitTaskHandle   = osThreadNew(StartUSBTransmitTask, NULL, &usbTransmitTask_attributes);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* init code for USB_DEVICE */
    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN StartDefaultTask */
    *((__IO unsigned *)((0x40005C00L) + 0x54)) = 1; // DP PULL UP
    bsp_key_init();
    usb_inited = 1;

    /* hack for overclock */

    /* Infinite loop */
    for (;;) {
        matrix_key_scan();
        osDelay(10); // 10ms
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static inline void panel_led_update(panel_state_t *panel)
{
    HAL_GPIO_WritePin(AP1_LED_GPIO_Port, AP1_LED_Pin, panel->ap1_active);
    HAL_GPIO_WritePin(AP2_LED_GPIO_Port, AP2_LED_Pin, panel->ap2_active);
    HAL_GPIO_WritePin(ATHR_LED_GPIO_Port, ATHR_LED_Pin, panel->ap_throttle_active);
    HAL_GPIO_WritePin(LOC_LED_GPIO_Port, LOC_LED_Pin, panel->loc_mode_active);
    HAL_GPIO_WritePin(EXPED_LED_GPIO_Port, EXPED_LED_Pin, panel->exped_mode_active);
    HAL_GPIO_WritePin(APPR_LED_GPIO_Port, APPR_LED_Pin, panel->appr_mode_active);
}
// 无返回值的函数
void float2Str(char *str, float value)
{
    static char sign;
    if (value >= 0) {
        sign = '+';
    } else {
        sign  = '-';
        value = -value;
    }
    int integer       = (int)value;                                                                              // 将浮点数转换为整数
    float decimalPart = value - (float)integer;                                                                  // 提取小数部分
    int decimalDigits = (decimalPart * 10 >= 0) ? (int)(decimalPart * 10 + 0.5) : (int)(decimalPart * 10 - 0.5); // 提取小数位数
    sprintf(str, "%c%d.%d\n", sign, integer, decimalDigits);                                                     // 打印结果
}
void StartDisplayTask(void *argument)
{
    char display_str_buf[6];
    extern u8g2_t screen_left;
    extern u8g2_t screen_right;
    extern const uint8_t digital7_18[1160] U8G2_FONT_SECTION("digital7_18");
    extern const uint8_t digital7_24[1690] U8G2_FONT_SECTION("digital7_24");
    /* Infinite loop */
    for (;;) {
#if USE_USB_PAGE
        if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) { // USB 已连接
            if (fcu_update == 0) continue;
            fcu_update = 0;
#endif
            /* ----------------------- LED刷新 ------------------------- */
            panel_led_update(&fcu_state);
            /* ----------------------- 显示刷新 ------------------------- */
            /* Left Screen */
            u8g2_ClearBuffer(&screen_left);

            u8g2_SetFont(&screen_left, digital7_24);
            /* SPD */
            if (fcu_state.spd_dashes) {
                u8g2_DrawStr(&screen_left, SPD_H_POS, 32, "---");
            } else {
                if (fcu_state.spd_mach_mode) { // mach
                    sprintf(display_str_buf, ".%02d", (uint8_t)(fcu_state.spd_selected * 100));
                } else { // knot
                    sprintf(display_str_buf, "%03d", (uint16_t)(fcu_state.spd_selected));
                }
                u8g2_DrawStr(&screen_left, SPD_H_POS, 32, display_str_buf);
            }
            if (fcu_state.spd_dot) { u8g2_DrawFilledEllipse(&screen_left, SPD_H_POS + 40, 23, 4, 4, U8G2_DRAW_ALL); }
            /* HDG */
            if (fcu_state.hdg_dashes) {
                u8g2_DrawStr(&screen_left, HDG_H_POS, 32, "---");
            } else {
                sprintf(display_str_buf, "%03d", fcu_state.hdg_selected);
                u8g2_DrawStr(&screen_left, HDG_H_POS, 32, display_str_buf);
            }
            if (fcu_state.hdg_dot) {
                u8g2_DrawFilledEllipse(&screen_left, HDG_H_POS + 42, 23, 4, 4, U8G2_DRAW_ALL);
            }

            u8g2_SetFont(&screen_left, u8g2_font_finderskeepers_tr);
            if (fcu_state.spd_mach_mode) {
                u8g2_DrawStr(&screen_left, SPD_H_POS + 20, 7, "MACH");
            } else {
                u8g2_DrawStr(&screen_left, SPD_H_POS, 7, "SPD");
            }
            if (fcu_state.trk_fpa_mode) {
                u8g2_DrawStr(&screen_left, HDG_H_POS + 14, 7, "TRK");
                u8g2_DrawStr(&screen_left, 110, 9, "TRK");
                u8g2_DrawStr(&screen_left, 110, 26, "FPA");
            } else {
                u8g2_DrawStr(&screen_left, HDG_H_POS, 7, "HDG");
                u8g2_DrawStr(&screen_left, 110, 9, "HDG");
                u8g2_DrawStr(&screen_left, 110, 26, "V/S");
            }
            u8g2_DrawStr(&screen_left, HDG_H_POS + 34, 7, "LAT");

            u8g2_SendBuffer(&screen_left);
            /* ------- Right Screen --------- */
            u8g2_ClearBuffer(&screen_right);

            u8g2_SetFont(&screen_right, digital7_24);
            sprintf(display_str_buf, "%05d", fcu_state.alt_selected);
            u8g2_DrawStr(&screen_right, ALT_H_POS, 32, display_str_buf);
            if (fcu_state.vs_dashes) {
                u8g2_DrawStr(&screen_right, VS_H_POS, 32, "-----");
            } else {
                if (fcu_state.trk_fpa_mode) { // FPA
                    float2Str(display_str_buf, (float)fcu_state.fpa_selected / 10.0f);
                    u8g2_DrawStr(&screen_right, VS_H_POS, 32, display_str_buf);
                } else {                                                      // V/S
                    sprintf(display_str_buf, "%+03d", fcu_state.vs_selected); // 无需补零
                    u8g2_DrawStr(&screen_right, VS_H_POS, 32, display_str_buf);
                    u8g2_SetFont(&screen_right, digital7_18);
                    u8g2_DrawStr(&screen_right, VS_H_POS + 34, 32, "00");
                }
            }
            if (fcu_state.alt_dot) { u8g2_DrawFilledEllipse(&screen_right, 63, 23, 5, 5, U8G2_DRAW_ALL); }

            u8g2_SetFont(&screen_right, u8g2_font_finderskeepers_tr);
            u8g2_DrawStr(&screen_right, ALT_H_POS + 18, 7, "ALT");
            u8g2_DrawStr(&screen_right, ALT_H_POS + 48, 7, "LVL/CH");
            if (fcu_state.trk_fpa_mode) {
                u8g2_DrawStr(&screen_right, VS_H_POS + 40, 7, "FPA");
            } else {
                u8g2_DrawStr(&screen_right, VS_H_POS + 24, 7, "V/S");
            }
            u8g2_DrawVLine(&screen_right, ALT_H_POS + 36, 3, 4);
            u8g2_DrawHLine(&screen_right, ALT_H_POS + 36, 3, 10);
            u8g2_DrawVLine(&screen_right, VS_H_POS + 20, 3, 4);
            u8g2_DrawHLine(&screen_right, VS_H_POS + 10, 3, 10);

            u8g2_SendBuffer(&screen_right);
#ifdef USE_USB_PAGE
        } else {
            u8g2_SetFont(&screen_right, u8g2_font_crox4h_tr);
            u8g2_ClearBuffer(&screen_right);
            u8g2_DrawStr(&screen_right, 0, 28, "USB Connect.");
            u8g2_SendBuffer(&screen_right);

            u8g2_SetFont(&screen_left, u8g2_font_crox4h_tr);
            u8g2_ClearBuffer(&screen_left);
            u8g2_DrawStr(&screen_left, 20, 28, "Waiting For");
            u8g2_SendBuffer(&screen_left);
        }
#endif
        osDelay(1);
    }
}
void StartKeyScanTask(void *argument)
{
    /* Infinite loop */
    for (;;) {
        KEY_ENUM key = bsp_key_dequeue();
        while (key != KEY_NONE) {
            switch (key) {
                case KEY_SPD_MACH_DOWN:
                    usb_send_buf |= 1 << SPD_MACH_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_SPD_MACH_DOWN\n");
                    break;
                case KEY_SPD_MACH_UP:
                    KEY_DEBUG_PRINTF("KEY_SPD_MACH_UP\n");
                    break;
                case KEY_SPD_MACH_LONG:
                    KEY_DEBUG_PRINTF("KEY_SPD_MACH_LONG\n");
                    break;

                case KEY_SPD_DOWN:
                    usb_send_buf |= 1 << SPD_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_SPD_DOWN\n");
                    break;
                case KEY_SPD_UP:
                    KEY_DEBUG_PRINTF("KEY_SPD_UP\n");
                    break;
                case KEY_SPD_LONG:
                    usb_send_buf |= 1 << SPD_PULL_SFB;
                    KEY_DEBUG_PRINTF("KEY_SPD_LONG\n");
                    break;

                case KEY_TRK_FPA_DOWN:
                    usb_send_buf |= 1 << TRK_FPA_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_TRK_FPA_DOWN\n");
                    break;
                case KEY_TRK_FPA_UP:
                    KEY_DEBUG_PRINTF("KEY_TRK_FPA_UP\n");
                    break;
                case KEY_TRK_FPA_LONG:
                    KEY_DEBUG_PRINTF("KEY_TRK_FPA_LONG\n");
                    break;

                case KEY_AP2_DOWN:
                    usb_send_buf |= 1 << AP2_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_AP2_DOWN\n");
                    break;
                case KEY_AP2_UP:
                    KEY_DEBUG_PRINTF("KEY_AP2_UP\n");
                    break;
                case KEY_AP2_LONG:
                    KEY_DEBUG_PRINTF("KEY_AP2_LONG\n");
                    break;

                case KEY_METRIC_DOWN:
                    usb_send_buf |= 1 << METRIC_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_METRIC_DOWN\n");
                    break;
                case KEY_METRIC_UP:
                    KEY_DEBUG_PRINTF("KEY_METRIC_UP\n");
                    break;
                case KEY_METRIC_LONG:
                    usb_send_buf |= 1 << METRIC_LONG_ALT_INC_TOGGLE_SFB;
                    usb_send_buf ^= 1 << METRIC_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_METRIC_LONG\n");
                    break;

                case KEY_HDG_DOWN:
                    usb_send_buf |= 1 << HDG_PULL_SFB;
                    KEY_DEBUG_PRINTF("KEY_HDG_DOWN\n");
                    break;
                case KEY_HDG_UP:
                    KEY_DEBUG_PRINTF("KEY_HDG_UP\n");
                    break;
                case KEY_HDG_LONG:
                    usb_send_buf |= 1 << HDG_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_HDG_LONG\n");
                    break;

                case KEY_AP1_DOWN:
                    usb_send_buf |= 1 << AP1_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_AP1_DOWN\n");
                    break;
                case KEY_AP1_UP:
                    KEY_DEBUG_PRINTF("KEY_AP1_UP\n");
                    break;
                case KEY_AP1_LONG:
                    KEY_DEBUG_PRINTF("KEY_AP1_LONG\n");
                    break;

                case KEY_ALT_DOWN:
                    usb_send_buf |= 1 << ALT_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_ALT_DOWN\n");
                    break;
                case KEY_ALT_UP:
                    KEY_DEBUG_PRINTF("KEY_ALT_UP\n");
                    break;
                case KEY_ALT_LONG:
                    usb_send_buf |= 1 << ALT_PULL_SFB;
                    KEY_DEBUG_PRINTF("KEY_ALT_LONG\n");
                    break;

                case KEY_VS_DOWN:
                    usb_send_buf |= 1 << VS_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_VS_DOWN\n");
                    break;
                case KEY_VS_UP:
                    KEY_DEBUG_PRINTF("KEY_VS_UP\n");
                    break;
                case KEY_VS_LONG:
                    usb_send_buf |= 1 << VS_PULL_SFB;
                    KEY_DEBUG_PRINTF("KEY_VS_LONG\n");
                    break;

                case KEY_LOC_DOWN:
                    usb_send_buf |= 1 << LOC_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_LOC_DOWN\n");
                    break;
                case KEY_LOC_UP:
                    KEY_DEBUG_PRINTF("KEY_LOC_UP\n");
                    break;
                case KEY_LOC_LONG:
                    KEY_DEBUG_PRINTF("KEY_LOC_LONG\n");
                    break;

                case KEY_ATHR_DOWN:
                    usb_send_buf |= 1 << ATHR_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_ATHR_DOWN\n");
                    break;
                case KEY_ATHR_UP:
                    KEY_DEBUG_PRINTF("KEY_ATHR_UP\n");
                    break;
                case KEY_ATHR_LONG:
                    KEY_DEBUG_PRINTF("KEY_ATHR_LONG\n");
                    break;

                case KEY_EXPED_DOWN:
                    usb_send_buf |= 1 << EXPED_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_EXPED_DOWN\n");
                    break;
                case KEY_EXPED_UP:
                    KEY_DEBUG_PRINTF("KEY_EXPED_UP\n");
                    break;
                case KEY_EXPED_LONG:
                    KEY_DEBUG_PRINTF("KEY_EXPED_LONG\n");
                    break;

                case KEY_APPR_DOWN:
                    usb_send_buf |= 1 << APPR_PUSH_SFB;
                    KEY_DEBUG_PRINTF("KEY_APPR_DOWN\n");
                    break;
                case KEY_APPR_UP:
                    KEY_DEBUG_PRINTF("KEY_APPR_UP\n");
                    break;
                case KEY_APPR_LONG:
                    KEY_DEBUG_PRINTF("KEY_APPR_LONG\n");
                    break;
                default:
                    break;
            }
            key = bsp_key_dequeue();
        }
        osDelay(100);
    }
}
void StartAutopilotBeepTask(void *argument)
{
    /* Infinite loop */
    for (;;) {
        //        if (usb_send_buf) {
        //            USBD_CUSTOM_HID_SendReport_FS((uint8_t *)&usb_send_buf, sizeof(usb_send_buf));
        //            usb_send_buf = 0;
        //        }
        osDelay(portMAX_DELAY);
    }
}
void StartUSBTransmitTask(void *argument)
{
    //    while (usb_inited != 1)
    //        osDelay(100);
    /* Infinite loop */
    static uint32_t start_ticks = 0;
    for (;;) {
        if (usb_send_buf) {
            USBD_CUSTOM_HID_SendReport_FS((uint8_t *)&usb_send_buf, sizeof(usb_send_buf));
            usb_send_buf = 0;
        }
        osDelay(20);
    }
}

#ifdef USE_SELF_SAFE_PRINTF
/**
 * @brief 线程安全的printf方式
 *
 * @param format 同printf的参数。
 * @param ... 在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
 */
void thread_safe_printf(char *format, ...)
{
    char buf_str[200 + 1];
    va_list v_args;

    va_start(v_args, format);
    (void)vsnprintf((char *)&buf_str[0],
                    (size_t)sizeof(buf_str),
                    (char const *)format,
                    v_args);
    va_end(v_args);

    /* 互斥信号量 */
    //    xSemaphoreTake(xMutex, portMAX_DELAY);
    osSemaphoreAcquire(printfMutexHandle, portMAX_DELAY);

    printf("%s", buf_str);

    //    xSemaphoreGive(xMutex);
    osSemaphoreRelease(printfMutexHandle);
}
#endif
/* USER CODE END Application */
