/**
 ******************************************************************************
 * @file    platform_config.h
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   Evaluation board specific configuration file.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"

// IO�ڲ����궨��
#define BITBAND(addr, bitnum)  (((addr)&0xF0000000) + 0x2000000 + (((addr)&0xFFFFF) << 5) + ((bitnum) << 2))
#define MEM_ADDR(addr)         *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOA_ODR_Addr         (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr         (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr         (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr         (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr         (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr         (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr         (GPIOG_BASE + 12) // 0x40011E0C

#define GPIOA_IDR_Addr         (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr         (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr         (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr         (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr         (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr         (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr         (GPIOG_BASE + 8) // 0x40011E08

#define PAout(n)               BIT_ADDR(GPIOA_ODR_Addr, n) // ���
#define PAin(n)                BIT_ADDR(GPIOA_IDR_Addr, n) // ����

#define PBout(n)               BIT_ADDR(GPIOB_ODR_Addr, n) // ���
#define PBin(n)                BIT_ADDR(GPIOB_IDR_Addr, n) // ����

#define PCout(n)               BIT_ADDR(GPIOC_ODR_Addr, n) // ���
#define PCin(n)                BIT_ADDR(GPIOC_IDR_Addr, n) // ����

#define PDout(n)               BIT_ADDR(GPIOD_ODR_Addr, n) // ���
#define PDin(n)                BIT_ADDR(GPIOD_IDR_Addr, n) // ����

#define PEout(n)               BIT_ADDR(GPIOE_ODR_Addr, n) // ���
#define PEin(n)                BIT_ADDR(GPIOE_IDR_Addr, n) // ����

#define PFout(n)               BIT_ADDR(GPIOF_ODR_Addr, n) // ���
#define PFin(n)                BIT_ADDR(GPIOF_IDR_Addr, n) // ����

#define PGout(n)               BIT_ADDR(GPIOG_ODR_Addr, n) // ���
#define PGin(n)                BIT_ADDR(GPIOG_IDR_Addr, n) // ����
#define ID1                    (0x1FFFF7E8)
#define ID2                    (0x1FFFF7EC)
#define ID3                    (0x1FFFF7F0)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */

#endif /* __PLATFORM_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
