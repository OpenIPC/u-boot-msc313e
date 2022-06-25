//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvSPINAND.c
/// @brief  SPI NAND Flash Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <common.h>
#include <malloc.h>
#include <linux/err.h>
//#include <linux/compat.h>
#include <asm/io.h>
#include <asm/errno.h>


#include <linux/string.h>
// Common Definition
#include "MsCommon.h"
#include "MsVersion.h"
#include "MsTypes.h"
//#include "MsOS.h"
#include "spinand.h"
// Internal Definition
#include "../../inc/common/drvSPINAND.h"

//	#ifndef _DRV_SPICMD_H_
//	#include "../../inc/common/drvSPICMD.h"
//	#endif


//extern SPINAND_FLASH_INFO_t _gtSpinandInfo;


#if defined(MSOS_TYPE_LINUX)
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#endif

//==========================================================================
// Define
//==========================================================================

//==========================================================================
// Global Variable
//==========================================================================
U8 u8MagicData[] = {0x4D, 0x53, 0x54, 0x41, 0x52, 0x53, 0x45, 0x4D, 0x49, 0x55, 0x53, 0x46, 0x44, 0x43, 0x49, 0x53};
#define HW_TIMER_DELAY_1us          1
#define HW_TIMER_DELAY_10us         10
#define HW_TIMER_DELAY_100us        100
#define HW_TIMER_DELAY_1ms          (1000 * HW_TIMER_DELAY_1us)
#define HW_TIMER_DELAY_5ms          (5    * HW_TIMER_DELAY_1ms)

//SPINAND_FLASH_INFO_t gtSpiNandInfoTable[]=
//{    //u8_IDByteCnt           au8_ID                                                                u16_SpareByteCnt   u16_PageByteCnt   u16_BlkPageCnt   u16_BlkCnt  u16_SectorByteCnt
//    {2, {MID_GD     , 0xF4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 4096, 512, 0},
//    {2, {MID_GD     , 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_GD     , 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_GD     , 0xD1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_GD     , 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_GD     , 0xD2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 2048, 512, 0},
//    {2, {MID_MICRON , 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 2},
//    {2, {MID_MICRON , 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 2048, 512, 2},
//    {2, {MID_MICRON , 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 4096, 512, 2},
//    {2, {MID_ATO    , 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_WINBOND, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_MXIC   , 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 1024, 512, 0},
//    {2, {MID_MXIC   , 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 2048, 512, 2},
//    {2, {MID_TOSHIBA, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64, 2048, 64, 2048, 512, 2},
//    {2, {0x00       , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  0,    0,  0,    0,   0, 0},
//};



//==========================================================================
// Global Function
//==========================================================================
static BOOL _MDrv_SPINAND_GET_INFO(void)
{
    U32 u32Ret;
    U8 u8Spare[16];
    U8 u8Data[512];
    U8 u8Idx, u8Status;
    U8 *u8SrcAddr, *u8DstAddr;
    U16 u16PageIndex;
    SPI_NAND_DRIVER_t *pSpiNandDrv = (SPI_NAND_DRIVER_t*)drvSPINAND_get_DrvContext_address();
    u8SrcAddr = u8Data;
    u8DstAddr = (U8*)(&pSpiNandDrv->tSpinandInfo);
//    RIU_FLAG = TRUE;
    // if ecc error read back up block(block2 ,4, 6, 8)
    for(u8Idx = 0; u8Idx < 10; u8Idx+=2)
    {
        u16PageIndex = (pSpiNandDrv->tSpinandInfo.u16_BlkPageCnt) * u8Idx;
        //read data to cache first
        u32Ret = HAL_SPINAND_RFC(u16PageIndex, &u8Status);

        if(u32Ret != ERR_SPINAND_SUCCESS)//|| u8Status & ECC_STATUS_ERR
        {
            spi_nand_debug("read data to cache first != ERR_SPINAND_SUCCESS\n");
            continue;
        }


        // Read SPINand Data
        u32Ret = HAL_SPINAND_Read (0, 512, u8SrcAddr);
        if(u32Ret != ERR_SPINAND_SUCCESS)
        {
            spi_nand_debug("Read SPINand Data != ERR_SPINAND_SUCCESS\n");
            continue;
        }

        // Read SPINand Spare Data
        u32Ret = HAL_SPINAND_Read(2048, 16, u8Spare);
        if(u32Ret != ERR_SPINAND_SUCCESS)
        {
            spi_nand_debug("Read SPINand Spare Data != ERR_SPINAND_SUCCESS\n");
            continue;
        }

        if(memcmp((const void *) u8SrcAddr, (const void *) u8MagicData, sizeof(u8MagicData)) != 0)
        {
            spi_nand_err("CIS Error\n");
            continue;
        }

        u8SrcAddr += sizeof(u8MagicData);
        memcpy(u8DstAddr, u8SrcAddr, sizeof(SPINAND_FLASH_INFO_t));
        {
            U8 u8ID[pSpiNandDrv->tSpinandInfo.u8_IDByteCnt];
            HAL_SPINAND_ReadID(pSpiNandDrv->tSpinandInfo.u8_IDByteCnt, u8ID);

            if(memcmp((const void *) pSpiNandDrv->tSpinandInfo.au8_ID, (const void *)u8ID, pSpiNandDrv->tSpinandInfo.u8_IDByteCnt) != 0)
            {
                spi_nand_err("memcmp ID diff\n");
                continue;
            }
            spi_nand_msg("Found spinand INFO \n");
			spi_nand_msg("Device detect! MID =%x, DID =%x\n",u8ID[0], u8ID[1]);
        }
        return TRUE;
    }
    return FALSE;
}

BOOL MDrv_SPINAND_Init(SPINAND_FLASH_INFO_t *tSpinandInfo)
{

    U8 u8ID[SPINAND_ID_SIZE];
    //U32 u32Index;
    U32 u32Ret;
    BOOL bReFind = FALSE;
    SPI_NAND_DRIVER_t *pSpiNandDrv = (SPI_NAND_DRIVER_t*)drvSPINAND_get_DrvContext_address();
    // 1. HAL init
    //
    _u8SPINANDDbgLevel = E_SPINAND_DBGLV_DEBUG;

MDrv_SPINAND_Init_Detect_ID:
#if 1
    u32Ret = HAL_SPINAND_Init();
    if(u32Ret != ERR_SPINAND_SUCCESS)
    {
        tSpinandInfo->au8_ID[0] = 0xFF;
        tSpinandInfo->au8_ID[1] = 0xFF;
        tSpinandInfo->u8_IDByteCnt = 2;
        if(!bReFind)
        {
            bReFind = TRUE;
            HAL_SPINAND_CSCONFIG();
            goto MDrv_SPINAND_Init_Detect_ID;
        }
        return FALSE;
    }
#endif
    /*_MDrv_SPINAND_GET_INFO : Search CIS in block 0 2 4 8.*/
    if(!_MDrv_SPINAND_GET_INFO())
    {
        u32Ret = HAL_SPINAND_ReadID(SPINAND_ID_SIZE, u8ID);
        if(u32Ret != ERR_SPINAND_SUCCESS)
        {
            spi_nand_debug("Can't not Detect SPINAND Device!!!!\r");
            tSpinandInfo->au8_ID[0] = 0xFF;
            tSpinandInfo->au8_ID[1] = 0xFF;
            tSpinandInfo->u8_IDByteCnt = 2;
            return FALSE;
        }
        printf("Spi-nand device detect! MID =%x, DID =%x\r",u8ID[0], u8ID[1]);
        tSpinandInfo->au8_ID[0] = u8ID[0];
        tSpinandInfo->au8_ID[1] = u8ID[1];
        tSpinandInfo->u8_IDByteCnt = 2;
        return FALSE;
#if 0
        spi_nand_debug("Search nand ID on internal table");
        for (u32Index = 0; gtSpiNandInfoTable[u32Index].au8_ID[0] != 0; u32Index++)
        {
            if(gtSpiNandInfoTable[u32Index].au8_ID[0] == u8ID[0] &&
               gtSpiNandInfoTable[u32Index].au8_ID[1] == u8ID[1])
            {
                spi_nand_debug("SPINAND Device DETECT");
                memcpy(tSpinandInfo, &gtSpiNandInfoTable[u32Index], sizeof(SPINAND_FLASH_INFO_t));
                memcpy(&_gtSpinandInfo, &gtSpiNandInfoTable[u32Index], sizeof(SPINAND_FLASH_INFO_t));
                break;
            }
        }
        if((!gtSpiNandInfoTable[u32Index].au8_ID[0]) && (!gtSpiNandInfoTable[u32Index].au8_ID[1]))
        {
            tSpinandInfo->au8_ID[0] = u8ID[0];
            tSpinandInfo->au8_ID[1] = u8ID[1];
            tSpinandInfo->u8_IDByteCnt = 2;
            if(!bReFind)
            {
                bReFind = TRUE;
                HAL_SPINAND_CSCONFIG();
                goto MDrv_SPINAND_Init_Detect_ID;
            }
            return FALSE;
        }
#endif
    }
    else
    {
        memcpy(tSpinandInfo, &pSpiNandDrv->tSpinandInfo, sizeof(SPINAND_FLASH_INFO_t));
    }

    //HAL_SPINAND_SetCKG(CLKCFG);
    return TRUE;
}
/*void MDrv_SPINAND_ForceInit(SPINAND_FLASH_INFO_t *tSpinandInfo)
{
    memcpy(&_gtSpinandInfo, tSpinandInfo, sizeof(SPINAND_FLASH_INFO_t));
}*/

//-------------------------------------------------------------------------------------------------
// Read SPINAND Data
// @param u32_PageIdx : page index of read data in specific block
// @return TRUE : succeed
// @return FALSE : fail
// @note : If Enable ISP engine, the XIU mode does not work
//-------------------------------------------------------------------------------------------------

U32 MDrv_SPINAND_Read(U32 u32_PageIdx, U8 *u8Data, U8 *pu8_SpareBuf)
{
    U8 u8Status;
    U32 u32Ret = ERR_SPINAND_SUCCESS;
    U16 u16ColumnAddr = 0;
    SPI_NAND_DRIVER_t *pSpiNandDrv = (SPI_NAND_DRIVER_t*)drvSPINAND_get_DrvContext_address();

    //read data to cache first
    u32Ret = HAL_SPINAND_RFC(u32_PageIdx, &u8Status);

    if(u32Ret != ERR_SPINAND_SUCCESS)
    {
        return u32Ret;
    }

    HAL_SPINAND_PLANE_HANDLER(u32_PageIdx);

    // Read SPINand Data

    if(pSpiNandDrv->tSpinandInfo.u8PlaneCnt && (((u32_PageIdx/pSpiNandDrv->tSpinandInfo.u16_BlkPageCnt)&0x1) == 1))
    {
        u16ColumnAddr = (1<<12); // plane select for MICRON & 2GB
        spi_nand_msg("PLANE: u16ColumnAddr %x\n", u16ColumnAddr);
    }
    udelay(HW_TIMER_DELAY_1ms);
    u32Ret = HAL_SPINAND_Read (u16ColumnAddr, pSpiNandDrv->tSpinandInfo.u16_PageByteCnt, u8Data);
    if(u32Ret != ERR_SPINAND_SUCCESS)
    {
        return u32Ret;
    }

    // Read SPINand Spare Data
    u32Ret= HAL_SPINAND_Read(pSpiNandDrv->tSpinandInfo.u16_PageByteCnt, pSpiNandDrv->tSpinandInfo.u16_SpareByteCnt, pu8_SpareBuf);

    if(u32Ret == ERR_SPINAND_SUCCESS){
        if(u8Status & ECC_STATUS_ERR)
            {
                u32Ret = ERR_SPINAND_ECC_ERROR;
            }
        else if(u8Status & ECC_STATUS_BITFLIP)
                u32Ret = ERR_SPINAND_ECC_BITFLIP;
    }

    return u32Ret;
}

//-------------------------------------------------------------------------------------------------
// Read SPINAND Data From Random column address
// @param u32_PageIdx : page index of read data in specific block
// @return TRUE : succeed
// @return FALSE : fail
// @note : If Enable ISP engine, the XIU mode does not work
//-------------------------------------------------------------------------------------------------
U32 MDrv_SPINAND_Read_RandomIn(U32 u32_PageIdx, U32 u32_Column, U32 u32_Byte, U8 *u8Data)
{
    return HAL_SPINAND_Read_RandomIn(u32_PageIdx, u32_Column, u32_Byte, u8Data);
}

U32 MDrv_SPINAND_ReadStatusRegister(MS_U8 *u8Status, MS_U8 u8Addr)
{
    return HAL_SPINAND_ReadStatusRegister(u8Status, u8Addr);
}

U32 MDrv_SPINAND_WriteStatusRegister(MS_U8 *u8Status, MS_U8 u8Addr)
{
    return HAL_SPINAND_WriteStatusRegister(u8Status, u8Addr);
}

U32 MDrv_SPINAND_SetMode(SPINAND_MODE eMode)
{
    return HAL_SPINAND_SetMode(eMode);
}

U32 MDrv_SPINAND_Write(U32 u32_PageIdx, U8 *u8Data, U8 *pu8_SpareBuf)
{
    return HAL_SPINAND_Write(u32_PageIdx, u8Data, pu8_SpareBuf);
}

U8 MDrv_SPINAND_ReadID(U16 u16Size, U8 *u8Data)
{
   return HAL_SPINAND_ReadID(u16Size, u8Data);
}

U32 MDrv_SPINAND_BLOCK_ERASE(U32 u32_PageIdx)
{
    return HAL_SPINAND_BLOCKERASE(u32_PageIdx);
}

void SpiNandMain(unsigned int dwSramAddress, unsigned int dwSramSize)
{
    U8 u8Data[2];
    MDrv_SPINAND_ReadID(2,u8Data);
}
