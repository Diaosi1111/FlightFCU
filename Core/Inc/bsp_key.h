#ifndef BSP_KEY_H
#define BSP_KEY_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 根据应用程序的功能重命名按键宏 */
#define KEY_DOWN_K1 KEY_1_DOWN
#define KEY_UP_K1   KEY_1_UP
#define KEY_LONG_K1 KEY_1_LONG

// #define KEY_DOWN_K2   KEY_2_DOWN
// #define KEY_UP_K2     KEY_2_UP
// #define KEY_LONG_K2   KEY_2_LONG

// #define KEY_DOWN_K3   KEY_3_DOWN
// #define KEY_UP_K3     KEY_3_UP
// #define KEY_LONG_K3   KEY_3_LONG

// #define JOY_DOWN_U    KEY_4_DOWN /* 上 */
// #define JOY_UP_U      KEY_4_UP
// #define JOY_LONG_U    KEY_4_LONG

// #define JOY_DOWN_D    KEY_5_DOWN /* 下 */
// #define JOY_UP_D      KEY_5_UP
// #define JOY_LONG_D    KEY_5_LONG

// #define JOY_DOWN_L    KEY_6_DOWN /* 左 */
// #define JOY_UP_L      KEY_6_UP
// #define JOY_LONG_L    KEY_6_LONG

// #define JOY_DOWN_R    KEY_7_DOWN /* 右 */
// #define JOY_UP_R      KEY_7_UP
// #define JOY_LONG_R    KEY_7_LONG

// #define JOY_DOWN_OK   KEY_8_DOWN /* ok */
// #define JOY_UP_OK     KEY_8_UP
// #define JOY_LONG_OK   KEY_8_LONG

// #define SYS_DOWN_K1K2 KEY_9_DOWN /* K1 K2 组合键 */
// #define SYS_UP_K1K2   KEY_9_UP
// #define SYS_LONG_K1K2 KEY_9_LONG

// #define SYS_DOWN_K2K3 KEY_10_DOWN /* K2 K3 组合键 */
// #define SYS_UP_K2K3   KEY_10_UP
// #define SYS_LONG_K2K3 KEY_10_LONG

/* 按键ID, 主要用于bsp_KeyState()函数的入口参数 */
typedef enum {
    KID_SPD_MACH = 0,
    KID_SPD,
    KID_TRK_FPA,
    KID_AP2,
    KID_METRIC,
    KID_HDG,
    KID_AP1,
    KID_ALT,
    KID_VS,
    KID_LOC,
    KID_ATHR,
    KID_EXPED,
    KID_APPR,

    //virtual key
    KID_SPD_PULL,
    KID_HDG_PULL,
    KID_ALT_PULL,
    KID_VS_PULL,
} KEY_ID_E;

/*
    按键滤波时间50ms, 单位10ms。
    只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
    即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME 5
#define KEY_LONG_TIME   100 /* 单位10ms， 持续1秒，认为长按事件 */
/*
    每个按键对应1个全局的结构体变量。
*/
typedef struct {
    /* 下面是一个函数指针，指向判断按键手否按下的函数 */
    uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

    uint8_t Count;       /* 滤波器计数器 */
    uint16_t LongCount;  /* 长按计数器 */
    uint16_t LongTime;   /* 按键按下持续时间, 0表示不检测长按 */
    uint8_t State;       /* 按键当前状态（按下还是弹起） */
    uint8_t RepeatSpeed; /* 连续按键周期 */
    uint8_t RepeatCount; /* 连续按键计数器 */
} bsp_key_t;

/*
    定义键值代码, 必须按如下次序定时每个键的按下、弹起和长按事件

    推荐使用enum, 不用#define，原因：
    (1) 便于新增键值,方便调整顺序，使代码看起来舒服点
    (2) 编译器可帮我们避免键值重复。
*/
typedef enum {
    KEY_NONE = 0, /* 0 表示按键事件 */

    KEY_SPD_MACH_DOWN,
    KEY_SPD_MACH_UP,
    KEY_SPD_MACH_LONG,

    KEY_SPD_DOWN,
    KEY_SPD_UP,
    KEY_SPD_LONG,

    KEY_TRK_FPA_DOWN,
    KEY_TRK_FPA_UP,
    KEY_TRK_FPA_LONG,

    KEY_AP2_DOWN,
    KEY_AP2_UP,
    KEY_AP2_LONG,

    KEY_METRIC_DOWN,
    KEY_METRIC_UP,
    KEY_METRIC_LONG,

    KEY_HDG_DOWN,
    KEY_HDG_UP,
    KEY_HDG_LONG,

    KEY_AP1_DOWN,
    KEY_AP1_UP,
    KEY_AP1_LONG,

    KEY_ALT_DOWN,
    KEY_ALT_UP,
    KEY_ALT_LONG,

    KEY_VS_DOWN,
    KEY_VS_UP,
    KEY_VS_LONG,

    KEY_LOC_DOWN,
    KEY_LOC_UP,
    KEY_LOC_LONG,

    KEY_ATHR_DOWN,
    KEY_ATHR_UP,
    KEY_ATHR_LONG,

    KEY_EXPED_DOWN,
    KEY_EXPED_UP,
    KEY_EXPED_LONG,

    KEY_APPR_DOWN,
    KEY_APPR_UP,
    KEY_APPR_LONG,
} KEY_ENUM;

/* 按键FIFO队列长度 */
#define KEY_FIFO_SIZE 20

typedef struct {
    uint8_t buf[KEY_FIFO_SIZE]; /* 键值缓冲区 */
    uint8_t read_ptr;               /* 缓冲区读指针1 */
    uint8_t Write;              /* 缓冲区写指针 */
    uint8_t Read2;              /* 缓冲区读指针2 */
} key_fifo_t;

/* 供外部调用的函数声明 */
void bsp_key_init(void);


void bsp_key_enqueue(uint8_t _KeyCode);

uint8_t bsp_key_dequeue(void);

uint8_t bsp_key_dequeue2(void);

uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);

void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed);

void bsp_ClearKey(void);

#ifdef __cplusplus
}
#endif
#endif /* BSP_KEY_H */
