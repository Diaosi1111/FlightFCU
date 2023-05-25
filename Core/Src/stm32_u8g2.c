#include <stdio.h>
#include "stm32_u8g2.h"
#include "tim.h"
#include "i2c.h"
//static __IO uint8_t i2c2_dma_flag = 0;
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//    if (hi2c->Instance == hi2c2.Instance) {
//        //        printf("IIC2 CALLBACK\n");
//        i2c2_dma_flag = 0;
//    }
//}

uint8_t u8x8_byte_hw_i2c1(__attribute__((unused)) u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buffer[128];
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg) {
        case U8X8_MSG_BYTE_INIT: {
            /* add your custom code to init i2c subsystem */
            MX_I2C1_Init(); // I2C初始化
        } break;

        case U8X8_MSG_BYTE_START_TRANSFER: {
            buf_idx = 0;
        } break;

        case U8X8_MSG_BYTE_SEND: {
            data = (uint8_t *)arg_ptr;

            while (arg_int > 0) {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
        } break;

        case U8X8_MSG_BYTE_END_TRANSFER: {
            if (HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, 0xFF) != HAL_OK)
                return 0;
        } break;
        case U8X8_MSG_BYTE_SET_DC:
            break;

        default:
            return 0;
    }

    return 1;
}

uint8_t u8x8_byte_hw_i2c2(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buffer_i2c2[128];
    static uint8_t buf_idx_i2c2;
    uint8_t *data;

    switch (msg) {
        case U8X8_MSG_BYTE_INIT: {
            /* add your custom code to init i2c subsystem */
            MX_I2C2_Init(); // I2C初始化
        } break;

        case U8X8_MSG_BYTE_START_TRANSFER: {
            buf_idx_i2c2 = 0;
        } break;

        case U8X8_MSG_BYTE_SEND: {
            data = (uint8_t *)arg_ptr;

            while (arg_int > 0) {
                buffer_i2c2[buf_idx_i2c2++] = *data;
                data++;
                arg_int--;
            }

        } break;

        case U8X8_MSG_BYTE_END_TRANSFER: {
            if (HAL_I2C_Master_Transmit(&hi2c2, u8x8_GetI2CAddress(u8x8), buffer_i2c2, buf_idx_i2c2, 0xFF) != HAL_OK)
                return 0;

        } break;

        case U8X8_MSG_BYTE_SET_DC:
            break;

        default:
            return 0;
    }

    return 1;
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg) {
        case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
            __NOP();
            break;
        case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
            for (uint16_t n = 0; n < 320; n++) {
                __NOP();
            }
            break;
        case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
            HAL_Delay(1);
            break;
        case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
            delay_us(5);
            break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
        case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
                                      //            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, arg_int);
            break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
        case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
                                      //            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, arg_int);
            break;                    // arg_int=1: Input dir with pullup high for I2C data pin
        case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
            break;
        default:
            u8x8_SetGPIOResult(u8x8, 1); // default return value
            break;
    }
    return 1;
}

// uint8_t u8x8_gpio_and_delay_template(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
//{
//     switch (msg) {
//         case U8X8_MSG_GPIO_AND_DELAY_INIT: // called once during init phase of u8g2/u8x8
//             break;                         // can be used to setup pins
//         case U8X8_MSG_DELAY_NANO:          // delay arg_int * 1 nano second
//             break;
//         case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
//             __NOP();
//             break;
//         case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
//             for (uint16_t n = 0; n < 320; n++) {
//                 __NOP();
//             }
//         case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
//             HAL_Delay(1);
//             break;
//         case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
//             delay_us(5);
//             break;               // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
//         case U8X8_MSG_GPIO_D0:   // D0 or SPI clock pin: Output level in arg_int
//                                  // case U8X8_MSG_GPIO_SPI_CLOCK:
//             break;
//         case U8X8_MSG_GPIO_D1: // D1 or SPI data pin: Output level in arg_int
//                                // case U8X8_MSG_GPIO_SPI_DATA:
//             break;
//         case U8X8_MSG_GPIO_D2: // D2 pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_D3: // D3 pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_D4: // D4 pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_D5: // D5 pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_D6: // D6 pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_D7: // D7 pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_E: // E/WR pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_CS: // CS (chip select) pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_DC: // DC (data/cmd, A0, register select) pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_RESET: // Reset pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_CS1: // CS1 (chip select) pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_CS2: // CS2 (chip select) pin: Output level in arg_int
//             break;
//         case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
//             break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
//         case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
//             break;                    // arg_int=1: Input dir with pullup high for I2C data pin
//         case U8X8_MSG_GPIO_MENU_SELECT:
//             u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
//             break;
//         case U8X8_MSG_GPIO_MENU_NEXT:
//             u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
//             break;
//         case U8X8_MSG_GPIO_MENU_PREV:
//             u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
//             break;
//         case U8X8_MSG_GPIO_MENU_HOME:
//             u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
//             break;
//         default:
//             u8x8_SetGPIOResult(u8x8, 1); // default return value
//             break;
//     }
//     return 1;
// }

// U8g2的初始化，需要调用下面这个u8g2_Setup_ssd1306_128x64_noname_f函数，该函数的4个参数含义：
// u8g2：传入的U8g2结构体
// U8G2_R0：默认使用U8G2_R0即可（用于配置屏幕是否要旋转）
// u8x8_byte_sw_i2c：使用软件IIC驱动，该函数由U8g2源码提供
// u8x8_gpio_and_delay：就是上面我们写的配置函数
void u8g2_init_hi2c1(u8g2_t *u8g2)
{
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c1, u8x8_gpio_and_delay); // 初始化u8g2 结构体
    u8g2_InitDisplay(u8g2);                                                                           //
    u8g2_SetPowerSave(u8g2, 0);                                                                       //
    u8g2_ClearBuffer(u8g2);
}

void u8g2_init_hi2c2(u8g2_t *u8g2)
{
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c2, u8x8_gpio_and_delay); // 初始化u8g2 结构体
    u8g2_InitDisplay(u8g2);                                                                           //
    u8g2_SetPowerSave(u8g2, 0);                                                                       //
    u8g2_ClearBuffer(u8g2);
}

void draw(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_SetFontMode(u8g2, 1);              /*字体模式选择*/
    u8g2_SetFontDirection(u8g2, 0);         /*字体方向选择*/
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /*字库选择*/
    u8g2_DrawStr(u8g2, 0, 20, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21, 8, "8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51, 30, "g");
    u8g2_DrawStr(u8g2, 67, 30, "\xb2");

    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1, 54, "github.com/olikraus/u8g2");

    u8g2_SendBuffer(u8g2);
    HAL_Delay(1000);
}

// 画点填充
void testDrawPixelToFillScreen(u8g2_t *u8g2)
{
    int t = 1000;
    u8g2_ClearBuffer(u8g2);

    for (int j = 0; j < 64; j++) {
        for (int i = 0; i < 128; i++) {
            u8g2_DrawPixel(u8g2, i, j);
        }
    }
    HAL_Delay(1000);
}