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
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DRVGPIO_C__
#define __DRVGPIO_C__

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <common.h>
#include <command.h>
#include <MsDebug.h>
#include "drvGPIO.h"
#include "mhal_gpio.h"


//-------------------------------------------------------------------------------------------------
//  Local Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

 
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static const struct gpio_operations __gpio =
{
     .set_high  = MHal_GPIO_Set_High,
     .set_low   = MHal_GPIO_Set_Low,
     .set_input = MHal_GPIO_Set_Input,
     .set_output = MHal_GPIO_Set_Output,
     .get_inout = MHal_GPIO_Get_InOut,
     .get_level = MHal_GPIO_Pad_Level,
     
};

static const struct gpio_operations *gpio_op = &__gpio;

static GPIO_DbgLv _geDbfLevel = E_GPIO_DBGLV_NONE;


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
#define MS_DEBUG_MSG(x)             x

#define GPIO_DBG_FUNC()             if (_geDbfLevel >= E_GPIO_DBGLV_ALL) \
                                        {MS_DEBUG_MSG(printf("\t====   %s   ====\n", __FUNCTION__);)}

#define GPIO_DBG_INFO(x, args...)   if (_geDbfLevel >= E_GPIO_DBGLV_INFO ) \
                                        {MS_DEBUG_MSG(printf(x, ##args);)}

#define GPIO_DBG_ERR(x, args...)    if (_geDbfLevel >= E_GPIO_DBGLV_ERR_ONLY) \
                                        {MS_DEBUG_MSG(printf(x, ##args);)}

#ifndef MS_ASSERT
#define MS_ASSERT(expr)             do {                                                        \
                                        if(!(expr))                                             \
                                            printf("MVD assert fail %s %d!\n", __FILE__, __LINE__); \
                                    } while(0)
#endif


//------------------------------------------------------------------------------
//  Local Functions
//------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// select one pad to set
/// @param  gpio              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pad_Set(MS_GPIO_NUM gpio)
{
    MHal_GPIO_Pad_Set(gpio);
}

void mdrv_gpio_set_high(MS_GPIO_NUM gpio)
{
    GPIO_DBG_FUNC();

    MS_ASSERT(gpio_op);
    if (gpio_op)
    {
        gpio_op->set_high(gpio);
    }
}

void mdrv_gpio_set_low(MS_GPIO_NUM gpio)
{
    GPIO_DBG_FUNC();

    MS_ASSERT(gpio_op);
    if (gpio_op)
    {
        gpio_op->set_low(gpio);
    }
}

void mdrv_gpio_set_input(MS_GPIO_NUM gpio)
{
    GPIO_DBG_FUNC();

    MS_ASSERT(gpio_op);
    if (gpio_op)
    {
        gpio_op->set_input(gpio);
    }
}

#if 0
void mdrv_gpio_set_output(MS_GPIO_NUM gpio)
{
    MS_ASSERT(gpio_op);
    if (gpio_op)
    {
        gpio_op->set_output(gpio);
    }
}
#endif

int mdrv_gpio_get_inout(MS_GPIO_NUM gpio)
{
    GPIO_DBG_FUNC();

    MS_ASSERT(gpio_op);
    if (gpio_op)
    {
        return gpio_op->get_inout(gpio);
    }
    else
    {
        return 0;
    }
}

int mdrv_gpio_get_level(MS_GPIO_NUM gpio)
{
    GPIO_DBG_FUNC();

    MS_ASSERT(gpio_op);
    if (gpio_op)
    {
        return gpio_op->get_level(gpio);
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------
/// enable input for selected one pad
/// @param  u32IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pad_Odn(MS_GPIO_NUM u32IndexGPIO)
{
    MHal_GPIO_Pad_Odn(u32IndexGPIO);
}

//-------------------------------------------------------------------------------------------------
/// enable output for selected one pad
/// @param  u32IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pad_Oen(MS_GPIO_NUM u32IndexGPIO)
{
    MHal_GPIO_Pad_Oen(u32IndexGPIO);
}

//-------------------------------------------------------------------------------------------------
/// output pull high for selected one pad
/// @param  u32IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pull_High(MS_GPIO_NUM u32IndexGPIO)
{
    MHal_GPIO_Pull_High(u32IndexGPIO);
}

//-------------------------------------------------------------------------------------------------
/// output pull low for selected one pad
/// @param  u32IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pull_Low(MS_GPIO_NUM u32IndexGPIO)
{
    MHal_GPIO_Pull_Low(u32IndexGPIO);
}

//-------------------------------------------------------------------------------------------------
/// read data from selected one pad
/// @param  u32IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
U8 MDrv_GPIO_Pad_Read(MS_GPIO_NUM u32IndexGPIO)
{
    return MHal_GPIO_Pad_Level(u32IndexGPIO);
}

#endif  //__DRVGPIO_C__
