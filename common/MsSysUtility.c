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

#ifndef __MSSYSUTILITY_C__
#define __MSSYSUTILITY_C__


#include <common.h>
#include <command.h>
#include <MsDebug.h>
#include <MsSysUtility.h>
#include "../drivers/mstar/gpio/drvGPIO.h"
#include "../arch/arm/include/asm/arch-infinity/mach/io.h"
#include "../arch/arm/include/asm/arch-infinity/mach/platform.h"


#ifndef RIUBASEADDR
#define RIUBASEADDR    (0x1F000000)
#endif


MS_U8 ReadByte(MS_U32 u32RegAddr)
{
    if(u32RegAddr & 0x01)
        return INREG8(RIUBASEADDR + ((u32RegAddr&0xFFFF00)<<1) + (((u32RegAddr-1)&0xFF)<<1) + 1);
    else
	    return INREG8(RIUBASEADDR + ((u32RegAddr&0xFFFFFF)<<1));
}

MS_U16 Read2Byte(MS_U32 u32RegAddr)
{
    MS_U16 u16Value = 0;

    if(u32RegAddr & 0x01)
    {
	    u16Value = INREG8(RIUBASEADDR + ((u32RegAddr&0xFFFF00)<<1) + (((u32RegAddr-1)&0xFF)<<1) + 1);
	    u16Value = (u16Value&0xFF) << 8;
	    u16Value += INREG8(RIUBASEADDR + ((u32RegAddr&0xFFFF00)<<1) + (((u32RegAddr-1)&0xFF)<<1));
    }
    else
    {
	    u16Value = INREG8(RIUBASEADDR + ((u32RegAddr&0xFFFFFF)<<1) + 1);
	    u16Value = (u16Value&0xFF) << 8;
	    u16Value += INREG8(RIUBASEADDR + ((u32RegAddr&0xFFFFFF)<<1));
    }

    return u16Value;
}

MS_BOOL WriteByte(MS_U32 u32RegAddr, MS_U8 u8Val)
{
    if(u32RegAddr & 0x01)
        OUTREG8(RIUBASEADDR + ((u32RegAddr&0xFFFF00)<<1) + (((u32RegAddr-1)&0xFF)<<1) + 1, u8Val);
    else
	    OUTREG8(RIUBASEADDR + ((u32RegAddr&0xFFFFFF)<<1), u8Val);

    return TRUE;
}

MS_BOOL Write2Byte(MS_U32 u32RegAddr, MS_U16 u16Val)
{
    if(u32RegAddr & 0x01)
    {
	    OUTREG8(RIUBASEADDR + ((u32RegAddr&0xFFFF00)<<1) + (((u32RegAddr-1)&0xFF)<<1) + 1, (u16Val>>8)&0xFF);
	    OUTREG8(RIUBASEADDR + ((u32RegAddr&0xFFFF00)<<1) + (((u32RegAddr-1)&0xFF)<<1), u16Val&0xFF);
    }
    else
	{
	    OUTREG8(RIUBASEADDR + ((u32RegAddr&0xFFFFFF)<<1) + 1, (u16Val>>8)&0xFF);
	    OUTREG8(RIUBASEADDR + ((u32RegAddr&0xFFFFFF)<<1), u16Val&0xFF);
    }

    return TRUE;
}

MS_BOOL WriteRegBit(MS_U32 u32RegAddr, MS_U16 u16Bit, MS_BOOL bEnable)
{
    MS_U16 u16Value = 0;

	u16Value = Read2Byte(u32RegAddr);
	u16Value = (bEnable) ? (u16Value | u16Bit) : (u16Value & ~u16Bit);
	Write2Byte(u32RegAddr, u16Value);

    return TRUE;
}

MS_BOOL WriteRegBitPos(MS_U32 u32RegAddr, MS_U8 u8Bit, MS_BOOL bEnable)
{
    MS_U16 u16Bit;
    
    if(u8Bit > 15)
    {
        UBOOT_ERROR("Over the bank boundary!\n");
        return FALSE;
    }

    u16Bit = (1 << u8Bit);
    WriteRegBit( u32RegAddr,  u16Bit,  bEnable);

    return TRUE;
}


int do_riu(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MS_U32 u32Target = 0;
    MS_U32 u32Value  = 0;
    MS_U8  u8Bit = 0;
	MS_U8  u8Len = 0;
	MS_U8  i = 0;
    
    if (argc < 3)
    {
        cmd_usage(cmdtp);
        return 0;
    }
     
    if(0 == strncmp(argv[1], "wword", 2))
    {
        u32Target = simple_strtoul(argv[2], NULL, 16);
        u32Value = simple_strtoul(argv[3], NULL, 16);
        Write2Byte(u32Target, (MS_U16)u32Value);
        u32Target = simple_strtoul(argv[2], NULL, 16);
        UBOOT_INFO("[%x]:0x%04x\n", (unsigned int)u32Target, (unsigned int)Read2Byte(u32Target));
    }
    else if(0 == strncmp(argv[1], "rword", 2))
    {
        u32Target = simple_strtoul(argv[2], NULL, 16);
		if (NULL != argv[3])
		    u8Len = simple_strtoul(argv[3], NULL, 10);
		else
			u8Len = 1;

		if (1 == u8Len)
		    UBOOT_INFO("[%x]:0x%04x\n", (unsigned int)u32Target,(unsigned int)Read2Byte(u32Target));
		else
		{
            if(u32Target & 0x01)
            {
                printf("%06x: ", (unsigned int)u32Target-1);
                u32Target = u32Target -1;
            }
            else
                printf("%06x: ", (unsigned int)u32Target);

		    for (i=1; i<=u8Len/2; i++)
		    {
		        printf("%04x ", (unsigned int)Read2Byte(u32Target+i*2-1));

				if (0x80 <= ((u32Target+i-1)&0xFF))
				{
					break;
				}

				if (0 == i%4) printf(" ");
				if (0 == i%8) printf("\n%06x: ", (unsigned int)(u32Target+i));
		    }
            printf("\n");
		}
    }
    else if(0 == strncmp(argv[1], "wbyte", 2))
    {
        u32Target = simple_strtoul(argv[2], NULL, 16);
        u32Value = simple_strtoul(argv[3], NULL, 16);
        WriteByte(u32Target, (MS_U8)u32Value);
        UBOOT_INFO("[%x]:0x%x\n", (unsigned int)u32Target, (unsigned int)ReadByte(u32Target));
    }
    else if(0 == strncmp(argv[1], "rbyte", 2))
    {
        u32Target = simple_strtoul(argv[2], NULL, 16);
		if (NULL != argv[3])
		    u8Len = simple_strtoul(argv[3], NULL, 10);
		else
			u8Len = 1;

		if (1 == u8Len)
            UBOOT_INFO("[%x]:0x%02x\n", (unsigned int)u32Target, (unsigned int)ReadByte(u32Target));
		else
		{
		    MS_U32 tmpValue = 0;
            MS_U32 tmpTarget = 0;

            if(u32Target & 0x01)
            {
                printf("%06x: ", (unsigned int)u32Target-1);
                u32Target = u32Target -1;
            }
            else
		        printf("%06x: ", (unsigned int)u32Target);

            for (i=1; i<=u8Len/2; i++)
			{
				tmpTarget = u32Target + i*2 -1;
                tmpValue = ReadByte(tmpTarget);
                tmpValue = tmpValue << 8;
                tmpValue += ReadByte(tmpTarget-1);
                printf("%04x ", (unsigned int)tmpValue);

				if (0xFF == (tmpTarget&0xFF))
				{
					break;
				}

				if (0 == i%4) printf(" ");
				if (0 == i%8) printf("\n%06x: ", (unsigned int)(u32Target+i*2));
		    }
            printf("\n");
		}
    }
    else if(0 == strncmp(argv[1], "bit", 2))
    {
        u32Target = simple_strtoul(argv[2], NULL, 16);
        u8Bit = simple_strtoul(argv[3], NULL, 10);
        u32Value = simple_strtoul(argv[4], NULL, 16);
        WriteRegBitPos(u32Target, u8Bit, (MS_U8)u32Value);
        UBOOT_INFO("[%x]:0x%02x\n", (unsigned int)u32Target, (unsigned int)ReadByte(u32Target));
    }
    else
    {
      cmd_usage(cmdtp);
    }

    return 0;
}

U_BOOT_CMD(
    riu, 5, 0, do_riu,
    "riu  - riu command\n",
    "wword [target: bank+offset Addr(8bit mode)][value]\n"
    "riu rword [target: bank+offset Addr(8bit mode)][(len: default=1)]\n"
    "riu wbyte [target: bank+offset Addr(8bit mode)][value]\n"
    "riu rbyte [target: bank+offset Addr(8bit mode)][(len: default=1)]\n"
    "riu bit   [target: bank+offset Addr(8bit mode)][bit][(1/0)]\n"
);


#ifdef CONFIG_MS_GPIO
int do_gpio( cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    UBOOT_TRACE("IN\n");
    MS_GPIO_NUM gpio_num_p1;

    if(3 > argc)
    {
        if((NULL == strcmp(argv[1], "list")) && (2 == argc))
        {
        }
        else
        {
            cmd_usage(cmdtp);
            return -1;
        }
    }

    gpio_num_p1 = (MS_GPIO_NUM)simple_strtoul(argv[2], NULL, 10);

    if((NULL == strcmp(argv[1], "get")) || (NULL == strncmp(argv[1], "inp", 3)))
    {
        MDrv_GPIO_Pad_Set(gpio_num_p1);
        mdrv_gpio_set_input(gpio_num_p1);      
        UBOOT_INFO("IN  ");
        UBOOT_INFO("pin=%d\n", mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "output", 3))
    {
        MDrv_GPIO_Pad_Set(gpio_num_p1);

        if('1' == argv[3][0])
            mdrv_gpio_set_high(gpio_num_p1);
        else if('0' == argv[3][0])
            mdrv_gpio_set_low(gpio_num_p1);

        UBOOT_INFO("gpio[%d] is %d\n", gpio_num_p1, mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "toggle", 3))
    {
        MDrv_GPIO_Pad_Set(gpio_num_p1);

        if(mdrv_gpio_get_level(gpio_num_p1)) 
        {
            mdrv_gpio_set_low(gpio_num_p1);        
        }
        else 
        {    
            mdrv_gpio_set_high(gpio_num_p1);
        }

        UBOOT_INFO("gpio[%d] is %d\n", gpio_num_p1, mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "state", 3))
    {
        if(mdrv_gpio_get_inout(gpio_num_p1)) 
        {
            UBOOT_INFO("IN  ");
        }
        else
        {
            UBOOT_INFO("OUT ");
        }
        UBOOT_INFO("pin=%d\n", mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "list", 3))
    {
        U8 i;
        U8 size;

        if(2 == argc)
        {
            size = 200;
        }
        else
        {
            size = (MS_GPIO_NUM)simple_strtoul(argv[2], NULL, 10);
        }
        
        for(i=0; i<size; i++)
        {
            UBOOT_INFO("GPIO %3d ", i);
            gpio_num_p1 = i;
        
            if(mdrv_gpio_get_inout(gpio_num_p1)) 
            {
                UBOOT_INFO("IN  ");
            }
            else
            {
                UBOOT_INFO("Out ");
            }
            UBOOT_INFO("pin=%d\n", mdrv_gpio_get_level(gpio_num_p1));

            if((i>0) && (i%10==0)) UBOOT_INFO("\n");
        }

    }
    else
    {
        cmd_usage(cmdtp);
        return -1;
    }

    UBOOT_TRACE("OK\n");    
    return 0;
}


U_BOOT_CMD(
    gpio, 4, 0, do_gpio,
    "Config gpio port",
    "(for 2nd parameter, you must type at least 3 characters)\n"
    "gpio output <gpio#> <1/0>  : ex: gpio output 69 1\n"
    "gpio input/get <gpio#>     : ex: gpio input 10  (gpio 10 set as input)\n"
    "gpio toggle <gpio#>        : ex: gpio tog 49 (toggle)\n"
    "gpio state <gpio#>         : ex: gpio sta 49 (get i/o status(direction) & pin status)\n"
    "gpio list [num_of_pins]    : ex: gpio list 10 (list GPIO1~GPIO10 status)\n"
    );

#endif  //#ifdef CONFIG_MS_GPIO

#endif //#define __MSSYSUTILITY_C__
