////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

////8 bit define
//CHIPTOP
#define REG_FUART_MODE     0x06
#define REG_UART0_MODE     0x06
#define REG_UART1_MODE     0x07
#define REG_SR_MODE        0x0C
#define REG_SR_I2C_MODE    0x0C
#define REG_PWM0_MODE      0x0E
#define REG_PWM1_MODE      0x0E
#define REG_PWM2_MODE      0x0E
#define REG_PWM3_MODE      0x0E
#define REG_PWM4_MODE      0x0F
#define REG_NAND_MODE      0x10
#define REG_SD_MODE        0x10
#define REG_SDIO_MODE      0x11
#define REG_I2C0_MODE      0x12
#define REG_I2C1_MODE      0x12
#define REG_SPI0_MODE      0x18
#define REG_SPI1_MODE      0x18
#define REG_EJ_MODE        0x1E
#define REG_ETH_MODE       0x1E
#define REG_CCIR_MODE      0x1E
#define REG_TTL_MODE       0x1E
#define REG_I2S_MODE       0x1F
#define REG_DMIC_MODE      0x1F
#define REG_TEST_IN_MODE   0x24
#define REG_TEST_OUT_MODE  0x24
#define REG_ALL_PAD_IN     0xA1

//PMSLEEP
#define REG_PMLOCK_L_MODE  0x24
#define REG_PMLOCK_H_MODE  0x25
#define REG_IRIN_MODE      0x38

//SAR
#define REG_SAR_MODE       0x22

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define RIU_MAP _gMIO_MapBase


#define RIU     ((unsigned short volatile *) RIU_MAP)
#define RIU8    ((unsigned char  volatile *) RIU_MAP)

#define MST_MACRO_START     do {
#define MST_MACRO_END       } while (0)

#define MDrv_ReadRegBit( u32Reg, u8Mask )                                               \
        (RIU8[(u32Reg) * 2 - ((u32Reg) & 1)] & (u8Mask))

#define MDrv_WriteRegBit( u32Reg, bEnable, u8Mask )                                     \
    MST_MACRO_START                                                                     \
    MS_U32 u32Reg8 = ((u32Reg) * 2) - ((u32Reg) & 1);                                   \
    RIU8[u32Reg8] = (bEnable) ? (RIU8[u32Reg8] |  (u8Mask)) :                           \
                                (RIU8[u32Reg8] & ~(u8Mask));                            \
    MST_MACRO_END

#define MDrv_WriteByte( u32Reg, u8Val )                                                 \
    MST_MACRO_START                                                                     \
    RIU8[((u32Reg) * 2) - ((u32Reg) & 1)] = u8Val;                                      \
    MST_MACRO_END

#define MDrv_ReadByte( u32Reg )                                                         \
        (RIU8[(u32Reg) * 2 - ((u32Reg) & 1)])

#define MDrv_Read2Byte( u32Reg )                                                        \
        (RIU8[(u32Reg)])

#define MDrv_Write2Byte( u32Reg, u16Val )                                               \
    MST_MACRO_START                                                                     \
    if ((u32Reg) & 0x01)                                                                \
    {                                                                                   \
        RIU8[((u32Reg) * 2) - 1] = (MS_U8)((u16Val));                                   \
        RIU8[((u32Reg) + 1) * 2] = (MS_U8)((u16Val) >> 8);                              \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        RIU[u32Reg] = u16Val;                                                           \
    }                                                                                   \
    MST_MACRO_END

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_

