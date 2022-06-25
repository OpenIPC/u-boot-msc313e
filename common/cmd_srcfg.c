#include <common.h>
#include <command.h>
#include <malloc.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include <../drivers/mstar/isp/isp_pub.h>


//#if defined(CONFIG_MS_SRCFG)
int do_sensor_cfg(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    if(argc<5)
    {
        printf("missing parameters\n");
        return CMD_RET_USAGE;
    }

    u32 pad = simple_strtol(argv[1],0,10);
    u32 mclk = simple_strtol(argv[2],0,10);
    u32 pdwn = simple_strtol(argv[3],0,10);
    u32 rst = simple_strtol(argv[4],0,10);
    //*buf = *buf & ~BIT11;
    Select_SR_IOPad((SR_PAD_CFG)pad);
    Set_SR_MCLK(mclk>=999?0:1,mclk);
    ISP_SrPwdn(0,pdwn);
    ISP_SrRst(0,rst);
    ISP_SrPwdn(1,pdwn);
    ISP_SrRst(1,rst);
    printf("\n sensor configuration, PAD=%u,MCLK=%uMHz, PWD_PIN=%u, RST_PIN=%u \n\n",pad,mclk,pdwn,rst);
    return 0;
}

U_BOOT_CMD(
    srcfg,  CONFIG_SYS_MAXARGS,    1,    do_sensor_cfg,
    "sensor pin and mclk configuration.",
    "pad mclk pwd_pin rst_pin\n"
	" -pad Select sensor pad 0~6\n"
    " -mclk 0:5.4MHz 1:12MHz 2:21.6MHz 3:27MHz 4:36MHz 5:43.2MHz 6:54MHz 7:61.7MHz 999:MCLK off\n"
    " -pwd_pin sensor power down pin 0:low 1:high\n"
	" -rst_pin sensor reset pin 0:low 1:high\n"
);
//#endif
