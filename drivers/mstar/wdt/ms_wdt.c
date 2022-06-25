/*
 * [origin: Linux kernel drivers/watchdog/at91sam9_wdt.c]
 *
 * Watchdog driver for Atmel AT91SAM9x processors.
 *
 * Copyright (C) 2008 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 * Copyright (C) 2008 Renaud CERRATO r.cerrato@til-technologies.fr
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * The Watchdog Timer Mode Register can be only written to once. If the
 * timeout need to be set from U-Boot, be sure that the bootstrap doesn't
 * write to this register. Inform Linux to it too
 */

#include <common.h>
#include <watchdog.h>
#include "environment.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "ms_version.h"


/* Hardware timeout in seconds */
#define WDT_HW_TIMEOUT 60

void hw_watchdog_disable(void)
{
	OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_H), (((CONFIG_PIUTIMER_CLOCK * WDT_HW_TIMEOUT)>>16) & 0x0));
    OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_L), ((CONFIG_PIUTIMER_CLOCK * WDT_HW_TIMEOUT) & 0x0));
}


void hw_watchdog_reset(void)
{
	OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_WDT_CLR), 0x1); //clear
}

void hw_watchdog_init(void)
{
	printf("[WDT] Enalbe WATCHDOG 60s\n");
	OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_H), (((CONFIG_PIUTIMER_CLOCK * WDT_HW_TIMEOUT)>>16) & 0xFFFF));
    OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_L), ((CONFIG_PIUTIMER_CLOCK * WDT_HW_TIMEOUT) & 0xFFFF));
	hw_watchdog_reset();
}


