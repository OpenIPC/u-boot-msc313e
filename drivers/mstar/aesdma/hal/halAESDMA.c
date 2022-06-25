/*
 *#############################################################################
 *
 * Copyright (c) 2006-2011 MStar Semiconductor, Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MStar Semiconductor Inc. and be kept in strict confidence
 * (��MStar Confidential Information��) by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MStar Confidential
 * Information is unlawful and strictly prohibited. MStar hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 *#############################################################################
 */

#ifndef _HAL_AESDMA_H_
#include "halAESDMA.h"
#endif

void HAL_AESDMA_SetXIULength(U32 u32Size)
{
    // AESDMA XIU length (byte):54~55
    RIU[(AESDMA_BASE_ADDR+(0x54<<1))]= (U16)((0x0000ffff)&(u32Size));
    RIU[(AESDMA_BASE_ADDR+(0x55<<1))]= (U16)(((0xffff0000)&(u32Size))>>16);
}

void HAL_AESDMA_UseHwKey(void)
{
    // AESDMA using HW key
//    RIU[(AESDMA_BASE_ADDR+(0x5e<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x5e<<1))])|AESDMA_CTRL_USE_SECRET_KEY);
    RIU[(AESDMA_BASE_ADDR+(0x79<<1))]= (((RIU[(AESDMA_BASE_ADDR+(0x79<<1))]) & (~AESDMA_USE_SECRET_KEY_MASK)) | AESDMA_USE_HW_KEY);
}

void HAL_AESDMA_UseEfuseKey(void)
{
    // AESDMA using HW key
    RIU[(AESDMA_BASE_ADDR+(0x79<<1))]= (((RIU[(AESDMA_BASE_ADDR+(0x79<<1))]) & (~AESDMA_USE_SECRET_KEY_MASK)) | AESDMA_USE_EFUSE_KEY);
}

void HAL_AESDMA_UseCipherKey(void)
{
    // AESDMA using HW key
    RIU[(AESDMA_BASE_ADDR+(0x79<<1))]= (((RIU[(AESDMA_BASE_ADDR+(0x79<<1))]) & (~AESDMA_USE_SECRET_KEY_MASK)) | AESDMA_USE_CIPHER_KEY);
}

void HAL_AESDMA_CipherEncrypt(void)
{
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x51<<1))]) & (~AESDMA_CTRL_CIPHER_DECRYPT));
}
void HAL_AESDMA_CipherDecrypt(void)
{
    // AESDMA ctrl(decrypt/aes_en)
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x51<<1))])|(AESDMA_CTRL_CIPHER_DECRYPT));
}

void HAL_AESDMA_Enable(void)
{
    // AESDMA ctrl(decrypt/aes_en)
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x51<<1))]) | AESDMA_CTRL_AES_EN);
}

void HAL_AESDMA_Disable(void)
{
    // AESDMA ctrl(decrypt/aes_en)
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x51<<1))])&(~AESDMA_CTRL_AES_EN));
}

void HAL_AESDMA_FileOutEnable(U8 u8FileOutEnable)
{
    // AESDMA fout_en
    if(u8FileOutEnable==1)
    {
        RIU[(AESDMA_BASE_ADDR+(0x50<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x50<<1))]) | AESDMA_CTRL_FOUT_EN);
    }
    else
    {
        RIU[(AESDMA_BASE_ADDR+(0x50<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x50<<1))])&(~AESDMA_CTRL_FOUT_EN));
    }
}

void HAL_AESDMA_SetFileinAddr(U32 u32addr)
{
    u32addr = u32addr & (~0xF0000000); //mask bit to avoid MIU/IMI wrap

    //SHA_SetLength:5c~5d(sha_message_length)
    RIU[(AESDMA_BASE_ADDR+(0x52<<1))]= (U16)((0x0000ffff)&(u32addr));
    RIU[(AESDMA_BASE_ADDR+(0x53<<1))]= (U16)(((0xffff0000)&(u32addr))>>16);
}

void HAL_AESDMA_SetFileoutAddr(U32 u32addr, U32 u32Size)
{
    u32addr = u32addr & (~0xF0000000); //mask bit to avoid MIU/IMI wrap

    //SHA_SetLength:5c~5d(sha_message_length)
    RIU[(AESDMA_BASE_ADDR+(0x56<<1))]= (U16)((0x0000ffff)&(u32addr));
    RIU[(AESDMA_BASE_ADDR+(0x57<<1))]= (U16)(((0xffff0000)&(u32addr))>>16);

    RIU[(AESDMA_BASE_ADDR+(0x58<<1))]= (U16)((0x0000ffff)&(u32addr+u32Size-1));
    RIU[(AESDMA_BASE_ADDR+(0x59<<1))]= (U16)(((0xffff0000)&(u32addr+u32Size-1))>>16);
}

void HAL_AESDMA_SetCipherKey(U16 *pu16Key)
{
    int i;

    for(i=0;i<8;i++)
    {
        RIU[(AESDMA_BASE_ADDR+((0x67-i)<<1))] =((pu16Key[i] & 0x00FF)<<8)|((pu16Key[i] & 0xFF00)>>8);
    }
}

void HAL_AESDMA_SetIV(U16 *pu16IV)
{
    int i;

    for(i=0;i<8;i++)
    {
        RIU[(AESDMA_BASE_ADDR+((0x6F-i)<<1))] = ((pu16IV[i] & 0x00FF)<<8)|((pu16IV[i] & 0xFF00)>>8);
    }
}

void HAL_AESDMA_SetChainModeECB(void)
{
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))] = (RIU[(AESDMA_BASE_ADDR+(0x51<<1))] & AESDMA_CTRL_CHAINMODE_CLEAR) | AESDMA_CTRL_CHAINMODE_ECB;
}

void HAL_AESDMA_SetChainModeCTR(void)
{
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))] = (RIU[(AESDMA_BASE_ADDR+(0x51<<1))] & AESDMA_CTRL_CHAINMODE_CLEAR) | AESDMA_CTRL_CHAINMODE_CTR;
}

void HAL_AESDMA_SetChainModeCBC(void)
{
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))] = (RIU[(AESDMA_BASE_ADDR+(0x51<<1))] & AESDMA_CTRL_CHAINMODE_CLEAR) | AESDMA_CTRL_CHAINMODE_CBC;
}

void HAL_AESDMA_Reset(void)
{
    int i;
    RIU[(AESDMA_BASE_ADDR+(0x50<<1))] = AESDMA_CTRL_SW_RST;
    RIU[(AESDMA_BASE_ADDR+(0x50<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x79<<1))]= 0;
    RIU[(AESDMA_BASE_ADDR+(0x51<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x52<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x53<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x54<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x55<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x56<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x57<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x58<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x59<<1))] = 0;
    RIU[(AESDMA_BASE_ADDR+(0x79<<1))] = (((RIU[(AESDMA_BASE_ADDR+(0x79<<1))]) & (~AESDMA_USE_SECRET_KEY_MASK)) | AESDMA_USE_CIPHER_KEY);
    for(i=0;i<8;i++)
    {
        RIU[(AESDMA_BASE_ADDR+((0x67-i)<<1))] = 0;
    }
    for(i=0;i<8;i++)
    {
        RIU[(AESDMA_BASE_ADDR+((0x6F-i)<<1))] = 0;
    }
}

void HAL_AESDMA_Start(U8 u8AESDMAStart)
{
    // AESDMA file start
    if(u8AESDMAStart==1)
    {
        RIU[(AESDMA_BASE_ADDR+(0x50<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x50<<1))])|(AESDMA_CTRL_FILE_ST));
        RIU[(AESDMA_BASE_ADDR+(0x50<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x50<<1))])&(~AESDMA_CTRL_FILE_ST));
    }
    else
    {
        RIU[(AESDMA_BASE_ADDR+(0x50<<1))]= ((RIU[(AESDMA_BASE_ADDR+(0x50<<1))])&(~AESDMA_CTRL_FILE_ST));
    }
}

U16 HAL_AESDMA_GetStatus(void)
{
    return RIU[(AESDMA_BASE_ADDR+(0x7F<<1))];
}

void HAL_RSA_ClearInt(void)
{
    //RSA interrupt clear
    RIU[(RSA_BASE_ADDR+(0x27<<1))]= ((RIU[(RSA_BASE_ADDR+(0x27<<1))])|(RSA_INT_CLR));
}

void HAL_RSA_Reset(void)
{
    //RSA Rst
    RIU[(RSA_BASE_ADDR+(0x28<<1))]= ((RIU[(RSA_BASE_ADDR+(0x28<<1))])|(RSA_CTRL_RSA_RST));
    RIU[(RSA_BASE_ADDR+(0x28<<1))]= ((RIU[(RSA_BASE_ADDR+(0x28<<1))])&(~RSA_CTRL_RSA_RST));
}

void HAL_RSA_Ind32Ctrl(U8 u8dirction)
{
    //[1] reg_ind32_direction 0: Read. 1: Write
    if(u8dirction==1)
    {
        RIU[(RSA_BASE_ADDR+(0x21<<1))]= ((RIU[(RSA_BASE_ADDR+(0x21<<1))])|(RSA_IND32_CTRL_DIRECTION_WRITE));
    }
    else
    {
        RIU[(RSA_BASE_ADDR+(0x21<<1))]= ((RIU[(RSA_BASE_ADDR+(0x21<<1))])&(~RSA_IND32_CTRL_DIRECTION_WRITE));
    }
    //[2] reg_addr_auto_inc : Set 1 to enable address auto-increment after finishing read/write
    RIU[(RSA_BASE_ADDR+(0x21<<1))]= ((RIU[(RSA_BASE_ADDR+(0x21<<1))])|(RSA_IND32_CTRL_ADDR_AUTO_INC));

    //[3] Set 1 to enable access auto-start after writing Data[31:16]
    RIU[(RSA_BASE_ADDR+(0x21<<1))]= ((RIU[(RSA_BASE_ADDR+(0x21<<1))])|(RSA_IND32_CTRL_ACCESS_AUTO_START));
}

void HAL_RSA_LoadSignInverse(U32 *ptr_Sign)
{

    U32 i;

    RIU[(RSA_BASE_ADDR+(0x22<<1))]= RSA_A_BASE_ADDR; //RSA A addr
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])|(RSA_IND32_START)); //RSA start

    for( i = 0; i < 64; i++ )
    {
        RIU[(RSA_BASE_ADDR+(0x23<<1))]= (U16)(((*(ptr_Sign+63-i))>>8)&0xFF00)|(((*(ptr_Sign+63-i))>>24)&0xFF);
        RIU[(RSA_BASE_ADDR+(0x24<<1))]= (U16)(((*(ptr_Sign+63-i))>>8)&0xFF)|(((*(ptr_Sign+63-i))<<8)&0xFF00);
    }
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])&(~RSA_IND32_START)); //RSA stop
}
void HAL_RSA_LoadSignInverse_2byte(U16 *ptr_Sign)
{
    S32 i;
    RIU[(RSA_BASE_ADDR+(0x22<<1))]= RSA_A_BASE_ADDR; //RSA A addr
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])|(RSA_IND32_START)); //RSA start
    for( i = 127; i >= 0; i -= 2 )
    {
        RIU[(RSA_BASE_ADDR+(0x23<<1))]= (U16)( ((*(ptr_Sign+i) << 8) & 0xFF00) | ((*(ptr_Sign+i) >> 8) & 0xFF) );
        RIU[(RSA_BASE_ADDR+(0x24<<1))]= (U16)( ((*(ptr_Sign+i-1) << 8) & 0xFF00) | ((*(ptr_Sign+i-1) >> 8) & 0xFF) );
    }
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])&(~RSA_IND32_START)); //RSA stop
}

void HAL_RSA_LoadKeyE(U32 *ptr_E)
{
    U32 i;

    RIU[(RSA_BASE_ADDR+(0x22<<1))]= RSA_E_BASE_ADDR; //RSA E addr
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])|(RSA_IND32_START)); //RSA start

    //RIU[(POR_STATUS_BASE_ADDR+(0xA<<1))]=(U16)((0x0000ffff)&(U32)(ptr_E)); //write ptr_E addr to por_status(0x10050A)

    for( i = 0; i < 64; i++ )
    {
        RIU[(RSA_BASE_ADDR+(0x23<<1))]= (U16)(((*(ptr_E+i))>>8)&0xFF00)|(((*(ptr_E+i))>>24)&0xFF);
        RIU[(RSA_BASE_ADDR+(0x24<<1))]= (U16)(((*(ptr_E+i))>>8)&0xFF)|(((*(ptr_E+i))<<8)&0xFF00);
    }
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])&(~RSA_IND32_START)); //RSA stop
}

void HAL_RSA_LoadKeyN(U32 *ptr_N)
{
    U32 i;

    RIU[(RSA_BASE_ADDR+(0x22<<1))]= RSA_N_BASE_ADDR; //RSA N addr
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])|(RSA_IND32_START)); //RSA start

    //RIU[(POR_STATUS_BASE_ADDR+(0xB<<1))]=(U16)((0x0000ffff)&(U32)(ptr_N)); //write ptr_N addr to por_status(0x10050B)

    for( i = 0; i < 64; i++ )
    {
        RIU[(RSA_BASE_ADDR+(0x23<<1))]= (U16)(((*(ptr_N+i))>>8)&0xFF00)|(((*(ptr_N+i))>>24)&0xFF);
        RIU[(RSA_BASE_ADDR+(0x24<<1))]= (U16)(((*(ptr_N+i))>>8)&0xFF)|(((*(ptr_N+i))<<8)&0xFF00);
    }
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])&(~RSA_IND32_START)); //RSA stop
}

void HAL_RSA_LoadKeyNInverse(U32 *ptr_N)
{
    U32 i;

    RIU[(RSA_BASE_ADDR+(0x22<<1))]= RSA_N_BASE_ADDR; //RSA N addr
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])|(RSA_IND32_START)); //RSA start

    //RIU[(POR_STATUS_BASE_ADDR+(0xB<<1))]=(U16)((0x0000ffff)&(U32)(ptr_N)); //write ptr_N addr to por_status(0x10050B)

    for( i = 0; i < 64; i++ )
    {
        RIU[(RSA_BASE_ADDR+(0x23<<1))]= (U16)(((*(ptr_N+63-i))>>8)&0xFF00)|(((*(ptr_N+63-i))>>24)&0xFF);
        RIU[(RSA_BASE_ADDR+(0x24<<1))]= (U16)(((*(ptr_N+63-i))>>8)&0xFF)|(((*(ptr_N+63-i))<<8)&0xFF00);
    }
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])&(~RSA_IND32_START)); //RSA stop
}

void HAL_RSA_SetKeyLength(U16 u16keylen)
{
    //[13:8] n_len_e: key length, if hardware key set, this register is ignored and hardware internal using 3f
    RIU[(RSA_BASE_ADDR+(0x28<<1))] = ((RIU[(RSA_BASE_ADDR+(0x28<<1))])|(u16keylen<<8));
}

void HAL_RSA_SetKeyType(U8 u8hwkey, U8 u8pubkey)
{
    //[1] hw_key_e : 0 : software key, 1: hardware key
    //[2] e_pub_e : 0: pvivate key, 1: public key
    if(u8hwkey==1)
    {
        RIU[(RSA_BASE_ADDR+(0x28<<1))] = ((RIU[(RSA_BASE_ADDR+(0x28<<1))])|(RSA_SEL_HW_KEY));
    }
    else
    {
        RIU[(RSA_BASE_ADDR+(0x28<<1))] = ((RIU[(RSA_BASE_ADDR+(0x28<<1))])&(~RSA_SEL_HW_KEY));
    }

    if(u8pubkey==1)
    {
        RIU[(RSA_BASE_ADDR+(0x28<<1))] = ((RIU[(RSA_BASE_ADDR+(0x28<<1))])|(RSA_SEL_PUBLIC_KEY));
    }
    else
    {
        RIU[(RSA_BASE_ADDR+(0x28<<1))] = ((RIU[(RSA_BASE_ADDR+(0x28<<1))])&(~RSA_SEL_PUBLIC_KEY));
    }

}

void HAL_RSA_ExponetialStart(void)
{
    //RSA exp start
    RIU[(RSA_BASE_ADDR+(0x27<<1))] = ((RIU[(RSA_BASE_ADDR+(0x27<<1))])|(RSA_EXP_START));
}

U16 HAL_RSA_GetStatus(void)
{
    return RIU[(RSA_BASE_ADDR+(0x29<<1))];
}

void HAL_RSA_FileOutStart(void)
{
    //RSA ind32_start
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])|(RSA_IND32_START));
}

void HAL_RSA_FileOutEnd(void)
{
    RIU[(RSA_BASE_ADDR+(0x20<<1))]= ((RIU[(RSA_BASE_ADDR+(0x20<<1))])&(~RSA_IND32_START));
}
void HAL_RSA_SetFileOutAddr(U32 u32offset)
{
    //RSA ind32_addr
    RIU[(RSA_BASE_ADDR+(0x22<<1))] = (U16)(RSA_Z_BASE_ADDR + u32offset);
}

U32 HAL_RSA_FileOut(void)
{
    return (U32)(RIU[(RSA_BASE_ADDR+(0x25<<1))]|(RIU[(RSA_BASE_ADDR+(0x26<<1))]<<16));
}

void HAL_SHA_Reset(void)
{
    //SHA_Reset
    RIU[SHARNG_BASE_ADDR+(0x08<<1)]= (RIU[SHARNG_BASE_ADDR+(0x08<<1)]|(SHARNG_CTRL_SHA_RST));
    RIU[SHARNG_BASE_ADDR+(0x08<<1)]= (RIU[SHARNG_BASE_ADDR+(0x08<<1)]&(~SHARNG_CTRL_SHA_RST));
    RIU[SHARNG_BASE_ADDR+(0x5d<<1)]= 0;
}

void HAL_SHA_SetAddress(U32 u32Address)
{
    u32Address = u32Address & (~0xF0000000); //mask bit to avoid MIU/IMI wrap
    //SHA_SetLength:5c~5d(sha_message_length)
    RIU[(SHARNG_BASE_ADDR+(0x0A<<1))]= (U16)((0x0000ffff)&(u32Address));
    RIU[(SHARNG_BASE_ADDR+(0x0B<<1))]= (U16)(((0xffff0000)&(u32Address))>>16);

    // Bypass scatter & gather address
    RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])|(1<<11));
}

void HAL_SHA_SetLength(U32 u32Size)
{
    //SHA_SetLength:5c~5d(sha_message_length)
    RIU[(SHARNG_BASE_ADDR+(0x0C<<1))]= (U16)((0x0000ffff)&(u32Size));
    RIU[(SHARNG_BASE_ADDR+(0x0D<<1))]= (U16)(((0xffff0000)&(u32Size))>>16);
}

void HAL_SHA_SelMode(U8 u8sha256)
{
    //SHA_SelMode:58~59(sha_ctrl & sha_scattergather_size)
    if(u8sha256==1)
    {
        RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])|(SHARNG_CTRL_SHA_SEL_SHA256));
    }
    else
    {
        RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])&(~SHARNG_CTRL_SHA_SEL_SHA256));
    }
}

U16 HAL_SHA_GetStatus(void)
{
    return (RIU[(SHARNG_BASE_ADDR+(0x0F<<1))]);
}

void HAL_SHA_Clear(void)
{
    //Set "1" to idle state after reg_read_sha_ready = 1
    RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])&(~SHARNG_CTRL_SHA_CLR));
    RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])|(SHARNG_CTRL_SHA_CLR));
}

void HAL_SHA_Start(void)
{
    RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])|(SHARNG_CTRL_SHA_FIRE_ONCE));
    RIU[(SHARNG_BASE_ADDR+(0x08<<1))]= ((RIU[(SHARNG_BASE_ADDR+(0x08<<1))])&(~SHARNG_CTRL_SHA_FIRE_ONCE));
}

void HAL_SHA_Out(U32 u32Buf)
{
    U32 index;

    //SHA_Out
    for( index = 0; index < 16; index++ )
    {
        *((U16 *)u32Buf + index) = (RIU[(SHARNG_BASE_ADDR+(0x10<<1)+index*2)]);
    }
}

