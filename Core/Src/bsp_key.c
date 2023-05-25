#include "bsp_key.h"
#include "tim.h"

#define HARD_KEY_NUM 13
#define SOFT_KEY_NUM 0
#define KEY_COUNT    (HARD_KEY_NUM + SOFT_KEY_NUM) /* 按键个数, 8个独立建 + 2个组合键 */

static bsp_key_t button_list[KEY_COUNT];
static key_fifo_t key_fifo; /* 按键FIFO变量,结构体 */

#define KEY_CLO1_OUT_LOW  HAL_GPIO_WritePin(COL_1_GPIO_Port, COL_1_Pin, GPIO_PIN_RESET)
#define KEY_CLO2_OUT_LOW  HAL_GPIO_WritePin(COL_2_GPIO_Port, COL_2_Pin, GPIO_PIN_RESET)
#define KEY_CLO3_OUT_LOW  HAL_GPIO_WritePin(COL_3_GPIO_Port, COL_3_Pin, GPIO_PIN_RESET)
#define KEY_CLO4_OUT_LOW  HAL_GPIO_WritePin(COL_4_GPIO_Port, COL_4_Pin, GPIO_PIN_RESET)

#define KEY_CLO1_OUT_HIGH HAL_GPIO_WritePin(COL_1_GPIO_Port, COL_1_Pin, GPIO_PIN_SET)
#define KEY_CLO2_OUT_HIGH HAL_GPIO_WritePin(COL_2_GPIO_Port, COL_2_Pin, GPIO_PIN_SET)
#define KEY_CLO3_OUT_HIGH HAL_GPIO_WritePin(COL_3_GPIO_Port, COL_3_Pin, GPIO_PIN_SET)
#define KEY_CLO4_OUT_HIGH HAL_GPIO_WritePin(COL_4_GPIO_Port, COL_4_Pin, GPIO_PIN_SET)

#define KEY_COL_COUNT     4
#define KEY_ROW_COUNT     3

/**
 * 检测高速的输入IO. 1ms刷新一次
 * @param i IO的id， 从0开始编码
 * @param is_pressed 该按键是否按下
 */
static void bsp_matrix_key_detect(uint8_t i, uint8_t is_pressed)
{
    bsp_key_t *p_key;
    p_key = &button_list[i];
    if (is_pressed) {
        if (p_key->State == 0) {
            p_key->State = 1;
            /* 发送按钮按下的消息 */
            bsp_key_enqueue((uint8_t)(3 * i + 1));
        }

        if (p_key->LongTime > 0) {
            if (p_key->LongCount < p_key->LongTime) {
                /* 发送按钮持续按下的消息 */
                if (++p_key->LongCount == p_key->LongTime) {
                    /* 键值放入按键FIFO */
                    bsp_key_enqueue((uint8_t)(3 * i + 3));
                }
            } else {
                if (p_key->RepeatSpeed > 0) {
                    if (++p_key->RepeatCount >= p_key->RepeatSpeed) {
                        p_key->RepeatCount = 0;
                        /* 常按键后，每隔10ms发送1个按键 */
                        bsp_key_enqueue((uint8_t)(3 * i + 1));
                    }
                }
            }
        }
    } else {
        if (p_key->State == 1) {
            p_key->State = 0;

            /* 发送按钮弹起的消息 */
            bsp_key_enqueue((uint8_t)(3 * i + 2));
        }

        p_key->LongCount   = 0;
        p_key->RepeatCount = 0;
    }
}
// static uint16_t key_down_scan = 0;
/***
 *函数名：key_matrix_row_scan
 *功  能：按键行扫描
 *返回值：1~4，对应1~4行按键位置
 */
uint8_t key_matrix_row_scan(uint8_t col)
{
    // 读出行扫描状态
    uint8_t row_scan;
    row_scan = HAL_GPIO_ReadPin(ROW_1_GPIO_Port, ROW_1_Pin);
    row_scan = row_scan | (HAL_GPIO_ReadPin(ROW_2_GPIO_Port, ROW_2_Pin) << 1);
    row_scan = row_scan | (HAL_GPIO_ReadPin(ROW_3_GPIO_Port, ROW_3_Pin) << 2);
    //    row_scan         = ~row_scan;
    uint8_t mask     = 0x01;
    uint8_t key_read = (row_scan & 0x07) << 3 * col;
    for (int i = 0; i < KEY_ROW_COUNT; ++i) {
        if ((row_scan & mask) == 0) {
            bsp_matrix_key_detect(KEY_COL_COUNT * i + col, 0);
        } else {
            bsp_matrix_key_detect(KEY_COL_COUNT * i + col, 1);
            //            key_down_scan |= 1 << (KEY_COL_COUNT * i + col);
            //            key_count++;
        }
        mask <<= 1;
    }
    return key_read;
}

/***
 *函数名：KEY_SCAN
 *功  能：4*4按键扫描
 *返回值：0~16，对应16个按键
 */
uint8_t matrix_key_scan(void)
{
    uint8_t key_count = 0; // 1-16对应的按键数

    KEY_CLO1_OUT_HIGH;
    delay_us(5);
    key_count += key_matrix_row_scan(1);
    KEY_CLO1_OUT_LOW;

    KEY_CLO2_OUT_HIGH;
    delay_us(5);
    key_count += key_matrix_row_scan(2);
    KEY_CLO2_OUT_LOW;

    KEY_CLO3_OUT_HIGH;
    delay_us(5);
    key_count += key_matrix_row_scan(3);
    KEY_CLO3_OUT_LOW;

    KEY_CLO4_OUT_HIGH;
    delay_us(5);
    key_count += key_matrix_row_scan(4);
    KEY_CLO4_OUT_LOW;

    bsp_matrix_key_detect(0, HAL_GPIO_ReadPin(SPD_MACH_GPIO_Port, SPD_MACH_Pin) == GPIO_PIN_RESET);

    return key_count;
}
/**
 * @brief 把一个按键加入FIFO缓冲区
 *
 * @param _KeyCode 按键代码
 */
void bsp_key_enqueue(uint8_t _KeyCode)
{
    key_fifo.buf[key_fifo.Write] = _KeyCode;

    if (++key_fifo.Write >= KEY_FIFO_SIZE) {
        key_fifo.Write = 0;
    }
}

/**
 * @brief 从按键FIFO缓冲区读取一个键值。
 *
 * @return uint8_t 按键代码
 */
uint8_t bsp_key_dequeue(void)
{
    uint8_t ret;

    if (key_fifo.read_ptr == key_fifo.Write) {
        return KEY_NONE;
    } else {
        ret = key_fifo.buf[key_fifo.read_ptr];

        if (++key_fifo.read_ptr >= KEY_FIFO_SIZE) {
            key_fifo.read_ptr = 0;
        }
        return ret;
    }
}

/**
 * @brief 从按键FIFO缓冲区根据2号读取指针读取一个键值。
 *
 * @return uint8_t 按键代码
 */
uint8_t bsp_key_dequeue2(void)
{
    uint8_t ret;

    if (key_fifo.Read2 == key_fifo.Write) {
        return KEY_NONE;
    } else {
        ret = key_fifo.buf[key_fifo.Read2];

        if (++key_fifo.Read2 >= KEY_FIFO_SIZE) {
            key_fifo.Read2 = 0;
        }
        return ret;
    }
}

/**
 * @brief 读取按键的状态
 *
 * @param _ucKeyID 按键ID，从0开始
 * @return uint8_t 1 表示按下， 0 表示未按下
 */
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
    return button_list[_ucKeyID].State;
}

/**
 * @brief 设置按键参数
 *
 * @param _ucKeyID 按键ID，从0开始
 * @param _LongTime 长按事件时间
 * @param _RepeatSpeed 连发速度
 */
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed)
{
    button_list[_ucKeyID].LongTime    = _LongTime;    /* 长按时间 0 表示不检测长按键事件 */
    button_list[_ucKeyID].RepeatSpeed = _RepeatSpeed; /* 按键连发的速度，0表示不支持连发 */
    button_list[_ucKeyID].RepeatCount = 0;            /* 连发计数器 */
}

/**
 * @brief 清空按键FIFO缓冲区
 *
 */
void bsp_ClearKey(void)
{
    key_fifo.read_ptr = key_fifo.Write;
    // key_fifo.Read2 = key_fifo.Write; //TODO:需要加这一行吗
}

/**
 * @brief 初始化全局变量
 *
 */
void bsp_key_init(void)
{
    uint8_t i;

    /* 对按键FIFO读写指针清零 */
    key_fifo.read_ptr = 0;
    key_fifo.Write    = 0;
    key_fifo.Read2    = 0;

    /* 给每个按键结构体成员变量赋一组缺省值 */
    for (i = 0; i < KEY_COUNT; i++) {
        button_list[i].LongTime    = KEY_LONG_TIME;       /* 长按时间 0 表示不检测长按键事件 */
        button_list[i].Count       = KEY_FILTER_TIME / 2; /* 计数器设置为滤波时间的一半 */
        button_list[i].State       = 0;                   /* 按键缺省状态，0为未按下 */
        button_list[i].RepeatSpeed = 0;                   /* 按键连发的速度，0表示不支持连发 */
        button_list[i].RepeatCount = 0;                   /* 连发计数器 */
    }
}

/**
 * 扫描所有按键。非阻塞，被systick中断周期性的调用，10ms一次
 */
void bsp_KeyScan10ms(void)
{
    //    uint8_t i;

    //    for (i = 0; i < KEY_COUNT; i++) {
    //        bsp_DetectKey(i);
    //    }
    matrix_key_scan();
}

/**
 *  扫描所有按键。非阻塞，被 systick 中断周期性的调用，1ms一次.
 */
void bsp_KeyScan1ms(void)
{
    //    uint8_t i;

    matrix_key_scan();
}