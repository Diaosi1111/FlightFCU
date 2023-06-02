/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
enum USB_SEND_FLAG_POS {
    AP1_PUSH_SFB,
    AP2_PUSH_SFB,
    ATHR_PUSH_SFB,
    LOC_PUSH_SFB,
    EXPED_PUSH_SFB,
    APPR_PUSH_SFB,
    SPD_MACH_PUSH_SFB,
    TRK_FPA_PUSH_SFB,
    METRIC_PUSH_SFB,
    SPD_PUSH_SFB,
    SPD_PULL_SFB,
    SPD_A_SFB,
    SPD_B_SFB,
    HDG_PUSH_SFB,
    HDG_PULL_SFB,
    HDG_A_SFB,
    HDG_B_SFB,
    ALT_PUSH_SFB,
    ALT_PULL_SFB,
    ALT_A_SFB,
    ALT_B_SFB,
    VS_PUSH_SFB,
    VS_PULL_SFB,
    VS_A_SFB,
    VS_B_SFB,
};
typedef struct FCU_Panel_HID {
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

        uint16_t spd_dashes : 1;
        uint16_t spd_dot : 1;
        uint16_t hdg_dashes : 1;
        uint16_t hdg_dot : 1;
        uint16_t alt_dot : 1;
        uint16_t alt_increment : 1; // 0|1=>100|1000
        uint16_t vs_dashes : 1;
    };
    uint16_t hdg_selected;
    float spd_selected;
    int8_t fpa_selected;
    int8_t vs_selected;
    uint16_t alt_selected;
} panel_state_t;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define APPR_LED_Pin GPIO_PIN_13
#define APPR_LED_GPIO_Port GPIOC
#define COL_4_Pin GPIO_PIN_14
#define COL_4_GPIO_Port GPIOC
#define ROW_3_Pin GPIO_PIN_15
#define ROW_3_GPIO_Port GPIOC
#define LOC_LED_Pin GPIO_PIN_0
#define LOC_LED_GPIO_Port GPIOA
#define ALT_B_Pin GPIO_PIN_1
#define ALT_B_GPIO_Port GPIOA
#define ALT_A_Pin GPIO_PIN_2
#define ALT_A_GPIO_Port GPIOA
#define ALT_A_EXTI_IRQn EXTI2_IRQn
#define HDG_B_Pin GPIO_PIN_3
#define HDG_B_GPIO_Port GPIOA
#define HDG_A_Pin GPIO_PIN_4
#define HDG_A_GPIO_Port GPIOA
#define HDG_A_EXTI_IRQn EXTI4_IRQn
#define SPD_B_Pin GPIO_PIN_5
#define SPD_B_GPIO_Port GPIOA
#define SPD_A_Pin GPIO_PIN_6
#define SPD_A_GPIO_Port GPIOA
#define SPD_A_EXTI_IRQn EXTI9_5_IRQn
#define EXPED_LED_Pin GPIO_PIN_7
#define EXPED_LED_GPIO_Port GPIOA
#define AP2_LED_Pin GPIO_PIN_0
#define AP2_LED_GPIO_Port GPIOB
#define ATHR_LED_Pin GPIO_PIN_1
#define ATHR_LED_GPIO_Port GPIOB
#define AP1_LED_Pin GPIO_PIN_12
#define AP1_LED_GPIO_Port GPIOB
#define COL_2_Pin GPIO_PIN_13
#define COL_2_GPIO_Port GPIOB
#define ROW_1_Pin GPIO_PIN_14
#define ROW_1_GPIO_Port GPIOB
#define COL_1_Pin GPIO_PIN_15
#define COL_1_GPIO_Port GPIOB
#define SPD_MACH_Pin GPIO_PIN_8
#define SPD_MACH_GPIO_Port GPIOA
#define ROW_2_Pin GPIO_PIN_4
#define ROW_2_GPIO_Port GPIOB
#define COL_3_Pin GPIO_PIN_5
#define COL_3_GPIO_Port GPIOB
#define VS_B_Pin GPIO_PIN_8
#define VS_B_GPIO_Port GPIOB
#define VS_A_Pin GPIO_PIN_9
#define VS_A_GPIO_Port GPIOB
#define VS_A_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
