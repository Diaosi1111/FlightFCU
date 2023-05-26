/**
 ******************************************************************************
 * @file    usb_desc.c
 * @author  MCD Application Team
 * @version V4.1.0
 * @date    26-May-2017
 * @brief   Descriptors for Joystick Mouse Demo
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usb_desc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t Joystick_DeviceDescriptor[JOYSTICK_SIZ_DEVICE_DESC] =
    {
        0x12,                       /*bLength */
        USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
        0x00,                       /*bcdUSB */
        0x02,
        0x00, /*bDeviceClass*/
        0x00, /*bDeviceSubClass*/
        0x00, /*bDeviceProtocol*/
        0x40, /*bMaxPacketSize 64*/
        0x83, /*idVendor (0x0483)*/
        0x04,
        0x10, /*idProduct = 0x0010*/
        0x00,
        0x00, /*bcdDevice rel. 2.00*/
        0x02,
        1,   /*Index of string descriptor describing
                               manufacturer */
        2,   /*Index of string descriptor describing
                              product*/
        3,   /*Index of string descriptor describing the
                              device serial number */
        0x01 /*bNumConfigurations*/
};           /* Joystick_DeviceDescriptor */

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t Joystick_ConfigDescriptor[JOYSTICK_SIZ_CONFIG_DESC] =
    {
        // 0x09,                              /* bLength: Configuration Descriptor size */
        // USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
        // JOYSTICK_SIZ_CONFIG_DESC,
        // /* wTotalLength: Bytes returned */
        // 0x00,
        // 0x01, /*bNumInterfaces: 1 interface*/
        // 0x01, /*bConfigurationValue: Configuration value*/
        // 0x00, /*iConfiguration: Index of string descriptor describing
        //                          the configuration*/
        // 0xC0, /*bmAttributes: Self powered */
        // 0x32, /*MaxPower 100 mA: this current is used for detecting Vbus*/

        // /************** Descriptor of Joystick Mouse interface ****************/
        // /* 09 */
        // 0x09,                          /*bLength: Interface Descriptor size*/
        // USB_INTERFACE_DESCRIPTOR_TYPE, /*bDescriptorType: Interface descriptor type*/
        // 0x00,                          /*bInterfaceNumber: Number of Interface*/
        // 0x00,                          /*bAlternateSetting: Alternate setting*/
        // 0x01,                          /*bNumEndpoints*/
        // 0x03,                          /*bInterfaceClass: HID*/
        // 0x00,                          /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
        // 0x00,                          /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
        // 0,                             /*iInterface: Index of string descriptor*/
        // /******************** Descriptor of Joystick Mouse HID ********************/
        // /* 18 */
        // 0x09,                /*bLength: HID Descriptor size*/
        // HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
        // 0x11,                /*bcdHID: HID Class Spec release number*/
        // 0x01,
        // 0x00,                     /*bCountryCode: Hardware target country*/
        // 0x01,                     /*bNumDescriptors: Number of HID class descriptors to follow*/
        // 0x22,                     /*bDescriptorType*/
        // JOYSTICK_SIZ_REPORT_DESC, /*wItemLength: Total length of Report descriptor*/
        // 0x00,
        // /******************** Descriptor of Joystick Mouse endpoint ********************/
        // /* 27 */
        // 0x07,                         /*bLength: Endpoint Descriptor size*/
        // USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

        // 0x81, /*bEndpointAddress: Endpoint Address (IN)*/
        // 0x03, /*bmAttributes: Interrupt endpoint*/
        // 0x04, /*wMaxPacketSize: 8 Byte max */
        // 0x00,
        // 0x20, /*bInterval: Polling Interval (32 ms)*/
        //       /* 34 */
        0x09,                        /* bLength: Configuration Descriptor size */
        USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
        JOYSTICK_SIZ_CONFIG_DESC,
        /* wTotalLength: Bytes returned */
        0x00,
        0x01, /*bNumInterfaces: 1 interface*/
        0x01, /*bConfigurationValue: Configuration value*/
        0x00, /*iConfiguration: Index of string descriptor describing
the configuration*/
        0xC0, /*bmAttributes: bus powered */
        0x32, /*MaxPower 100 mA: this current is used for detecting Vbus*/

        /************** Descriptor of CUSTOM HID interface ****************/
        /* 09 */
        0x09,                    /*bLength: Interface Descriptor size*/
        USB_INTERFACE_DESCRIPTOR_TYPE, /*bDescriptorType: Interface descriptor type*/
        0x00,                    /*bInterfaceNumber: Number of Interface*/
        0x00,                    /*bAlternateSetting: Alternate setting*/
        0x02,                    /*bNumEndpoints*/
        0x03,                    /*bInterfaceClass: CUSTOM_HID*/
        0x00,                    /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
        0x00,                    /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
        0,                       /*iInterface: Index of string descriptor*/
        /******************** Descriptor of CUSTOM_HID *************************/
        /* 18 */
        0x09,                       /*bLength: CUSTOM_HID Descriptor size*/
        HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
        0x11,                       /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
        0x01,
        0x00,                             /*bCountryCode: Hardware target country*/
        0x01,                             /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
        0x22,                             /*bDescriptorType*/
        JOYSTICK_SIZ_REPORT_DESC, /*wItemLength: Total length of Report descriptor*/
        0x00,
        /******************** Descriptor of Custom HID endpoints ********************/
        /* 27 */
        0x07,                   /*bLength: Endpoint Descriptor size*/
        USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

        0x81, /*bEndpointAddress: Endpoint Address (IN)*/
        0x03,                 /*bmAttributes: Interrupt endpoint*/
        CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
        0x00,
        CUSTOM_HID_FS_BINTERVAL, /*bInterval: Polling Interval */
        /* 34 */

        0x07,                   /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
        CUSTOM_HID_EPOUT_ADDR,  /*bEndpointAddress: Endpoint Address (OUT)*/
        0x03,                   /* bmAttributes: Interrupt endpoint */
        CUSTOM_HID_EPOUT_SIZE,  /* wMaxPacketSize: 2 Bytes max  */
        0x00,
        CUSTOM_HID_FS_BINTERVAL, /* bInterval: Polling Interval */
                                 /* 41 */
};                               /* MOUSE_ConfigDescriptor */
const uint8_t FSReportDescriptor[JOYSTICK_SIZ_REPORT_DESC] =
    {

        /* USER CODE BEGIN 0 */

        /* 告诉PC我是桌面类的 */
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)

        /* 告诉PC我是干什么的，他也可以定义成鼠标或者键盘，具体根据自己需求*/
        0x09, 0x04, // USAGE (Joystick)
        //        0x09, 0x05, // USAGE (Game Pad)

        /* 他告诉PC端我的物理逻辑，我们属于输入设备，以外部物理逻辑判断我们的行为再向PC端输入 */
        0xa1, 0x01, // COLLECTION (Application)
        0xa1, 0x02, //   COLLECTION (Logical)

        /* 这是一个游戏手柄的报告描述符，所以拥有X轴和Y轴 */
        0x09, 0x30, //     USAGE (X)
        0x09, 0x31, //     USAGE (Y)
        /* 这是形容XY轴所占的字节大小0-255，没错就是一个字节 */
        0x15, 0x00,       //     LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00, //     LOGICAL_MAXIMUM (255)
        0x35, 0x00,       //     PHYSICAL_MINIMUM (0)
        0x46, 0xff, 0x00, //     PHYSICAL_MAXIMUM (255)
        /* 一次性报告的大小，这里面的大小是按照BIT来算的，一个字节就是8个bit位，在这里我们要知道，
         * usb向PC端报告最小是一个字节，不能比一个字节小，后面会说到按键，按键是按照一个按键占用一个BIT位，
         * 当你的按键数量不够八个的时候，也就是小于一个字节，此时我们要补足，必须要补足一个字节。 */
        0x75, 0x08, //     REPORT_SIZE (8)
        /* 这是报告次数的一次，X轴报告一次，Y轴报告一次共两次。一次一个字节，共两个字节 */
        0x95, 0x02, //     REPORT_COUNT (2)
        /* 是输入还是输出，这里的输入输出是PC端为主 */
        0x81, 0x02, //     INPUT (Data,Var,Abs)28

        //        0x09, 0x39,       //   USAGE (Hat switch)
        //        0x15, 0x00,       //   LOGICAL_MINIMUM (0)
        //        0x25, 0x03,       //   LOGICAL_MAXIMUM (3)
        //        0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
        //        0x46, 0x0e, 0x01, //   PHYSICAL_MAXIMUM (270)
        //        0x65, 0x14,       //   UNIT (Eng Rot:Angular Pos)
        //        0x75, 0x08,       //   REPORT_SIZE (8)
        //        0x95, 0x01,       //   REPORT_COUNT (1)
        //        0x81, 0x02,       //   INPUT (Data,Var,Abs)    19

        //        /* 告诉PC我要添加一个滑块，滑块就是一个ADC采集输入，游戏设备上的油门之类的
        //         * 告诉PC我的报告字节大小，输入还是输出，该滑块占用一个字节。
        //         * 接下来我添加了共6个滑块，PC端最多只能添加6个滑块，添加滑块的时候要注意，这些东西都是PC上定死的，
        //         * 我们添加的时候要注意PC上的规定，比如滑块属性最多只能添加3个，位置在1,5,6，当你再添加的时候就不会再添加了，
        //         * 所以我加了x（在位置2），y（在位置3），z（在位置4）另外三个滑块凑足6个滑块。这些位置是PC定死的。 */
        //        0x09, 0x36,       //   USAGE (Slider)
        //        0x15, 0x00,       //   LOGICAL_MINIMUM (0)
        //        0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
        //        0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
        //        0x46, 0xff, 0x7f, //   PHYSICAL_MAXIMUM (32767)
        //        0x75, 0x08,       //   REPORT_SIZE (8)
        //        0x95, 0x01,       //   REPORT_COUNT (1)
        //        0x81, 0x02,       //   INPUT (Data,Var,Abs)18
        //
        //        0x09, 0x36,       //   USAGE (Slider)
        //        0x15, 0x00,       //   LOGICAL_MINIMUM (0)
        //        0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
        //        0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
        //        0x46, 0xff, 0x7f, //   PHYSICAL_MAXIMUM (32767)
        //        0x75, 0x08,       //   REPORT_SIZE (8)
        //        0x95, 0x01,       //   REPORT_COUNT (1)
        //        0x81, 0x02,       //   INPUT (Data,Var,Abs)18
        //
        //        0x09, 0x36,       //   USAGE (Slider)
        //        0x15, 0x00,       //   LOGICAL_MINIMUM (0)
        //        0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
        //        0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
        //        0x46, 0xff, 0x7f, //   PHYSICAL_MAXIMUM (32767)
        //        0x75, 0x08,       //   REPORT_SIZE (8)
        //        0x95, 0x01,       //   REPORT_COUNT (1)
        //        0x81, 0x02,       //   INPUT (Data,Var,Abs)18
        //
        //        0x09, 0x33,       //   USAGE (x)
        //        0x15, 0x00,       //   LOGICAL_MINIMUM (0)
        //        0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
        //        0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
        //        0x46, 0xff, 0x7f, //   PHYSICAL_MAXIMUM (32767)
        //        0x75, 0x08,       //   REPORT_SIZE (8)
        //        0x95, 0x01,       //   REPORT_COUNT (1)
        //        0x81, 0x02,       //   INPUT (Data,Var,Abs)46   18
        //
        //        0x09, 0x34,       // USAGE (y)
        //        0x15, 0x00,       // LOGICAL_MINIMUM (0)
        //        0x26, 0xff, 0x00, // LOGICAL_MAXIMUM (255)
        //        0x35, 0x00,       // PHYSICAL_MINIMUM (0)
        //        0x46, 0xff, 0x7f, // PHYSICAL_MAXIMUM (32767)
        //        0x75, 0x08,       // REPORT_SIZE (8)
        //        0x95, 0x01,       // REPORT_COUNT (1)
        //        0x81, 0x02,       // INPUT (Data,Var,Abs)46 18
        //
        //        0x09, 0x35,       //   USAGE (z)
        //        0x15, 0x00,       //   LOGICAL_MINIMUM (0)
        //        0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
        //        0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
        //        0x46, 0xff, 0x7f, //   PHYSICAL_MAXIMUM (32767)
        //        0x75, 0x08,       //   REPORT_SIZE (8)
        //        0x95, 0x01,       //   REPORT_COUNT (1)
        //        0x81, 0x02,       //   INPUT (Data,Var,Abs)46   18

        //        /* 告诉PC我要添加按键属性 */
        //        0x05, 0x09, // USAGE_PAGE (Button)
        //        // 按键最小是1最大是32，这里PC最多只能添加32个按键
        //        0x19, 0x01, // USAGE_MINIMUM (Button 1)
        //        0x29, 0x20, // USAGE_MAXIMUM (Button 32)
        //        // 按键逻辑最小为0，最大为1，按键就0和1的逻辑，按下和抬起
        //        0x15, 0x00, // LOGICAL_MINIMUM (0)
        //        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        //        // 按键报告是1个bit，共报告32次，所以占用4个字节
        //        0x95, 0x20, // REPORT_COUNT (32)
        //        0x75, 0x01, // REPORT_SIZE (1)
        //        0x81, 0x02, // INPUT (Data,Var,Abs)
        /* 告诉PC我要添加按键属性 */
        0x05, 0x09, // USAGE_PAGE (Button)
        // 按键最小是1最大是32，这里PC最多只能添加32个按键
        0x19, 0x01, // USAGE_MINIMUM (Button 1)
        0x29, 0x08, // USAGE_MAXIMUM (Button 8)
        // 按键逻辑最小为0，最大为1，按键就0和1的逻辑，按下和抬起
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        // 按键报告是1个bit，共报告32次，所以占用4个字节
        0x95, 0x08, // REPORT_COUNT (8)
        0x75, 0x01, // REPORT_SIZE (1)
        0x81, 0x02, // INPUT (Data,Var,Abs)
        /* 结束，前面用到几个COLLECTION结尾就用到几个结束。
         * 上述报告描述符共用到3个字节: X轴1个字节 Y轴1个字节 按键8个共1个字节
         * 报告描述符修改完了 接下来修改设备描述符 */
        0xc0, // END_COLLECTION
        /* USER CODE END 0 */
        0xC0 /*     END_COLLECTION	             */
};           /* FSReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t Joystick_StringLangID[JOYSTICK_SIZ_STRING_LANGID] =
    {
        JOYSTICK_SIZ_STRING_LANGID,
        USB_STRING_DESCRIPTOR_TYPE,
        0x09,
        0x04}; /* LangID = 0x0409: U.S. English */

const uint8_t Joystick_StringVendor[JOYSTICK_SIZ_STRING_VENDOR] =
    {
        JOYSTICK_SIZ_STRING_VENDOR, /* Size of Vendor string */
        USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType*/
        /* Manufacturer: "STMicroelectronics" */
        'M', 0, 'E', 0, 'G', 0, 'A', 0, 'H', 0, 'U', 0, 'N', 0, 'T', 0,
        'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, ' ', 0, ' ', 0, ' ', 0,
        ' ', 0, ' ', 0};

const uint8_t Joystick_StringProduct[JOYSTICK_SIZ_STRING_PRODUCT] =
    {
        JOYSTICK_SIZ_STRING_PRODUCT, /* bLength */
        USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType */
        'M', 0, 'H', 0, '3', 0, '2', 0, ' ', 0, 'K', 0, 'e', 0,
        'y', 0, 'b', 0, 'o', 0, 'r', 0, 'd', 0, ' ', 0, ' ', 0};
uint8_t Joystick_StringSerial[JOYSTICK_SIZ_STRING_SERIAL] =
    {
        JOYSTICK_SIZ_STRING_SERIAL, /* bLength */
        USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
        'M', 0, 'H', 0, '3', 0, '2', 0, ' ', 0};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
