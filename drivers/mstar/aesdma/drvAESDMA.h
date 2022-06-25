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

#ifndef _DRV_AESDMA_H_
#define _DRV_AESDMA_H_

#include "hal/halAESDMA.h"

typedef enum
{
    E_AESDMA_KEY_CIPHER,
    E_AESDMA_KEY_EFUSE,
    E_AESDMA_KEY_HW,
    E_AESDMA_KEY_NUM
} enumAESDMA_KeyType;

typedef enum
{
    E_AESDMA_CHAINMODE_ECB,
    E_AESDMA_CHAINMODE_CTR,
    E_AESDMA_CHAINMODE_CBC,
    E_AESDMA_CHAINMODE_NUM
} enumAESDMA_ChainMode;

typedef struct
{
    U32 u32SrcAddr;
    U32 u32Size;
    U32 u32DstAddr;
    enumAESDMA_KeyType eKeyType;
    U16 *pu16Key;
    BOOL bSetIV;
    BOOL bDecrypt;
    U16 *pu16IV;
    enumAESDMA_ChainMode eChainMode;
} aesdmaConfig;

typedef enum
{
    E_SHA_MODE_1,
    E_SHA_MODE_256,
    E_SHA_MODE_NUM
} enumShaMode;

typedef struct
{
    U32 *pu32Sig;
    U32 *pu32KeyN;
    U32 *pu32KeyE;
    U32 *pu32Output;
    BOOL bHwKey;
    BOOL bPublicKey;
    U32 u32KeyLen;
} rsaConfig;

void MDrv_AESDMA_Run(aesdmaConfig *pConfig);
void MDrv_SHA_Run(U32 u32SrcAddr, U32 u32Size, enumShaMode eMode, U16 *pu16Output);
void MDrv_RSA_Run(rsaConfig *pConfig);
#endif

