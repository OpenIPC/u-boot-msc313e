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
#include "drvAESDMA.h"
#endif

void MDrv_AESDMA_Run(aesdmaConfig *pConfig)
{
    HAL_AESDMA_Reset();

    HAL_AESDMA_SetFileinAddr(pConfig->u32SrcAddr);
    HAL_AESDMA_SetXIULength(pConfig->u32Size);
    HAL_AESDMA_SetFileoutAddr(pConfig->u32DstAddr, pConfig->u32Size);

    switch(pConfig->eKeyType)
    {
    case E_AESDMA_KEY_CIPHER:
        HAL_AESDMA_UseCipherKey();
        HAL_AESDMA_SetCipherKey(pConfig->pu16Key);
        break;
    case E_AESDMA_KEY_EFUSE:
        HAL_AESDMA_UseEfuseKey();
        break;
    case E_AESDMA_KEY_HW:
        HAL_AESDMA_UseHwKey();
        break;
    default:
        return;
    }

    if(pConfig->bDecrypt)
    {
        HAL_AESDMA_CipherDecrypt();
    }
    else
    {
        HAL_AESDMA_CipherEncrypt();
    }      

    if(pConfig->bSetIV)
    {
        HAL_AESDMA_SetIV(pConfig->pu16IV);
    }

    HAL_AESDMA_Enable();

    switch(pConfig->eChainMode)
    {
    case E_AESDMA_CHAINMODE_ECB:
        HAL_AESDMA_SetChainModeECB();
        HAL_AESDMA_SetXIULength(((pConfig->u32Size+15)/16)*16); // ECB mode size should align 16byte
        break;
    case E_AESDMA_CHAINMODE_CTR:
        HAL_AESDMA_SetChainModeCTR();
        HAL_AESDMA_CipherEncrypt();  // CTR mode can't set cipher_decrypt bit
        break;
    case E_AESDMA_CHAINMODE_CBC:
        HAL_AESDMA_SetChainModeCBC();
        break;
    default:
        return;
    }

    HAL_AESDMA_FileOutEnable(1);

    HAL_AESDMA_Start(1);

    // Wait for ready.
    while((HAL_AESDMA_GetStatus() & AESDMA_CTRL_DMA_DONE) != AESDMA_CTRL_DMA_DONE);
    HAL_AESDMA_Reset();

}

void MDrv_SHA_Run(U32 u32SrcAddr, U32 u32Size, enumShaMode eMode, U16 *pu16Output)
{
    HAL_SHA_Reset();

    HAL_SHA_SetAddress(u32SrcAddr);
    HAL_SHA_SetLength(u32Size);

    switch(eMode)
    {
    case E_SHA_MODE_1:
        HAL_SHA_SelMode(0);
        break;
    case E_SHA_MODE_256:
        HAL_SHA_SelMode(1);
        break;
    default:
        return;
    }

    HAL_SHA_Start();

    // Wait for the SHA done.
    while((HAL_SHA_GetStatus() & SHARNG_CTRL_SHA_READY) != SHARNG_CTRL_SHA_READY);

    HAL_SHA_Out((U32)pu16Output);

    HAL_SHA_Clear();
    HAL_SHA_Reset();
}

void MDrv_RSA_Run(rsaConfig *pConfig)
{
    int nOutSize;
    int i;

    HAL_RSA_Reset();

    HAL_RSA_SetKeyLength((pConfig->u32KeyLen-1) & 0x3F);
    HAL_RSA_SetKeyType(pConfig->bHwKey, pConfig->bPublicKey);

    HAL_RSA_Ind32Ctrl(0);
    HAL_RSA_Ind32Ctrl(1);
    HAL_RSA_LoadSignInverse_2byte((U16*)pConfig->pu32Sig);
    if((!pConfig->bHwKey) && (pConfig->pu32KeyN))
    {
        HAL_RSA_LoadKeyN(pConfig->pu32KeyN);
    }
    if((!pConfig->bHwKey) && (pConfig->pu32KeyE))
    {
        HAL_RSA_LoadKeyE(pConfig->pu32KeyE);
    }

    HAL_RSA_ExponetialStart();

    while((HAL_RSA_GetStatus() & RSA_STATUS_RSA_DONE) != RSA_STATUS_RSA_DONE);

    if((pConfig->bHwKey) || (pConfig->u32KeyLen == 2048))
    {
        nOutSize = (2048/8)/4;
    }
    else
    {
        nOutSize = (1024/8)/4;
    }

    HAL_RSA_Ind32Ctrl(0);
    for( i = 0; i<nOutSize; i++)
    {
        HAL_RSA_SetFileOutAddr(i);
        HAL_RSA_FileOutStart();
        *(pConfig->pu32Output+i) = HAL_RSA_FileOut();
    }
    HAL_RSA_FileOutEnd();
    HAL_RSA_Reset();
}

