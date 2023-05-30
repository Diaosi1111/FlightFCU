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
#include "cmsis_os.h"
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t usb_send_buf = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
uint8_t __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 使用状态机来完成数据维护 */
panel_state_t fcu_state = {0};
uint8_t fcu_update      = 1;

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
                usb_send_buf |= 1 << SPD_A_SFB;
                //                fcu_state.spd_selected += 1;
            }
            if (spd_cw1 == 0 && spd_cw2 == 0) {
                usb_send_buf |= 1 << SPD_B_SFB;
                //                fcu_state.spd_selected -= 1;
            }
            spd_flag = 0;
        }
    } else if (GPIO_Pin == HDG_A_Pin) {
        alv = HAL_GPIO_ReadPin(HDG_A_GPIO_Port, HDG_A_Pin);
        blv = HAL_GPIO_ReadPin(HDG_B_GPIO_Port, HDG_B_Pin);
        if (hdg_flag == 0 && alv == 0) {
            hdg_cw1  = blv;
            hdg_flag = 1;
        }
        if (hdg_flag && alv) {
            hdg_cw2 = 1 - blv; // 取反是因为 alv,blv必然异步，一高一低。
            if (hdg_cw1 && hdg_cw2) {
                usb_send_buf |= 1 << HDG_A_SFB;
                //                fcu_state.hdg_selected += 1;
            }
            if (hdg_cw1 == 0 && hdg_cw2 == 0) {
                usb_send_buf |= 1 << HDG_B_SFB;
                //                fcu_state.hdg_selected -= 1;
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
                usb_send_buf |= 1 << ALT_A_SFB;
                fcu_state.alt_selected += fcu_state.alt_increment == 0 ? 100 : 1000;
            }
            if (alt_cw1 == 0 && alt_cw2 == 0) {
                usb_send_buf |= 1 << ALT_B_SFB;
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
                usb_send_buf |= 1 << VS_A_SFB;
                //                fcu_state.vs_selected += 1;
            }
            if (vs_cw1 == 0 && vs_cw2 == 0) {
                usb_send_buf |= 1 << VS_B_SFB;
                //                fcu_state.vs_selected -= 1;
            }
            vs_flag = 0;
        }
    }
}
u8g2_t screen_left;
u8g2_t screen_right;
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

    /* Initialize interrupts */
    MX_NVIC_Init();
    /* USER CODE BEGIN 2 */
    /*------------ 初始化 ---------------*/

    u8g2_init_hi2c2(&screen_left);
    u8g2_init_hi2c1(&screen_right);
    /* USER CODE END 2 */

    /* Init scheduler */
    osKernelInitialize(); /* Call init function for freertos objects (in freertos.c) */
    MX_FREERTOS_Init();

    /* Start scheduler */
    osKernelStart();
    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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
    /* USB_LP_CAN1_RX0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM2 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM2) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

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
