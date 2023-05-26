/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_customhid.h"
#include "stdio.h"
#include "bsp_key.h"
#include "u8g2.h"
#include "stm32_u8g2.h"
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

#define KEY_DEBUG_PRINTF(...) printf(__VA_ARGS__)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// typedef  struct FCUPanelDataReceive {
////    struct {
////        uint16_t ap_master: 1;
////        uint16_t ap1: 1;
////        uint16_t ap2: 1;
////        uint16_t ap_throttle: 1;
////        uint16_t loc_mode_active: 1;
////        uint16_t exped_mode_active: 1;
////        uint16_t appr_mode_active: 1;
////        uint16_t alt_inc: 1;
////        uint16_t spd_dashes: 1;
////        uint16_t spd_dot: 1;
////        uint16_t spd_mach: 1;
////        uint16_t hdg_dashes: 1;
////        uint16_t hdg_dot: 1;
////        uint16_t alt_dot: 1;
////        uint16_t vs_dashes: 1;
////        uint16_t trk_fpa_mode: 1;
////    };
//    uint8_t flag1;
//    uint8_t flag2;
//    float spd_selected;
//    int16_t hdg_selected;
//    int16_t fpa_selected;
//    uint16_t alt_selected;
//    int8_t vs_selected;
//} panel_receive_t;
extern const uint8_t digital7_18[1160] U8G2_FONT_SECTION("digital7_18");
extern const uint8_t digital7_20[1299] U8G2_FONT_SECTION("digital7_20");
extern const uint8_t digital7_24[1690] U8G2_FONT_SECTION("digital7_24");
uint32_t usb_send_buf = {0};
unsigned char usb_receive_buf[13];    // USB接收缓存
unsigned char USB_Received_Count = 0; // USB接收数据计数
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
uint8_t __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 使用状态机来完成数据维护 */
typedef struct FCU_Panel_State {

    struct {
        uint16_t ap_master : 1;
        uint16_t ap1_active : 1;
        uint16_t ap2_active : 1;
        uint16_t ap_throttle_active : 1;
        uint16_t loc_mode_active : 1;
        uint16_t exped_mode_active : 1;
        uint16_t appr_mode_active : 1;
        uint16_t spd_mach_mode : 1;
        uint16_t trk_fpa_mode : 1;
        uint16_t metric_mode : 1;

        uint16_t spd_dashes : 1;
        uint16_t spd_dot : 1;
        uint16_t hdg_dashes : 1;
        uint16_t hdg_dot : 1;
        uint16_t alt_dot : 1;
        uint16_t alt_increment : 1; // 0|1=>100|1000
        uint16_t vs_dashes : 1;
    };
    float spd_selected;
    uint16_t hdg_selected;
    int8_t fpa_selected;
    int8_t vs_selected;
    uint16_t alt_selected;
} panel_state_t;

panel_state_t fcu_state = {0};

char display_str_buf[6];
void panel_led_update(panel_state_t *panel)
{
    HAL_GPIO_WritePin(AP1_LED_GPIO_Port, AP1_LED_Pin, panel->ap1_active);
    HAL_GPIO_WritePin(AP2_LED_GPIO_Port, AP2_LED_Pin, panel->ap2_active);
    HAL_GPIO_WritePin(ATHR_LED_GPIO_Port, ATHR_LED_Pin, panel->ap_throttle_active);
    HAL_GPIO_WritePin(LOC_LED_GPIO_Port, LOC_LED_Pin, panel->loc_mode_active);
    HAL_GPIO_WritePin(EXPED_LED_GPIO_Port, EXPED_LED_Pin, panel->exped_mode_active);
    HAL_GPIO_WritePin(APPR_LED_GPIO_Port, APPR_LED_Pin, panel->appr_mode_active);
}
/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // ec11 interrupt variable
    static uint8_t spd_flag, spd_cw1, spd_cw2;
    static uint8_t hdg_flag, hdg_cw1, hdg_cw2;
    static uint8_t alt_flag, alt_cw1, alt_cw2;
    static uint8_t vs_flag, vs_cw1, vs_cw2;
    static uint8_t alv, blv;
    /* Prevent unused argument(s) compilation warning */
    /* NOTE: This function Should not be modified, when the callback is needed,
             the HAL_GPIO_EXTI_Callback could be implemented in the user file
     */
    if (GPIO_Pin == SPD_A_Pin) {
        alv = HAL_GPIO_ReadPin(SPD_A_GPIO_Port, SPD_A_Pin);
        blv = HAL_GPIO_ReadPin(SPD_B_GPIO_Port, SPD_B_Pin);
        if (spd_flag == 0 && alv == 0) {
            spd_cw1  = blv;
            spd_flag = 1;
        }
        if (spd_flag && alv) {
            spd_cw2 = 1 - blv; // 取反是因为 alv,blv必然异步，一高一低。
            if (spd_cw1 && spd_cw2) {
                //                usb_send_buf |= 1 << ALT_A_SFB;
                fcu_state.spd_selected += 1;
            }
            if (spd_cw1 == 0 && spd_cw2 == 0) {
                //                usb_send_buf |= 1 << ALT_B_SFB;
                fcu_state.spd_selected -= 1;
            }
            spd_flag = 0;
        }
    } else if (GPIO_Pin == HDG_A_Pin) {
        alv = HAL_GPIO_ReadPin(HDG_A_GPIO_Port, HDG_A_Pin);
        blv = HAL_GPIO_ReadPin(HDG_B_GPIO_Port, HDG_B_Pin);
        //        printf("alv:%d blv:%d\n", alv, blv);
        if (hdg_flag == 0 && alv == 0) {
            hdg_cw1  = blv;
            hdg_flag = 1;
        }
        if (hdg_flag && alv) {
            hdg_cw2 = 1 - blv; // 取反是因为 alv,blv必然异步，一高一低。
            if (hdg_cw1 && hdg_cw2) {
                fcu_state.hdg_selected += 1;
            }
            if (hdg_cw1 == 0 && hdg_cw2 == 0) {
                fcu_state.hdg_selected -= 1;
            }
            hdg_flag = 0;
        }
    } else if (GPIO_Pin == ALT_A_Pin) {
        alv = HAL_GPIO_ReadPin(ALT_A_GPIO_Port, ALT_A_Pin);
        blv = HAL_GPIO_ReadPin(ALT_B_GPIO_Port, ALT_B_Pin);
        if (alt_flag == 0 && alv == 0) {
            alt_cw1  = blv;
            alt_flag = 1;
        }
        if (alt_flag && alv) {
            alt_cw2 = 1 - blv; // 取反是因为 alv,blv必然异步，一高一低。
            if (alt_cw1 && alt_cw2) {
                fcu_state.alt_selected += fcu_state.alt_increment == 0 ? 100 : 1000;
            }
            if (alt_cw1 == 0 && alt_cw2 == 0) {
                fcu_state.alt_selected -= fcu_state.alt_increment == 0 ? 100 : 1000;
            }
            alt_flag = 0;
        }
    } else if (GPIO_Pin == VS_A_Pin) {
        alv = HAL_GPIO_ReadPin(VS_A_GPIO_Port, VS_A_Pin);
        blv = HAL_GPIO_ReadPin(VS_B_GPIO_Port, VS_B_Pin);
        //        printf("alv:%d blv:%d\n", alv, blv);
        if (vs_flag == 0 && alv == 0) {
            vs_cw1  = blv;
            vs_flag = 1;
        }
        if (vs_flag && alv) {
            vs_cw2 = 1 - blv; // 取反是因为 alv,blv必然异步，一高一低。
            if (vs_cw1 && vs_cw2) {
                //                usb_send_buf |= 1 << ALT_A_SFB;
                fcu_state.vs_selected += 1;
            }
            if (vs_cw1 == 0 && vs_cw2 == 0) {
                //                usb_send_buf |= 1 << ALT_B_SFB;
                fcu_state.vs_selected -= 1;
            }
            vs_flag = 0;
        }
    }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_USART1_UART_Init();
    MX_TIM3_Init();
    MX_USB_DEVICE_Init();

    /* Initialize interrupts */
    MX_NVIC_Init();
    /* USER CODE BEGIN 2 */
    /*------------ 初始化 ---------------*/
    *((__IO unsigned *)((0x40005C00L) + 0x54)) = 1; // DP_PUP

    bsp_key_init();

    u8g2_t screen_left;
    u8g2_t screen_right;
    u8g2_init_hi2c2(&screen_left);
    u8g2_init_hi2c1(&screen_right);

    //    fcu_state.trk_fpa_mode = 1;
    //    fcu_state.spd_dot      = 1;
    //    fcu_state.hdg_dot      = 1;

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        //        printf("2s\n");
        //        HAL_GPIO_TogglePin(ATHR_LED_GPIO_Port, ATHR_LED_Pin);
        /* ----------------------- 按键扫描 ------------------------- */
        //        matrix_key_scan();
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
        /* --------------------- USB 数据发送 ----------------------- */

        /* --------------------- USB 数据接收 ----------------------- */

        /* ----------------------- LED刷新 ------------------------- */
        panel_led_update(&fcu_state);
        //        fcu_state.trk_fpa_mode=1-fcu_state.trk_fpa_mode;
        /* ----------------------- 显示刷新 ------------------------- */
        //        if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) { // USB 已连接
        /* Left Screen */
        u8g2_ClearBuffer(&screen_left);

        u8g2_SetFont(&screen_left, digital7_24);
        /* SPD */
        if (fcu_state.spd_dashes) {
            u8g2_DrawStr(&screen_left, SPD_H_POS, 32, "---");
        } else {
            if (fcu_state.spd_mach_mode) { // mach
                sprintf(display_str_buf, "%0.2f", fcu_state.spd_selected);
            } else { // knot
                sprintf(display_str_buf, "%03d", (uint8_t)fcu_state.spd_selected);
            }
            u8g2_DrawStr(&screen_left, SPD_H_POS, 32, display_str_buf);
        }
        if (fcu_state.spd_dot) { u8g2_DrawFilledEllipse(&screen_left, SPD_H_POS + 40, 23, 4, 4, U8G2_DRAW_ALL); }
        /* HDG */
        if (fcu_state.hdg_dashes) {
            u8g2_DrawStr(&screen_left, HDG_H_POS, 32, "---");
        } else {
            sprintf(display_str_buf, "%03d", (uint8_t)fcu_state.hdg_selected);
            u8g2_DrawStr(&screen_left, HDG_H_POS, 32, display_str_buf);
        }
        if (fcu_state.hdg_dot) {
            u8g2_DrawFilledEllipse(&screen_left, HDG_H_POS + 42, 23, 4, 4, U8G2_DRAW_ALL);
        }

        u8g2_SetFont(&screen_left, u8g2_font_finderskeepers_tr);
        u8g2_DrawStr(&screen_left, SPD_H_POS, 7, "SPD");
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
            if (fcu_state.trk_fpa_mode) {                                          // FPA
                sprintf(display_str_buf, "%+1.1f", fcu_state.fpa_selected / 10.0); // 无需补零
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
        //        }
        //        else{
        //        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct   = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection    = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief NVIC Configuration.
 * @retval None
 */
static void MX_NVIC_Init(void)
{
    /* USB_HP_CAN1_TX_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    /* USB_LP_CAN1_RX0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    printf("Error Handler!\n");
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
