/*
 *#############################################################################
 *
 * Copyright (c) 2006-2011 MStar Semiconductor, Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MStar Semiconductor Inc. and be kept in strict confidence
 * (¡§MStar Confidential Information¡¨) by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MStar Confidential
 * Information is unlawful and strictly prohibited. MStar hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 *#############################################################################
 */

#ifndef _SPINAND_H_
#define _SPINAND_H_

#include "drvSPINAND.h"
#define printk	printf

#if 0
#define spi_nand_msg(fmt, ...) printk(KERN_NOTICE "SPINAND: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#else
#define spi_nand_msg(fmt, ...)
#endif

#if 0
#define spi_nand_debug(fmt, ...) printk(KERN_NOTICE "SPINAND: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#else
#define spi_nand_debug(fmt, ...)
#endif

#define spi_nand_warn(fmt, ...) printk(KERN_WARNING "SPINAND: %s:warning, " fmt "\n", __func__, ##__VA_ARGS__)
#define spi_nand_err(fmt, ...) printk(KERN_ERR "SPINAND: %s:error, " fmt "\n", __func__, ##__VA_ARGS__)

#define UNFD_LOGI_PART    0x8000 // bit-or if the partition needs Wear-Leveling
#define UNFD_HIDDEN_PART  0x4000 // bit-or if this partition is hidden, normally it is set for the LOGI PARTs.

//#define UNFD_PART_MIU			1
#define UNFD_PART_IPL_CUST		1
#define UNFD_PART_BOOTLOGO		2
#define UNFD_PART_IPL			3
#define UNFD_PART_OS			4
#define UNFD_PART_CUS			5
#define UNFD_PART_UBOOT			6
#define UNFD_PART_SECINFO		7
#define UNFD_PART_OTP			8
#define UNFD_PART_RECOVERY		9
#define UNFD_PART_E2PBAK		10
#define UNFD_PART_NVRAMBAK		11
#define UNFD_PART_NPT			12
#define UNFD_PART_ENV			13
#define UNFD_PART_MISC			14

typedef struct
{
    U16 u16_StartBlk;		// the start block index
    U16 u16_BlkCnt;			// project team defined
    U16 u16_PartType;		// project team defined
    U16 u16_BackupBlkCnt;	// reserved good blocks count for backup, UNFD internal use.
    // e.g. u16BackupBlkCnt  = u16BlkCnt * 0.03 + 2
} PARTITION_RECORD_t;

typedef struct
{
    U32 u32_ChkSum;
    U16	u16_SpareByteCnt;
    U16	u16_PageByteCnt;
    U16	u16_BlkPageCnt;
    U16	u16_BlkCnt;
    U16 u16_PartCnt;
    U16 u16_UnitByteCnt;
    PARTITION_RECORD_t records[62];

} PARTITION_INFO_t;

typedef struct _SPI_NAND_DRIVER
{
    SPINAND_FLASH_INFO_t tSpinandInfo;

    PARTITION_INFO_t tPartInfo;
    U8 u8_HasPNI;

    U8 *pu8_pagebuf;
    U8 *pu8_sparebuf;
    U8 *pu8_statusbuf;
    U8 u8_statusRequest;
    U32 u32_column;
    U8 u8_status;
    U8  u8_SectorByteCntBits;
    U8  u8_PageSectorCntBits;
    U8  u8_PageByteCntBits;
    U8  u8_BlkPageCntBits;
    U16 u16_PageSectorCnt;
} SPI_NAND_DRIVER_t;

typedef struct
{
    U8  au8_Tag[16];
    SPINAND_FLASH_INFO_t tSpiNandInfo;
//	    U8 au8_Reserved[];

} SPINAND_FLASH_INFO_TAG_t;

#define SPINAND_FLASH_INFO_TAG "MSTARSEMIUSFDCIS"

// this definition is used to measure nand read/write speed
#define NAND_SPEED_TEST                     0
void *drvSPINAND_get_DrvContext_address(void);
extern void dump_mem(unsigned char *buf, int cnt);

#endif
