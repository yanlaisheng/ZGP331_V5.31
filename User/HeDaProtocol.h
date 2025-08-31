/**
 ******************************************************************************
 * @file    HeDaProtocol.h
 * @author  Laisheng Yan  - 燕来胜
 * @version V1.00
 * @date    2025-08-23
 * @brief   和达协议数据结构声明
 ******************************************************************************
 */

#ifndef __HEDAPROTOCOL_H
#define __HEDAPROTOCOL_H

#include "GlobalConst.h"

#define HEDA_TLV1_NESTED_NUM 11
#define HEDA_TLV2_ITEM_NUM 44
#define HEDA_DATA_LEN_ALL 0x010C
#define HEDA_DATA_LEN_TLV1 0x0004
#define HEDA_DATA_LEN_TLV2 0x0102

// 单个TLV结构
typedef struct
{
    uint8_t tag;
    uint16_t len;
    uint8_t DataLength; // 数据实际长度（字节数）
    uint8_t value[32];  // 根据实际最大长度调整
} HeDaTLV;

// 嵌套TLV结构（第一个TLV的value是11个TLV）
typedef struct
{
    uint8_t Tlv1_tag;
    uint16_t Tlv1_len;
    HeDaTLV Tlv1_nested[HEDA_TLV1_NESTED_NUM];
} HeDaTLV_Nested;

typedef union
{
    uint16_t value;
    struct QValueBits
    {
        uint16_t bit0 : 1;
        uint16_t bit1 : 1;
        uint16_t bit2 : 1;
        uint16_t bit3 : 1;
        uint16_t bit4 : 1;
        uint16_t bit5 : 1;
        uint16_t bit6 : 1;
        uint16_t bit7 : 1;
        uint16_t bit8 : 1;
        uint16_t bit9 : 1;
        uint16_t bit10 : 1;
        uint16_t bit11 : 1;
        uint16_t bit12 : 1;
        uint16_t bit13 : 1;
        uint16_t bit14 : 1;
        uint16_t bit15 : 1;
    } bits;
} QValue_t;

typedef union
{
    uint32_t value;
    struct MValueBits
    {
        uint32_t bit0 : 1;
        uint32_t bit1 : 1;
        uint32_t bit2 : 1;
        uint32_t bit3 : 1;
        uint32_t bit4 : 1;
        uint32_t bit5 : 1;
        uint32_t bit6 : 1;
        uint32_t bit7 : 1;
        uint32_t bit8 : 1;
        uint32_t bit9 : 1;
        uint32_t bit10 : 1;
        uint32_t bit11 : 1;
        uint32_t bit12 : 1;
        uint32_t bit13 : 1;
        uint32_t bit14 : 1;
        uint32_t bit15 : 1;
        uint32_t bit16 : 1;
        uint32_t bit17 : 1;
        uint32_t bit18 : 1;
        uint32_t bit19 : 1;
        uint32_t bit20 : 1;
        uint32_t bit21 : 1;
        uint32_t bit22 : 1;
        uint32_t bit23 : 1;
        uint32_t bit24 : 1;
        uint32_t bit25 : 1;
        uint32_t bit26 : 1;
        uint32_t bit27 : 1;
        uint32_t bit28 : 1;
        uint32_t bit29 : 1;
        uint32_t bit30 : 1;
        uint32_t bit31 : 1;
    } bits;
} MValue_t;

typedef union
{
    uint32_t PulseValue_u32;     // 4字节长整型（大端模式，发送/接收时需转换字节序）
    uint16_t PulseValue_u16[2];  // 2个2字节整型（大端模式，发送/接收时需转换字节序）
    uint8_t PulseValue_bytes[4]; // 原始字节访问
} PulseValue_t;

// 数据项类型定义
// 这样你既可以用item.Data.QValue.value访问Q通道的整体数值，也可以用item.Data.QValue.bit3等方式访问单个位。M通道同理。
typedef struct
{
    uint8_t DataType;     // 数据类型：1=脉冲量(4字节uint32)，2=开关量(1字节)，3=模拟量(4字节float)，4=Q通道(2字节)，5=M通道(4字节)
    uint8_t ChannelIndex; // 通道序号
    uint8_t DataLength;   // 数据实际长度（字节数）
    union
    {
        /*
        注意事项：
        发送和接收时，需根据协议要求将PulseValue_t中的数据转换为大端字节序（如htonl、htons，或手动字节交换）。
        访问4字节整型时用item.Data.PulseValue.u32，访问2个2字节整型时用item.Data.PulseValue.u16[0]和u16[1]，访问原始字节时用bytes[0~3]。
        这样定义既兼容两种脉冲量格式，也方便后续协议打包和解析。
        */
        PulseValue_t PulseValue; // 脉冲量
        uint8_t hditchValue;     // 开关量
        float AnalogValue;       // 模拟量
        QValue_t QValue;         // Q通道，支持按位访问
        MValue_t MValue;         // M通道，支持按位访问
        uint8_t Raw[4];          // 最大长度占位
    } Data;
} HeDaTLV2_Item;

// 单通道数据
typedef struct
{
    uint8_t itemCount; // 实际数据项个数
    HeDaTLV2_Item items[HEDA_TLV2_ITEM_NUM];
} HeDaTLV2_Channel;

// TLV2数据区结构体
typedef struct
{
    uint8_t Tlv2_tag;
    uint16_t Tlv2_len;
    uint8_t Tlv2_hd_DataStartTimeStamp[6]; // 数据开始时间戳 BCD码  6个字节 YYMMDDHHMMSS
    uint8_t Tlv2_DataLog_Interval;         // 数据记录间隔，0=分钟；1=秒；2=毫秒
    uint16_t Tlv2_DataLog_IntervalValue;   // 数据记录间隔值
    uint8_t Tlv2_DataLog_Num;              // 数据记录个数，最大255

    // 下面定义数据区，数据区有2台泵，每台泵有18个P（1：脉冲量类型，4 字节，无符号长整型）+4个Q（Q 通道类型，2 字节，按位解析）；
    // P1-P18、Q1-Q4为1号泵；Q19-Q36、Q5-Q8为2号泵
    HeDaTLV2_Item items[HEDA_TLV2_ITEM_NUM];

} HeDaTLV2;

// 主协议包结构体
typedef struct
{
    uint16_t hd_StartFlag;        // 1、帧起始标志 2个字节 0xF88F
    uint8_t hd_DeviceID[8];       // 2、设备ID 8个字节 BCD码
    uint16_t hd_CompanyCode;      // 3、厂商代码 2个字节 三利：0x1001
    uint8_t hd_ProtocolVersion;   // 4、协议版本， 1个字节 当前版本为1
    uint8_t hd_TimeStamp[6];      // 5、时间戳 BCD码  6个字节 YYMMDDHHMMSS
    uint8_t hd_CmdType;           // 6、命令类型  1个字节 数据上报功能码为02
    uint16_t hd_SendOrder;        // 7、发送流水序号 2个字节 每次发送数据时加1，溢出后清零
    uint8_t hd_EncryptionMethod;  // 8、加密方式 1个字节，=0不加密
    uint16_t hd_KeyVersion;       // 9、秘钥版本号 2个字节
    uint8_t hd_CompressionMethod; // 10、压缩方式 1个字节 =0不压缩
    uint16_t hd_ReserveData;      // 11、保留字段 2个字节
    uint16_t hd_DataDomain_Len;   // 12、数据域长度 2个字节 hd_DataDomain_Len=hd_Data_Len+2
    uint16_t hd_Data_Len;         // 13、数据长度 2个字节

    // tlv数据
    // 14、第一个TLV（嵌套11个TLV）
    // HeDaTLV_Nested tlv1;
    uint8_t Tlv1_tag;
    uint16_t Tlv1_len;
    HeDaTLV Tlv1_nested[HEDA_TLV1_NESTED_NUM];

    // 15、第二个TLV（数据区）
    // HeDaTLV2 tlv2;
    uint8_t Tlv2_tag;
    uint16_t Tlv2_len;
    uint8_t Tlv2_hd_DataStartTimeStamp[6]; // 数据开始时间戳 BCD码  6个字节 YYMMDDHHMMSS
    uint8_t Tlv2_DataLog_Interval;         // 数据记录间隔，0=分钟；1=秒；2=毫秒
    uint16_t Tlv2_DataLog_IntervalValue;   // 数据记录间隔值
    uint8_t Tlv2_DataLog_Num;              // 数据记录个数，最大255

    // 下面定义数据区，数据区有2台泵，每台泵有18个P（1：脉冲量类型，4 字节，无符号长整型）+4个Q（Q 通道类型，2 字节，按位解析）；
    // P1-P18、Q1-Q4为1号泵；Q19-Q36、Q5-Q8为2号泵
    HeDaTLV2_Item items[HEDA_TLV2_ITEM_NUM];
    uint8_t hd_Sum;     // 16、校验和 1个字节
    uint8_t hd_EndFlag; // 17、帧结束标志 1个字节 0x16

} HeDaPacket;

#endif
