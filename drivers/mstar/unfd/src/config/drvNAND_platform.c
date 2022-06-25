//=======================================================================
//  MStar Semiconductor - Unified Nand Flash Driver
//
//  drvNAND_platform.c - Storage Team, 2009/08/20
//
//  Design Notes: defines common platform-dependent functions.
//
//    1. 2009/08/25 - support C5 eCos platform
//
//=======================================================================
#include "../../inc/common/drvNAND.h"
#include <linux/string.h>



//=============================================================
//=============================================================


#if (defined(NAND_DRV_TV_MBOOT)&&NAND_DRV_TV_MBOOT)

NAND_DRIVER sg_NandDrv;
U32 gau32_PartInfo[0x200/4];

U32 nand_hw_timer_delay(U32 u32usTick)
{


#if defined(CONFIG_MIPS) // Use PIU timer

  U32 u32HWTimer = 0;
  volatile U16 u16TimerLow = 0;
  volatile U16 u16TimerHigh = 0;

  // reset HW timer
  REG_WRITE_UINT16(TIMER0_MAX_LOW, 0xFFFF);
  REG_WRITE_UINT16(TIMER0_MAX_HIGH, 0xFFFF);
  REG_WRITE_UINT16(TIMER0_ENABLE, 0);

  // start HW timer
  REG_SET_BITS_UINT16(TIMER0_ENABLE, 0x0001);

  while( u32HWTimer < 12*u32usTick ) // wait for u32usTick micro seconds
  {
    REG_READ_UINT16(TIMER0_CAP_LOW, u16TimerLow);
    REG_READ_UINT16(TIMER0_CAP_HIGH, u16TimerHigh);

    u32HWTimer = (u16TimerHigh<<16) | u16TimerLow;
  }

  REG_WRITE_UINT16(TIMER0_ENABLE, 0);

#else // Use kernel udelay
  extern void udelay(unsigned long usec);
  udelay(u32usTick);

#endif

  return u32usTick+1;
}

U32 nand_pads_init(void)
{
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	//Read CEz Configure Setting from u-boot before FCIE is reset.
	pNandDrv->u16_Reg40_Signal =
			(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
			~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);

	return UNFD_ST_SUCCESS;
}

U32 nand_pads_switch(U32 u32EnableFCIE)
{
	#if (ENABLE_CEDRIC) 
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	#if defined(FCIE4_DDR) && FCIE4_DDR
	pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO);
	
	if(NAND_PAD_BYPASS_MODE == u32EnableFCIE)
	{
		//printf("nand_pads_switch BYPASS\r\n");
	    REG_SET_BITS_UINT16(NC_REG_2Fh, BIT11);
    	REG_CLR_BITS_UINT16(REG_ALL_PAD_IN, BIT0);
	    REG_SET_BITS_UINT16(REG_BPS_MODE, (BIT10 - 1));
    	REG_SET_BITS_UINT16(REG_NF_MODE, BIT6);	
	}
	else if(NAND_PAD_TOGGLE_MODE == u32EnableFCIE)
	{
		//RETAILMSG(1, (_T("nand_pads_switch TOGGLE\r\n")));
		//printf("nand_pads_switch TOGGLE\r\n");
		REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
	    REG_CLR_BITS_UINT16(REG_ALL_PAD_IN, BIT0);
    	REG_CLR_BITS_UINT16(REG_BPS_MODE, (BIT10 - 1));
	    REG_SET_BITS_UINT16(REG_NF_MODE, BIT6);
		pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_TOGGLE|BIT_SDR_DIN_FROM_MACRO);
	}
	else if(NAND_PAD_ONFI_SYNC_MODE == u32EnableFCIE)
	{
		//printf("nand_pads_switch ONFI\r\n"); 
		REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
    	REG_CLR_BITS_UINT16(REG_ALL_PAD_IN, BIT0);
	    REG_CLR_BITS_UINT16(REG_BPS_MODE, (BIT10 - 1));
    	REG_SET_BITS_UINT16(REG_NF_MODE, BIT6);
		pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_ONFI|BIT_SDR_DIN_FROM_MACRO);
		
	}

	REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);
	
	#else	//defined(FCIE4_DDR) && FCIE4_DDR

	pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO);
    REG_SET_BITS_UINT16(NC_REG_2Fh, BIT11);
    REG_CLR_BITS_UINT16(REG_ALL_PAD_IN, BIT0);
    REG_SET_BITS_UINT16(REG_BPS_MODE, (BIT10 - 1));
    REG_SET_BITS_UINT16(REG_NF_MODE, BIT6);	
	REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);
		
	#endif

	return UNFD_ST_SUCCESS;

	#elif (ENABLE_INFINITY)
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

    pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO|BIT_NC_32B_MODE);

	if(u32EnableFCIE)
	{
		REG_CLR_BITS_UINT16(REG_ALLPAD_IN, BIT15);
	    REG_SET_BITS_UINT16(REG_NAND_MODE, NAND_MODE1);	//set nand mode
		REG_CLR_BITS_UINT16(REG_SD_MODE, REG_SD_MODE_MASK);
	
		REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);

		REG_SET_BITS_UINT16(REG_CTRL_PE, 0x1F);
	}
	#endif

	return UNFD_ST_SUCCESS;
}

U32 nand_clock_setting(U32 u32ClkParam)   
{
	#if(ENABLE_CEDRIC)
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    REG_CLR_BITS_UINT16(REG_CLK_NFIE, NFIE_CLK_MASK);
    REG_SET_BITS_UINT16(REG_CLK_NFIE, u32ClkParam);

	#if defined(FCIE4_DDR) && FCIE4_DDR
	if ((pNandDrv->u16_Reg58_DDRCtrl & (BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO)) == 0)
	{
		REG_CLR_BITS_UINT16(FCIE_REG_CLK_4X_DIV_EN, BIT0);
		REG_CLR_BITS_UINT16(NC_LATCH_DATA, BIT_NC_LATCH_DATA_MASK);
		if (pNandDrv->u8_RequireRandomizer)
		{
			//RETAILMSG(1, (_T("delay 1.5T\r\n")));
			REG_SET_BITS_UINT16(NC_LATCH_DATA, BIT_NC_LATCH_DATA_1_5_T);
		}
		else
		{
			//RETAILMSG(1, (_T("delay 0.5T\r\n")));
			REG_SET_BITS_UINT16(NC_LATCH_DATA, BIT_NC_LATCH_DATA_0_5_T);
		}
	}
	else
	{
		//printf("SET_BITS_UINT16(FCIE_REG_CLK_4X_DIV_EN, BIT0)\r\n");
		REG_SET_BITS_UINT16(FCIE_REG_CLK_4X_DIV_EN, BIT0);
		REG_WRITE_UINT16(NC_LATCH_DATA,pNandDrv->u16_Reg57_RELatch);
	}
	#else
	REG_CLR_BITS_UINT16(NC_LATCH_DATA, BIT_NC_LATCH_DATA_MASK);
	if (pNandDrv->u8_RequireRandomizer)
	{
		//RETAILMSG(1, (_T("delay 1.5T\r\n")));
		REG_SET_BITS_UINT16(NC_LATCH_DATA, BIT_NC_LATCH_DATA_1_5_T);
	}
	else
	{
		//RETAILMSG(1, (_T("delay 0.5T\r\n")));
		REG_SET_BITS_UINT16(NC_LATCH_DATA, BIT_NC_LATCH_DATA_0_5_T);
	}
	#endif //defined(FCIE4_DDR) && FCIE4_DDR


	#elif (ENABLE_INFINITY)
	//fcie clock
    REG_CLR_BITS_UINT16(reg_ckg_fcie, BIT0|BIT1);
    REG_CLR_BITS_UINT16(reg_ckg_fcie, NFIE_CLK_MASK);
    REG_SET_BITS_UINT16(reg_ckg_fcie, u32ClkParam);

	//ecc clock
	REG_CLR_BITS_UINT16(reg_ckg_ecc, BIT0|BIT1);
    REG_CLR_BITS_UINT16(reg_ckg_ecc, BIT2|BIT3|BIT4);	//set to 160M
    REG_SET_BITS_UINT16(reg_ckg_ecc, BIT2|BIT3);	//set to 12M
	#endif
	return UNFD_ST_SUCCESS;
}


void nand_DumpPadClk(void)
{
#if (ENABLE_CEDRIC)
	

#elif (ENABLE_INFINITY)
	nand_debug(0, 1, "clk setting: \n");
	nand_debug(0, 1, "reg_ckg_fcie(0x%X):0x%x\n", reg_ckg_fcie, REG(reg_ckg_fcie));

	nand_debug(0, 1, "pad setting: \n");
	//fcie pad register
	nand_debug(0, 1, "NC_DDR_CTRL(0x%X):0x%x\n", NC_DDR_CTRL, REG(NC_DDR_CTRL));
	//chiptop pad register
	nand_debug(0, 1, "reg_all_pad_in(0x%X):0x%x\n", REG_ALLPAD_IN, REG(REG_ALLPAD_IN));
	//platform depend reg_sd_use_bypass
	nand_debug(0, 1, "REG_NAND_MODE(0x%X):0x%x\n", REG_NAND_MODE, REG(REG_NAND_MODE));
#endif
}


#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
#define MAX_UNFD(a,b) ((a) > (b) ? (a) : (b))

U32 nand_config_timing(U16 u16_1T)  
{
	#if 1	// defined(FCIE4_DDR) && FCIE4_DDR
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U16 u16_DefaultTRR;
	U16 u16_DefaultTCS;
	U16 u16_DefaultTWW;
	U16 u16_DefaultRX40Cmd=0;
	U16 u16_DefaultRX40Adr=0;
	U16 u16_DefaultRX56;
	U16 u16_DefaultTADL;
	U16 u16_DefaultTCWAW;
	U16 u16_DefaultTCLHZ = 0;
	U16 u16_DefaultTWHR;
	U16 u16_Tmp, u16_Cnt;
	U16 u16_Tmp2, u16_Cnt2;
	u16_DefaultRX40Cmd=u16_DefaultRX40Cmd;
	u16_DefaultRX40Adr=u16_DefaultRX40Adr;
	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_ONFI)
	{
		u16_DefaultTRR = NC_ONFI_DEFAULT_TRR;
		u16_DefaultTCS = NC_ONFI_DEFAULT_TCS;
		u16_DefaultTWW = NC_ONFI_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_ONFI_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_ONFI_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_ONFI_DEFAULT_RX56;
		u16_DefaultTADL = NC_ONFI_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_ONFI_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_ONFI_DEFAULT_TWHR;
	}
	else if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_TOGGLE)
	{
		u16_DefaultTRR = NC_TOGGLE_DEFAULT_TRR;
		u16_DefaultTCS = NC_TOGGLE_DEFAULT_TCS;
		u16_DefaultTWW = NC_TOGGLE_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_TOGGLE_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_TOGGLE_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_TOGGLE_DEFAULT_RX56;
		u16_DefaultTADL = NC_TOGGLE_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_TOGGLE_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_TOGGLE_DEFAULT_TWHR;
	}
	else

	{
		u16_DefaultTRR = NC_SDR_DEFAULT_TRR;
		u16_DefaultTCS = NC_SDR_DEFAULT_TCS;
		u16_DefaultTWW = NC_SDR_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_SDR_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_SDR_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_SDR_DEFAULT_RX56;
		u16_DefaultTADL = NC_SDR_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_SDR_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_SDR_DEFAULT_TWHR;		
	}
	#endif
	
	#if defined(NC_INST_DELAY) && NC_INST_DELAY
	// Check CMD_END
	u16_Tmp = MAX_UNFD(pNandDrv->u16_tWHR, pNandDrv->u16_tCWAW);
	u16_Cnt = (u16_Tmp+u16_1T-1)/u16_1T;

	if(u16_DefaultRX40Cmd >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultRX40Cmd > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultRX40Cmd;

	// Check ADR_END
	u16_Tmp2 = MAX_UNFD(MAX_UNFD(pNandDrv->u16_tWHR, pNandDrv->u16_tADL), pNandDrv->u16_tCCS);
	u16_Cnt2 = (u16_Tmp2+u16_1T-1)/u16_1T;

	if(u16_DefaultRX40Adr >= u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultRX40Adr > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultRX40Adr;

	// get the max cnt
	u16_Cnt = MAX_UNFD(u16_Cnt, u16_Cnt2);

	pNandDrv->u16_Reg40_Signal &= ~(0x00FF<<8);
	pNandDrv->u16_Reg40_Signal |= (u16_Cnt<<8);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg40_Signal =  %X\n",pNandDrv->u16_Reg40_Signal);	
	#endif

	#if defined(NC_HWCMD_DELAY) && NC_HWCMD_DELAY
	u16_Cnt = (pNandDrv->u16_tRHW+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultRX56 >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultRX56 > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultRX56;

	pNandDrv->u16_Reg56_Rand_W_Cmd &= ~(0x00FF<<8);
	pNandDrv->u16_Reg56_Rand_W_Cmd |= (u16_Cnt<<8);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg56_Rand_W_Cmd =  %X\n",pNandDrv->u16_Reg56_Rand_W_Cmd);
	#endif
	
	#if defined(NC_TRR_TCS) && NC_TRR_TCS
	u16_Cnt = (pNandDrv->u8_tRR+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultTRR >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTRR > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTRR;

	u16_Tmp = (pNandDrv->u8_tCS+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultTCS >= u16_Tmp)
		u16_Tmp = 0;
	else if(u16_Tmp-u16_DefaultTCS > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Tmp -= u16_DefaultTCS;

	u16_Tmp2 = (pNandDrv->u16_tWW+u16_1T-1)/u16_1T + 2;

    if(u16_DefaultTWW >= u16_Tmp2)
        u16_Tmp2 = 0;
    else if(u16_Tmp2-u16_DefaultTWW > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
        u16_Tmp2 -= u16_DefaultTWW;

	u16_Cnt2 = MAX_UNFD(u16_Tmp, u16_Tmp2);

	pNandDrv->u16_Reg59_LFSRCtrl &= ~(0x00FF);
	pNandDrv->u16_Reg59_LFSRCtrl |= (u16_Cnt|(u16_Cnt2<<4));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg59_LFSRCtrl =  %X\n",pNandDrv->u16_Reg59_LFSRCtrl);	
	#endif

	#if defined(NC_TCWAW_TADL) && NC_TCWAW_TADL 
	u16_Cnt = (pNandDrv->u16_tADL + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTADL > u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTADL > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTADL;
	
	u16_Cnt2 = (pNandDrv->u16_tCWAW + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTADL > u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultTCWAW > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultTCWAW;

	pNandDrv->u16_Reg5D_tCWAW_tADL &= ~(0xFFFF);
	pNandDrv->u16_Reg5D_tCWAW_tADL |= (u16_Cnt|(u16_Cnt2<<8));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg5D_tCWAW_tADL =  %X\n",pNandDrv->u16_Reg5D_tCWAW_tADL);	
	#endif

	#if defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ 
	u16_Cnt = (pNandDrv->u8_tCLHZ + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTCLHZ > u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTCLHZ > 0xF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTCLHZ;
	
	u16_Cnt2 = (pNandDrv->u16_tWHR + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTWHR > u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultTWHR > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultTWHR;

	pNandDrv->u16_Reg5A_tWHR_tCLHZ &= ~(0xFFFF);
	pNandDrv->u16_Reg5A_tWHR_tCLHZ |= ((u16_Cnt&0xF)|(u16_Cnt2<<8));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg5A_tWHR_tCLHZ =  %X\n",pNandDrv->u16_Reg5A_tWHR_tCLHZ);
	#endif

	NC_Config();
	return UNFD_ST_SUCCESS;
}

U32 nand_find_timing(U8 *pu8_ClkIdx, U8 u8_find_DDR_timg)
{
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    U32 au32_1TTable[NFIE_CLK_TABLE_CNT] = NFIE_1T_TABLE;
    U32 au32_ClkValueTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_VALUE_TABLE;

    #if defined(FCIE4_DDR) && FCIE4_DDR || defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
    U32 au32_4Clk1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
    U32 au32_4ClkValueTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_VALUE_TABLE;
    #endif
    #if defined(NC_SEL_FCIE5) && NC_SEL_FCIE5
    U16 u16_i, u16_j, u16_pass_begin = 0xFF, u16_pass_cnt = 0;
    U16 u16_nandstatus;
    U32 u32_Err;
    U8 au8_ID[NAND_ID_BYTE_CNT];
    U32 au32_ClkTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE;
    #endif
    U32 u32_Clk;
    U16 u16_SeqAccessTime, u16_Tmp, u16_Tmp2, u16_1T;
    S8 s8_ClkIdx;    
    #if defined(NC_SEL_FCIE3) && NC_SEL_FCIE3
    U16 u16_RE_LATCH_DELAY = 0;
    #endif
    s8_ClkIdx = 0;
    u16_1T = 0;

    if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_ONFI)
    {
        u16_SeqAccessTime = 10;
    }
    else if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_TOGGLE)
    {
        u16_Tmp = MAX_UNFD(MAX_UNFD(2*pNandDrv->u8_tRP, 2*pNandDrv->u8_tREH), pNandDrv->u16_tRC);
        u16_Tmp2 = MAX_UNFD(MAX_UNFD(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)/2);
        u16_SeqAccessTime = MAX_UNFD(u16_Tmp, u16_Tmp2);
    }
    else
    {
        u16_Tmp = MAX_UNFD(MAX_UNFD(pNandDrv->u8_tRP, pNandDrv->u8_tREH), (pNandDrv->u16_tRC+1)/2);
        u16_Tmp2 = MAX_UNFD(MAX_UNFD(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)/2);
        u16_SeqAccessTime = MAX_UNFD(u16_Tmp, u16_Tmp2);

        u16_Tmp = (pNandDrv->u8_tREA + NAND_SEQ_ACC_TIME_TOL)/2;
        u16_Tmp2 = u16_SeqAccessTime;
        u16_SeqAccessTime = MAX_UNFD(u16_Tmp, u16_Tmp2);
    }

    u32_Clk = 1000000000/((U32)u16_SeqAccessTime); //FCIE5 needs to be update .....

    if(!u8_find_DDR_timg)
    {
        for(s8_ClkIdx =  0; s8_ClkIdx <= NFIE_CLK_TABLE_CNT - 1; s8_ClkIdx ++)
        {
            if(u32_Clk <= au32_ClkValueTable[s8_ClkIdx])
            {
                break;
            }
        }
    }
    else
    {
        #if defined(FCIE4_DDR) && FCIE4_DDR || defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
        for(s8_ClkIdx =  0; s8_ClkIdx <= NFIE_4CLK_TABLE_CNT - 1; s8_ClkIdx ++)
        {
            if(u32_Clk < au32_4ClkValueTable[s8_ClkIdx])
            {
                break;
            }
        }
        #endif
    }
    s8_ClkIdx --;


RETRY:
    if(s8_ClkIdx<0)
        return UNFD_ST_ERR_INVALID_PARAM;
    if(!u8_find_DDR_timg)
        u16_1T = au32_1TTable[s8_ClkIdx];
    #if defined(FCIE4_DDR) && FCIE4_DDR || defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
    else
        u16_1T = au32_4Clk1TTable[s8_ClkIdx];
    #endif

    if(nand_config_timing(u16_1T) != UNFD_ST_SUCCESS)
    {
        s8_ClkIdx--;
        goto RETRY;
    }

    #if defined(NC_SEL_FCIE3) && NC_SEL_FCIE3
    u16_RE_LATCH_DELAY = BIT_NC_LATCH_DATA_1_0_T;

    pNandDrv->u16_Reg57_RELatch &= ~BIT_NC_LATCH_DATA_MASK;
    pNandDrv->u16_Reg57_RELatch |= u16_RE_LATCH_DELAY;
    #elif defined(NC_SEL_FCIE5) && NC_SEL_FCIE5
    if (!u8_find_DDR_timg)
    {
        NC_ReadStatus();
        REG_READ_UINT16(NC_ST_READ, u16_nandstatus);

        pNandDrv->u32_Clk = au32_ClkTable[s8_ClkIdx];
        nand_clock_setting(pNandDrv->u32_Clk);

        //using read id to detect relatch
        memcpy(au8_ID, pNandDrv->au8_ID, NAND_ID_BYTE_CNT);
        for(u16_i = 0 ; u16_i < 16 ; u16_i ++)
        {

            pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT_NC_PAD_SEL_FAILLING);
            if(u16_i &1)    //select falling edge otherwise rising edge is selected
                pNandDrv->u16_Reg57_RELatch|=BIT_NC_PAD_SEL_FAILLING;
            pNandDrv->u16_Reg57_RELatch |= ((u16_i/2) << 1) &BIT_NC_LATCH_DATA_MASK;

            NC_Config();
            u32_Err = NC_ReadID();
            if(u32_Err != UNFD_ST_SUCCESS)
            {
                nand_debug(0, 1, "ReadID Error with ErrCode 0x%lX\n", u32_Err);
                nand_die();
            }
            for(u16_j = 0; u16_j < NAND_ID_BYTE_CNT; u16_j++)
            {
                if(pNandDrv->au8_ID[u16_j] != au8_ID[u16_j])
                {
                    break;
                }
            }
            if(u16_j == NAND_ID_BYTE_CNT)
            {
                if(u16_pass_begin == 0xFF)
                    u16_pass_begin = u16_i;
                u16_pass_cnt ++;
            }
            //  break;
        }

        if(u16_pass_cnt == 0)
        {
            nand_debug(0, 1, "Read ID detect timing fails\n");
            pNandDrv->u16_Reg57_RELatch = BIT_NC_LATCH_DATA_2_0_T|BIT_NC_LATCH_STS_2_0_T;
            NC_Config();
            s8_ClkIdx = 0;
            *pu8_ClkIdx = (U8)s8_ClkIdx;
            memcpy(pNandDrv->au8_ID, au8_ID, NAND_ID_BYTE_CNT);
            return UNFD_ST_SUCCESS;
        }
        else
        {
            u16_i = u16_pass_begin + u16_pass_cnt/2;
            pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT0);
            if(u16_i &1)    //select falling edge otherwise rising edge is selected
                pNandDrv->u16_Reg57_RELatch|=BIT0;
            pNandDrv->u16_Reg57_RELatch |= ((u16_i/2) << 1) &BIT_NC_LATCH_DATA_MASK;
            memcpy(pNandDrv->au8_ID, au8_ID, NAND_ID_BYTE_CNT);
        }

        //detect read status

        u16_pass_begin = 0xFF;
        u16_pass_cnt = 0;

        for(u16_i = 0 ; u16_i < 8 ; u16_i ++)
        {
            U16 u16_tmpStatus;
            pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_STS_MASK);
            pNandDrv->u16_Reg57_RELatch |= ((u16_i) << 5) & BIT_NC_LATCH_STS_MASK;

            NC_Config();
            NC_ReadStatus();
            REG_READ_UINT16(NC_ST_READ, u16_tmpStatus);

            if(u16_tmpStatus == u16_nandstatus)
            {
                if(u16_pass_begin == 0xFF)
                    u16_pass_begin = u16_i;
                u16_pass_cnt ++;
            }
        }
        if(u16_pass_cnt == 0)
        {
            nand_debug(0, 1, "Read status detect timing fails\n");
            pNandDrv->u16_Reg57_RELatch = BIT_NC_LATCH_DATA_2_0_T|BIT_NC_LATCH_STS_2_0_T;
            NC_Config();
            s8_ClkIdx = 0;
            *pu8_ClkIdx = (U8)s8_ClkIdx;

            return UNFD_ST_SUCCESS;
        }
        else
        {
            u16_i = u16_pass_begin + u16_pass_cnt/2;
            pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_STS_MASK);
            pNandDrv->u16_Reg57_RELatch |= ((u16_i) << 5) & BIT_NC_LATCH_STS_MASK;
        }
    }

    #endif

    *pu8_ClkIdx = (U8)s8_ClkIdx;

    return UNFD_ST_SUCCESS;

}

#if (defined(FCIE4_DDR) && FCIE4_DDR) || (defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT)

U8 gu8_unfd_disable_uart = 0;

static const U8 aau8_max_delay_cell[NFIE_CLK_TABLE_CNT][DQS_MODE_TABLE_CNT] =
{
	{31, 31, 31, 0}, // 12M
	{31, 31, 12, 12}, // 20M
	{31, 17, 17, 0}, // 27M
	{27, 17, 0, 0}, // 36M
	{24, 15, 0, 0}, // 40M
	{19,  4, 4, 0}, // 48M
	{4, 1, 1, 7}, // 54M
	{16, 0, 0, 0}, // 72M
	{16, 0, 0, 0}, // 108M

};

/*
static const U8 aau8_half_delay_cell[NFIE_CLK_TABLE_CNT][DQS_MODE_TABLE_CNT] =
{
	{18, 15, 15, 0}, // 12M
	{18, 15, 6, 6}, // 20M
	{18, 9, 9, 0}, // 27M
	{16, 9, 0, 0}, // 36M
	{15, 6, 6, 0}, // 40M
	{13, 8, 0, 0}, // 48M
	{12, 2, 2, 0}, // 54M
	{10, 0, 0, 0}, // 72M
	{7,  0, 0, 0}, // 108M
};
*/
static const U8 aau8_half_delay_cell[NFIE_4CLK_TABLE_CNT][DQS_MODE_TABLE_CNT] =
{
	{15, 15, 15, 0},  //{84, 28, 28, 0} // 12M    
	{15, 14, 14, 0},	//{42, 14, 14, 0}, // 24M
	{15, 11, 11, 0},	//{32, 11, 11, 0}, // 32M
	{15, 10, 10, 0},	//{28, 10, 10, 0}, // 36M
	{15, 9, 9, 0},	//{25, 9, 9, 0}, // 40M
	{15, 8, 8, 0},	//{24, 8, 8, 0}, // 43.2M
	{15, 7, 7, 0},	//{21, 7, 7, 0}, // 48M
	{15, 7, 7, 0},	//{19, 7, 7, 0}, // 54M
	{15, 6, 6, 0},	//{17, 6, 6, 0}, // 62M
	{14, 5, 5, 0}, // 72M
	{13, 5, 5, 0}, // 80M
	{12, 4, 4, 0}, // 86M
	{10, 4, 4, 0}, // 108M
	{9, 3, 3, 0}, // 123M
	{7, 3, 3, 0}, // 144M
	{6, 2, 2, 0}, // 172M
	{6, 2, 2, 0}, // 192M
	{5, 2, 2, 0}, // 216M
	{5, 2, 2, 0}, // 240M
};

#define MIN_PASS_CNT	3

static U16 *au16_WritePageBuf;
static U16 *au16_ReadPageBuf;
static U8  au8_ReadSpareBuf[512];

static U32 u32_TestBlk;

static U32 nand_prepare_test_pattern(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_Err;
	U16 i, j, u16_pattern_size;
	U32 au32_ClkTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE; 
	U32 au32_1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE; 
	
	// prepare test pattern
	u16_pattern_size = pNandDrv->u16_SectorByteCnt/8/2;
	for(i=0 ; i<8 ; i++)
	{
		switch(i)
		{
			case 0:
				for(j=0 ; j<u16_pattern_size ; j++)
					au16_WritePageBuf[i*u16_pattern_size + j] = 0x0000;
				break;
			case 1:
				for(j=0 ; j<u16_pattern_size ; j++)
					au16_WritePageBuf[i*u16_pattern_size + j] = 0xFFFF;
				break;				
			case 2:
				for(j=0 ; j<u16_pattern_size ; j++)
					au16_WritePageBuf[i*u16_pattern_size + j] = 0x0FF0;
				break;	
			case 3:
				for(j=0 ; j<u16_pattern_size ; j++)
					au16_WritePageBuf[i*u16_pattern_size + j] = 0xFF00;
				break;	
			case 4:
				for(j=0 ; j<u16_pattern_size ; j++)
					au16_WritePageBuf[i*u16_pattern_size + j] = 0x55AA;
				break;	
			case 5:
				for(j=0 ; j<u16_pattern_size ; j++)
					au16_WritePageBuf[i*u16_pattern_size + j] = 0x5A5A;
				break;	
			case 6:
				for(j=0 ; j<u16_pattern_size*2 ; j++)
					((U8 *)au16_WritePageBuf)[i*u16_pattern_size*2 + j] = j;
				break;	
			case 7:
				for(j=0 ; j<u16_pattern_size*2 ; j++)
					((U8 *)au16_WritePageBuf)[i*u16_pattern_size*2 + j] = 0xFF-j;
				break;	
			default:
				break;	
		}
	}
	//nand_debug(UNFD_DEBUG_LEVEL, 0, "dump write buffer-------------------------------\n");		
	//dump_mem((unsigned char *) au16_WritePageBuf, pNandDrv->u16_SectorByteCnt);

	pNandDrv->u32_Clk = au32_ClkTable[pNandDrv->tMinDDR.u8_ClkIdx];
	nand_clock_setting(pNandDrv->u32_Clk);
	NC_FCIE4SetInterface(1, pNandDrv->tMinDDR.u8_DqsMode, pNandDrv->tMinDDR.u8_DelayCell, pNandDrv->tMinDDR.u8_DdrTiming);
	nand_config_timing(au32_1TTable[pNandDrv->tMinDDR.u8_ClkIdx]);

	// write pattern to test blk
	for(u32_TestBlk=9 ; u32_TestBlk>=5 ; u32_TestBlk--)
	{
		if(!drvNAND_IsGoodBlk(u32_TestBlk))
		{
			continue;
		}
			
		u32_Err = NC_EraseBlk(u32_TestBlk*pNandDrv->u16_BlkPageCnt); 
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "Err, NAND, bad blk=%X\n", u32_TestBlk);	
			drvNAND_MarkBadBlk(u32_TestBlk);
			continue;
		}

		u32_Err = NC_WriteSectors(u32_TestBlk*pNandDrv->u16_BlkPageCnt, 0, (U8 *)au16_WritePageBuf, NULL, 1);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "Err, NAND, write fail, blk=%X\n", u32_TestBlk);
			continue;
		}
		
		u32_Err = NC_ReadSectors(u32_TestBlk*pNandDrv->u16_BlkPageCnt, 0, (U8 *)au16_ReadPageBuf, au8_ReadSpareBuf,1);//NULL, 1);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "Err, NAND, read fail, blk=%X\n", u32_TestBlk);
			printf("nand_prepare_test_pattern error\r\n");
			continue;
		}		
		
		if(memcmp((const void *) au16_ReadPageBuf, (const void *) au16_WritePageBuf, pNandDrv->u16_SectorByteCnt) == 0)
		{
			printf("u32_TestBlk : %X", u32_TestBlk);
			break;
		}
		else
		{
			printf("nand_prepare_test_pattern error 2\r\n");
			while(1);
		}
		/*
		for(i=0;i<(pNandDrv->u16_SectorByteCnt>>1);i++)
		{
				if(au16_ReadPageBuf[i] != au16_WritePageBuf[i])
				{
						break;
				}
		}
		if(i == (pNandDrv->u16_SectorByteCnt>>1))
			break;
		*/
	}
	//nand_debug(UNFD_DEBUG_LEVEL, 0, "\n");
	//dump_mem((unsigned char *) au8_ReadSpareBuf, pNandDrv->u16_SectorSpareByteCnt);
	if(u32_TestBlk<5)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, too many bad blk\n");	
		printf("nand_prepare_test_pattern Err, too many bad blk\n");
		while(1);
		
		
		nand_die();
		return UNFD_ST_ERR_LACK_BLK;
	}
	
	return UNFD_ST_SUCCESS;
}

static U32 nand_probe_ddr(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_Err;
//	U16 i;
	gu8_unfd_disable_uart = 1;
	
	//printf("u32_TestBlk : %X\r\n", u32_TestBlk);
	//printf("pNandDrv->u16_Reg58_DDRCtrl : %X\r\n",pNandDrv->u16_Reg58_DDRCtrl);
	
	u32_Err = NC_ReadSectors(u32_TestBlk*pNandDrv->u16_BlkPageCnt, 0, (U8 *)au16_ReadPageBuf, au8_ReadSpareBuf,1);//NULL, 1);
     
	if(u32_Err == UNFD_ST_SUCCESS)
	{				    	
		//printf("NC_ReadSectors OK\r\n");
		if(memcmp((const void *) au16_ReadPageBuf, (const void *) au16_WritePageBuf, pNandDrv->u16_SectorByteCnt) == 0)
		{	
					gu8_unfd_disable_uart = 0;
					return UNFD_ST_SUCCESS;
		}

/*		
		printf("au16_ReadPageBuf\r\n");
		for(i=0;i<(pNandDrv->u16_SectorByteCnt>>1);i++)
		{
			printf("%X ",au16_ReadPageBuf[i]);
		}
		
		printf("\r\nau16_WritePageBuf\r\n");
		for(i=0;i<(pNandDrv->u16_SectorByteCnt>>1);i++)
		{
			printf("%X ",au16_WritePageBuf[i]);
		}
		printf("\r\n");
*/
		/*
		for(i=0;i<(pNandDrv->u16_SectorByteCnt>>1);i++)
		{
				if(au16_ReadPageBuf[i] != au16_WritePageBuf[i])
				{
						break;
				}
		}
		if(i == (pNandDrv->u16_SectorByteCnt>>1))
		{	
		    gu8_unfd_disable_uart = 0;
				return UNFD_ST_SUCCESS;
		}
		*/
	}
	
	//printf("nand_probe_ddr ReadSectors error\r\n");

	return UNFD_ST_ERR_NO_TOGGLE_DDR_TIMING;
}

static U32 nand_probe_ddr_ex(U8 u8_min_delaycell, U8 u8_2Ch_dqsmode)
{
	U8 u8_delaycell, u8_57h;
	U8 u8_57h_start=0;
	U8 u8_pass_cnt;
	const U8 au8_dqsmode[DQS_MODE_TABLE_CNT] = DQS_MODE_TABLE;	// defined in platform-depedent .h
	U8 u8_match;
	u8_57h_start=u8_57h_start;
	for(u8_delaycell=0 ; u8_delaycell<=u8_min_delaycell ; u8_delaycell++)
	{
		u8_pass_cnt = 0;
		u8_match = 0;
		for(u8_57h=3 ; u8_57h<0x10 ; u8_57h++) // ddr timing shouldn't match from 0~2
		{
			NC_FCIE4SetInterface(1, au8_dqsmode[u8_2Ch_dqsmode], u8_delaycell, u8_57h);
			if(UNFD_ST_SUCCESS == nand_probe_ddr()) 
			{
				if(u8_pass_cnt == 0)
					u8_57h_start = u8_57h;

				if((++u8_pass_cnt)==MIN_PASS_CNT)
				{
					u8_match = 1;
					break;
				}
			}
			else
			{
				if(u8_pass_cnt != 0)
					break;
			}		
		}

		if(u8_match == 0)
			return UNFD_ST_ERR_NO_TOGGLE_DDR_TIMING;
	}

	return UNFD_ST_SUCCESS;
}

extern U8  au8_nand_clk_max;

#define DELAY_CELL_PASS_CNT 3
U32 nand_detect_ddr_timing(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_Err;
	DDR_TIMING_GROUP_t *ptDDR;	
	U8 u8_2Ch_dqsmode, u8_delaycell, u8_57h,u8_max_delay_cell;;
	U8 u8_57h_start = 0;
	U8 u8_pass_cnt;
	U8 u8_delaycellpasscnt = 0;
	U8 u8_clk, u8_clk_default=0;
	const U8 au8_dqsmode[DQS_MODE_TABLE_CNT] = DQS_MODE_TABLE;	// defined in platform-depedent .h
	U32 au32_ClkTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE; 
	U32 au32_1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE; 	
	U8 u8_DqsMatch;
	
	U8 pu8buffer1[(16*DQS_MODE_TABLE_CNT*sizeof(DDR_TIMING_GROUP_t))];
	U8 pu8buffer2[1024];
	U8 pu8buffer3[1024];
	u8_clk_default=u8_clk_default;
	ptDDR = (DDR_TIMING_GROUP_t *)pu8buffer1;
	//ptDDR = (DDR_TIMING_GROUP_t *)malloc(16*DQS_MODE_TABLE_CNT*sizeof(DDR_TIMING_GROUP_t));
	au16_WritePageBuf = (U16 *)pu8buffer2;
	//au16_WritePageBuf = (U16 *)malloc(pNandDrv->u16_SectorByteCnt);
	au16_ReadPageBuf = (U16 *)pu8buffer3;
	//au16_ReadPageBuf = (U16 *)malloc(pNandDrv->u16_SectorByteCnt);	
	if(!ptDDR || !au16_WritePageBuf || !au16_ReadPageBuf)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, cannot allocate memory\n");
		nand_die();
		return UNFD_ST_ERR_INVALID_ADDR;
	}

	u32_Err = nand_prepare_test_pattern();    
	if(u32_Err < UNFD_ST_SUCCESS)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, cannot prepare test pattern\n");	
		nand_die();
		return UNFD_ST_ERR_LACK_BLK;
	}

	/**************** search min ddr timing *********************/
	nand_debug(UNFD_DEBUG_LEVEL, 0,"*** Try to detect min ddr timing ***\r\n");			
	printf("*** Try to detect min ddr timing ***\r\n");
	u8_DqsMatch = 0;
	pNandDrv->u32_Clk = au32_ClkTable[0];
	nand_clock_setting(pNandDrv->u32_Clk);
	nand_config_timing(au32_1TTable[0]);
	gu8_unfd_disable_uart = 0;
	nand_debug(UNFD_DEBUG_LEVEL, 0," **********************************************\r\n");
    nand_debug(UNFD_DEBUG_LEVEL, 0," CLK[%X]\r\n", pNandDrv->u32_Clk);
	nand_debug(UNFD_DEBUG_LEVEL, 0," **********************************************\r\n");	
	u8_max_delay_cell=0x1;
	
	for(u8_2Ch_dqsmode=0 ; u8_2Ch_dqsmode<DQS_MODE_TABLE_CNT ; u8_2Ch_dqsmode++)
	{
		u8_pass_cnt = 0;
		for(u8_delaycell=0; u8_delaycell<u8_max_delay_cell; u8_delaycell++)
		{
        	for(u8_57h=3 ; u8_57h<0x10 ; u8_57h++) // ddr timing shouldn't match from 0~2   
        	{
        		NC_FCIE4SetInterface(1, au8_dqsmode[u8_2Ch_dqsmode], u8_delaycell, u8_57h);    
        		if(UNFD_ST_SUCCESS == nand_probe_ddr())        
        		{
        			nand_debug(UNFD_DEBUG_LEVEL, 0,"clk[%X], dqsmode[%u], DelayCell[%u], ddr_timing[%u]\r\n",
        						pNandDrv->u32_Clk, au8_dqsmode[u8_2Ch_dqsmode], u8_delaycell,
        						u8_57h);				
        			if(u8_pass_cnt == 0)
        				u8_57h_start = u8_57h;
        
        			if((++u8_pass_cnt)==MIN_PASS_CNT)
        			{
        				ptDDR[u8_2Ch_dqsmode].u8_ClkIdx = 0;
        				ptDDR[u8_2Ch_dqsmode].u8_DqsMode = au8_dqsmode[u8_2Ch_dqsmode];
        				ptDDR[u8_2Ch_dqsmode].u8_DelayCell = u8_delaycell;
        				ptDDR[u8_2Ch_dqsmode].u8_DdrTiming = u8_57h_start+1;
        				u8_DqsMatch |= (1<<u8_2Ch_dqsmode);
        				break;
        			}
        		}
        		else
        		{
        			if(u8_pass_cnt != 0)
        				break;
        		}		
        	}
		}
	}
    gu8_unfd_disable_uart = 0;
	u8_2Ch_dqsmode = 0;
	switch(u8_DqsMatch)
	{
		case 0x0:
		case 0x8:
		case 0x4:
		case 0xA:
		case 0x1:
		case 0x9:
		case 0x5:
		case 0xD:
		case 0xB:
			nand_debug(UNFD_DEBUG_LEVEL, 0,"Cannot find ddr timing @clk=%X, DQS combination=%X\r\n", pNandDrv->u32_Clk, u8_DqsMatch);			
			break;
			
		case 0x2:
		case 0x3:			
			if(nand_probe_ddr_ex(aau8_half_delay_cell[u8_clk][1], 1) == UNFD_ST_SUCCESS)  
				u8_2Ch_dqsmode = 1; 
			break;	
			
		case 0xC: // 2.5T
			u8_2Ch_dqsmode = 3;
			break;
		case 0x6: // 1.5T
			u8_2Ch_dqsmode = 1;
			break;
		case 0xE: // 2T
			u8_2Ch_dqsmode = 2;
			break;			
		case 0x7: // 1.5T
			u8_2Ch_dqsmode = 1;
			break;
		case 0xF: // 2T
			u8_2Ch_dqsmode = 2;
			break;
	}
    if(u8_2Ch_dqsmode == 0)
	{
		printf("Err, NAND, can't detect min ddr timing\n");
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, can't detect min ddr timing\n");
		nand_die();
		return UNFD_ST_ERR_NO_TOGGLE_DDR_TIMING;
	}
	else
	{
		pNandDrv->tMinDDR.u8_ClkIdx = ptDDR[u8_2Ch_dqsmode].u8_ClkIdx;
		pNandDrv->tMinDDR.u8_DqsMode = ptDDR[u8_2Ch_dqsmode].u8_DqsMode;
		pNandDrv->tMinDDR.u8_DelayCell = ptDDR[u8_2Ch_dqsmode].u8_DelayCell;
		pNandDrv->tMinDDR.u8_DdrTiming = ptDDR[u8_2Ch_dqsmode].u8_DdrTiming;
		nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get min DDR timing: clk[%X], dqsmode[%u], DelayCell[%u], ddr_timing[%u]\r\n",
				pNandDrv->tMinDDR.u8_ClkIdx, pNandDrv->tMinDDR.u8_DqsMode, pNandDrv->tMinDDR.u8_DelayCell,
				pNandDrv->tMinDDR.u8_DdrTiming);			
	}
	pNandDrv->u32_minClk = au32_ClkTable[pNandDrv->tMinDDR.u8_ClkIdx];
	
	/**************** search default ddr timing *********************/
	nand_debug(UNFD_DEBUG_LEVEL, 0,"*** Try to detect default ddr timing ***\r\n");
	printf("*** Try to detect default ddr timing ***\r\n");
	u32_Err = nand_find_timing(&u8_clk, 1);

	if(u32_Err != UNFD_ST_SUCCESS)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
		nand_die();
		return(u32_Err);
	}
	#if IF_IP_VERIFY
    u8_clk =au8_nand_clk_max;
	#endif
  for( ; u8_clk>0 ; u8_clk--) // search from high-speed clk
	{
		u8_DqsMatch = 0;
		pNandDrv->u32_Clk = au32_ClkTable[u8_clk];
		nand_debug(UNFD_DEBUG_LEVEL, 0," **********************************************\r\n");
	    nand_debug(UNFD_DEBUG_LEVEL, 0," CLK[%X]\r\n", au32_ClkTable[u8_clk]);
		nand_debug(UNFD_DEBUG_LEVEL, 0," **********************************************\r\n");		
		nand_clock_setting(pNandDrv->u32_Clk);
		if(nand_config_timing(au32_1TTable[u8_clk]) != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL, 0,"Cannot match nand timing @clk=%X, DQS combination=%X\r\n", pNandDrv->u32_Clk);		
			continue;
		}

		for(u8_2Ch_dqsmode=0 ; u8_2Ch_dqsmode<DQS_MODE_TABLE_CNT ; u8_2Ch_dqsmode++)
		{
				u8_delaycellpasscnt = 0;
				for(u8_delaycell=0; u8_delaycell < DELAY_CELL_PASS_CNT; u8_delaycell++)
				{
        		u8_pass_cnt = 0; 
        		for(u8_57h=3 ; u8_57h<0x10 ; u8_57h++) // ddr timing shouldn't match from 0~2
        		{
        			NC_FCIE4SetInterface(1, au8_dqsmode[u8_2Ch_dqsmode], u8_delaycell, u8_57h);

        			if(UNFD_ST_SUCCESS == nand_probe_ddr())
        			{
        				nand_debug(UNFD_DEBUG_LEVEL, 0,"clk[%X], dqsmode[%u], DelayCell[%u], ddr_timing[%u]\r\n",
        							pNandDrv->u32_Clk, au8_dqsmode[u8_2Ch_dqsmode], u8_delaycell,	u8_57h);				
        				if(u8_pass_cnt == 0)
        					u8_57h_start = u8_57h;
        
        				if((++u8_pass_cnt)==MIN_PASS_CNT)
        				{
        							if (u8_delaycell == 0)
        							{
        								ptDDR[u8_2Ch_dqsmode].u8_ClkIdx = u8_clk;
        								ptDDR[u8_2Ch_dqsmode].u8_DqsMode = au8_dqsmode[u8_2Ch_dqsmode];
        								ptDDR[u8_2Ch_dqsmode].u8_DelayCell = u8_delaycell;
        								ptDDR[u8_2Ch_dqsmode].u8_DdrTiming = u8_57h_start+1;
        								//u8_DqsMatch |= (1<<u8_2Ch_dqsmode);
        							}
        							u8_delaycellpasscnt++;
        							break;
        				}
        			}
        			else
        			{
        				if(u8_pass_cnt != 0)
        					break;
        			}
    		    }
        		
		        if (u8_delaycellpasscnt==(u8_delaycell+1))
        		{
        			if (u8_delaycellpasscnt == DELAY_CELL_PASS_CNT)
        			{
        					nand_debug(UNFD_DEBUG_LEVEL, 0,"u8_delaycellpass = 3 pass\r\n");
        					u8_DqsMatch |= (1<<u8_2Ch_dqsmode);
        					break;
        			}
    		    }
		        else
        		{
        			break;
        		}
        }
		}
        gu8_unfd_disable_uart=0;
		u8_2Ch_dqsmode = 0;
		switch(u8_DqsMatch)
		{
			case 0x0:
			case 0x8:
			case 0x4:
			case 0xA:
			case 0x1:
			case 0x9:
			case 0x5:
			case 0xD:
			case 0xB:
				nand_debug(UNFD_DEBUG_LEVEL, 0,"Cannot find ddr timing @clk=%X, DQS combination=%X\r\n", pNandDrv->u32_Clk, u8_DqsMatch);			
				break;
				
			case 0x2:
			case 0x3:			
				if(nand_probe_ddr_ex(aau8_half_delay_cell[u8_clk][1], 1) == UNFD_ST_SUCCESS)
					u8_2Ch_dqsmode = 1;	
				break;	
				
			case 0xC: // 2.5T
				u8_2Ch_dqsmode = 3;
				break;
			case 0x6: // 1.5T
				u8_2Ch_dqsmode = 1;
				break;
			case 0xE: // 2T
				u8_2Ch_dqsmode = 2;
				break;			
			case 0x7: // 1.5T
				u8_2Ch_dqsmode = 1;
				break;
			case 0xF: // 2T
				u8_2Ch_dqsmode = 2;
				break;
		}
        #if 0
		pNandDrv->tDefaultDDR.u8_ClkIdx = ptDDR[u8_2Ch_dqsmode].u8_ClkIdx;
		pNandDrv->tDefaultDDR.u8_DqsMode = ptDDR[u8_2Ch_dqsmode].u8_DqsMode;
		pNandDrv->tDefaultDDR.u8_DelayCell = ptDDR[u8_2Ch_dqsmode].u8_DelayCell;
		pNandDrv->tDefaultDDR.u8_DdrTiming = ptDDR[u8_2Ch_dqsmode].u8_DdrTiming;
		nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: clk[%X], dqsmode[%u], DelayCell[%u], ddr_timing[%u]\r\n",
				pNandDrv->tDefaultDDR.u8_ClkIdx, pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tDefaultDDR.u8_DelayCell,
				pNandDrv->tDefaultDDR.u8_DdrTiming);	
		break;
		#else
		if(u8_2Ch_dqsmode != 0)
		{
			pNandDrv->tDefaultDDR.u8_ClkIdx = ptDDR[u8_2Ch_dqsmode].u8_ClkIdx;
			pNandDrv->tDefaultDDR.u8_DqsMode = ptDDR[u8_2Ch_dqsmode].u8_DqsMode;
			pNandDrv->tDefaultDDR.u8_DelayCell = ptDDR[u8_2Ch_dqsmode].u8_DelayCell;
			pNandDrv->tDefaultDDR.u8_DdrTiming = ptDDR[u8_2Ch_dqsmode].u8_DdrTiming;
			printf("ok, get default DDR timing: clk[%X], dqsmode[%u], DelayCell[%u], ddr_timing[%u]\r\n",
					pNandDrv->tDefaultDDR.u8_ClkIdx, pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tDefaultDDR.u8_DelayCell,
					pNandDrv->tDefaultDDR.u8_DdrTiming);	
			//nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: clk[%X], dqsmode[%u], DelayCell[%u], ddr_timing[%u]\r\n",
			//		pNandDrv->tDefaultDDR.u8_ClkIdx, pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tDefaultDDR.u8_DelayCell,
			//		pNandDrv->tDefaultDDR.u8_DdrTiming);	
			break;
		}
		#endif

	}

	if(u8_clk == 0)
	{
		//nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Warning, NAND, can't detect higher default timing, set min as default\n");
		printf("Warning, NAND, can't detect higher default timing, set min as default\n");
		pNandDrv->tDefaultDDR.u8_ClkIdx = pNandDrv->tMinDDR.u8_ClkIdx;
		pNandDrv->tDefaultDDR.u8_DqsMode = pNandDrv->tMinDDR.u8_DqsMode;
		pNandDrv->tDefaultDDR.u8_DelayCell = pNandDrv->tMinDDR.u8_DelayCell;
		pNandDrv->tDefaultDDR.u8_DdrTiming = pNandDrv->tMinDDR.u8_DdrTiming;
		u8_clk_default = 1;
	}
	else
	{
		u8_clk_default = u8_clk;
		nand_debug(UNFD_DEBUG_LEVEL, 0,"u8_clk_default:%x\r\n",u8_clk_default);
	}

	/**************** search max ddr timing *********************/
	// TODO
	/************************************************************/

	pNandDrv->u32_Clk = au32_ClkTable[pNandDrv->tDefaultDDR.u8_ClkIdx];
	nand_clock_setting(pNandDrv->u32_Clk);
	nand_config_timing(au32_1TTable[pNandDrv->tDefaultDDR.u8_ClkIdx]);
	NC_FCIE4SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);

	NC_EraseBlk(u32_TestBlk*pNandDrv->u16_BlkPageCnt);
	//free(ptDDR);
	//free(au16_WritePageBuf);
	//free(au16_ReadPageBuf);
	
	return UNFD_ST_SUCCESS;
}
#endif

#endif

U32 nand_config_clock(U16 u16_SeqAccessTime)
{
  // do nothing in Linux

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	NAND_DRIVER * pNandDrv = drvNAND_get_DrvContext_address();
	U32 u32_Err = 0;
	U32 au32_ClkTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE;
	char *ClkStrTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE_STR;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	U32 au32_4ClkTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE;
	char *Clk4StrTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE_STR;
	U32 au32_1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE; 	
	#endif

	U8 u8_ClkIdx = 0;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_MASM)
	{
		if(pNandDrv->tDefaultDDR.u8_DdrTiming == 0)
		{
			nand_detect_ddr_timing();
		}
		else
		{
			pNandDrv->u32_minClk = au32_4ClkTable[pNandDrv->tMinDDR.u8_ClkIdx];			
			pNandDrv->u32_Clk = au32_4ClkTable[pNandDrv->tDefaultDDR.u8_ClkIdx];
			NC_FCIE4SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode, 
				pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);

			if(nand_config_timing(au32_1TTable[pNandDrv->tDefaultDDR.u8_ClkIdx]) != UNFD_ST_SUCCESS)
			{
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
				nand_die();
				return(u32_Err);
			}			
		}
		nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: 2Ch:%X, 57h:%X\n", 
					pNandDrv->u16_Reg2C_SMStatus, pNandDrv->u16_Reg57_RELatch);	
		u8_ClkIdx = pNandDrv->tDefaultDDR.u8_ClkIdx;
		printf("FCIE is set to %sHz...4X\n", Clk4StrTable[u8_ClkIdx]);
	}
	else
	#endif
	{
		
		u32_Err = nand_find_timing(&u8_ClkIdx, 0);   
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
			nand_die();
			return(u32_Err);
		}
		pNandDrv->u32_Clk = au32_ClkTable[u8_ClkIdx];
		nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "FCIE is set to %sHz\n", ClkStrTable[u8_ClkIdx]);
	}

	nand_clock_setting(pNandDrv->u32_Clk);
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	//nand_debug(UNFD_DEBUG_LEVEL, 0,"reg_ckg_fcie(%08X)=%08X\n", reg_ckg_fcie, REG(reg_ckg_fcie));
//	printf("RE LATCH is set to %X\n", pNandDrv->u16_Reg57_RELatch);
	
#endif

  return UNFD_ST_SUCCESS;
}


void nand_set_WatchDog(U8 u8_IfEnable)
{
  // do nothing in Linux
}

void nand_reset_WatchDog(void)
{
  // do nothing in Linux
}

//DECLARE_MUTEX(FCIE3_mutex);
//atomic_t nand_need_init = ATOMIC_INIT(0);
//atomic_t nand_in_use = ATOMIC_INIT(0);
static U16 reg_sdio_mod;
static U16 reg_sdio_ctl;
static U16 reg_mie_evt_en;

void nand_lock_fcie(void)
{
  U32 cnt = 0;

  //down(&FCIE3_mutex);

  //atomic_set(&nand_in_use, 1);

    REG_READ_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x10), reg_sdio_mod);
    REG_READ_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x11), reg_sdio_ctl);
    REG_READ_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x01), reg_mie_evt_en);
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x01), 0);
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x11), 0);

    do {
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x10), 0);
    if( ++cnt > 0xFFFFFF )
    {
      //printk("\033[31mTimeout!!!\033[m\n");
      printf("\033[31mTimeout!!!\033[m\n");
      break;
    }
  } while( REG(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x10)));

    #if defined(NC_SEL_FCIE3) && NC_SEL_FCIE3
    REG_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);
    #elif defined(NC_SEL_FCIE5) && NC_SEL_FCIE5
    REG_WRITE_UINT16(NC_FUN_CTL, BIT_NC_EN);
    #endif

  //if( atomic_read(&nand_need_init))
  //{
    NC_ResetFCIE();
    //NC_ResetNandFlash();
    NC_Config();
    //atomic_set(&nand_need_init, 0);
  //}
}

void nand_unlock_fcie(void)
{
    HAL_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0A), 0x02);
    HAL_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x10), reg_sdio_mod);
    HAL_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x11), reg_sdio_ctl);
    HAL_WRITE_UINT16(GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x01), reg_mie_evt_en);
    //atomic_set(&nand_in_use, 0);

  //up(&FCIE3_mutex);
}


U32 nand_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
  flush_cache(u32_DMAAddr, u32_ByteCnt); 
  #if defined(CONFIG_MIPS32)
  Chip_Flush_Memory();
  return  (u32_DMAAddr & 0x1fffffff);
  #elif defined(CONFIG_ARM)  
  return  (u32_DMAAddr);
  #endif
}

U32 nand_translate_Spare_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
  flush_cache(u32_DMAAddr, u32_ByteCnt); 
  #if defined(CONFIG_MIPS32)
  Chip_Flush_Memory();
  return  (u32_DMAAddr & 0x1fffffff);
  #elif defined(CONFIG_ARM)  
  return  (u32_DMAAddr);
  #endif
}

void nand_flush_miu_pipe(void)
{
  #if defined(CONFIG_URANUS4)
  Chip_Read_Memory();
  #endif
}

U32 nand_translate_DMA_MIUSel(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    return 0;
}


void nand_read_dma_post_flush(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    #if defined(CONFIG_MSTAR_MONACO) || defined(CONFIG_MSTAR_MONET) || defined(CONFIG_MSTAR_MESSI)
    flush_cache(u32_DMAAddr, u32_ByteCnt);
    #endif
}

#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static int fcie_int = 0;
static U16 u16CurNCMIEEvent = 0;      // Used to store current IRQ state

irqreturn_t NC_FCIE_IRQ(int irq, void *dummy)
{
    REG_WRITE_UINT16(NC_MIE_EVENT, BIT_NC_JOB_END);

    u16CurNCMIEEvent = 0;               // Reset the current IRQ state
    fcie_int = 1;
    wake_up(&fcie_wait);

  return IRQ_HANDLED;
}

U32 nand_WaitCompleteIntr(U16 u16_WaitEvent, U32 u32_MicroSec)
{
  U16 u16_Reg;
  U32 u32_Count = 1; // Return microsec if timout
  U32 u32_Timeout = ((u32_MicroSec/1000) > 0) ? (u32_MicroSec/1000) : 1;  // timeout time

  if( wait_event_timeout(fcie_wait, (fcie_int == 1), u32_Timeout) == 0 ) // wait at least 2 second for FCIE3 events
  {
    u32_Count = u32_MicroSec;
    REG_READ_UINT16(NC_MIE_EVENT, u16_Reg);   // Read all events
    REG_WRITE_UINT16(NC_MIE_EVENT, u16_Reg);  // Clear all events
  }

  fcie_int = 0;

  return UNFD_ST_SUCCESS;
}

void nand_enable_intr_mode(void)
{
  request_irq(13, NC_FCIE_IRQ, SA_INTERRUPT, "fcie", NULL);
  REG_WRITE_UINT16(NC_MIE_INT_EN, BIT_NC_JOB_END);
}
#endif

void *drvNAND_get_DrvContext_address(void) // exposed API
{
  return &sg_NandDrv;
}
void *drvNAND_get_DrvContext_PartInfo(void)
{
   return (void*)((U32)gau32_PartInfo);
}

static UNFD_ALIGN0 U32 drvNandPageBuf[8192/4] UNFD_ALIGN1; /* 8192 Bytes */
static UNFD_ALIGN0 U32 drvNandSpareBuf[512/4] UNFD_ALIGN1; /*   512 Bytes */

//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
const U8 u8FSTYPE[256] =
{ 
	0,19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
  	0, 0, 0, 0, 0, 0, 0,16, 0, 0, 0, 0, 0, 0, 0, 0, // 3
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
  	0,18, 0, 6, 0, 8,10, 0, 0,12, 0, 0, 0, 0, 0, 0, // 7
  	0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, // 8
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
  	0,13, 0,16, 0,17, 3, 0, 0, 0,15, 0,14, 0, 0, 0, // D
  	0, 0, 0, 2, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
   20,13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F
};

U16 PairedPageMap_Hynix_LSB[] = {
0x0, 0x1, 0x2, 0x3, 0x6, 0x7, 0xA, 0xB, 0xE, 0xF, 0x12, 0x13, 0x16, 0x17, 0x1A, 0x1B,
0x1E, 0x1F, 0x22, 0x23, 0x26, 0x27, 0x2A, 0x2B, 0x2E, 0x2F, 0x32, 0x33, 0x36, 0x37, 0x3A, 0x3B,
0x3E, 0x3F, 0x42, 0x43, 0x46, 0x47, 0x4A, 0x4B, 0x4E, 0x4F, 0x52, 0x53, 0x56, 0x57, 0x5A, 0x5B,
0x5E, 0x5F, 0x62, 0x63, 0x66, 0x67, 0x6A, 0x6B, 0x6E, 0x6F, 0x72, 0x73, 0x76, 0x77, 0x7A, 0x7B,
0x7E, 0x7F, 0x82, 0x83, 0x86, 0x87, 0x8A, 0x8B, 0x8E, 0x8F, 0x92, 0x93, 0x96, 0x97, 0x9A, 0x9B,
0x9E, 0x9F, 0xA2, 0xA3, 0xA6, 0xA7, 0xAA, 0xAB, 0xAE, 0xAF, 0xB2, 0xB3, 0xB6, 0xB7, 0xBA, 0xBB,
0xBE, 0xBF, 0xC2, 0xC3, 0xC6, 0xC7, 0xCA, 0xCB, 0xCE, 0xCF, 0xD2, 0xD3, 0xD6, 0xD7, 0xDA, 0xDB,
0xDE, 0xDF, 0xE2, 0xE3, 0xE6, 0xE7, 0xEA, 0xEB, 0xEE, 0xEF, 0xF2, 0xF3, 0xF6, 0xF7, 0xFA, 0xFB
};
U16 PairedPageMap_Hynix_MSB[] = {
0x4, 0x5, 0x8, 0x9, 0xC, 0xD, 0x10, 0x11, 0x14, 0x15, 0x18, 0x19, 0x1C, 0x1D, 0x20, 0x21,
0x24, 0x25, 0x28, 0x29, 0x2C, 0x2D, 0x30, 0x31, 0x34, 0x35, 0x38, 0x39, 0x3C, 0x3D, 0x40, 0x41,
0x44, 0x45, 0x48, 0x49, 0x4C, 0x4D, 0x50, 0x51, 0x54, 0x55, 0x58, 0x59, 0x5C, 0x5D, 0x60, 0x61,
0x64, 0x65, 0x68, 0x69, 0x6C, 0x6D, 0x70, 0x71, 0x74, 0x75, 0x78, 0x79, 0x7C, 0x7D, 0x80, 0x81,
0x84, 0x85, 0x88, 0x89, 0x8C, 0x8D, 0x90, 0x91, 0x94, 0x95, 0x98, 0x99, 0x9C, 0x9D, 0xA0, 0xA1,
0xA4, 0xA5, 0xA8, 0xA9, 0xAC, 0xAD, 0xB0, 0xB1, 0xB4, 0xB5, 0xB8, 0xB9, 0xBC, 0xBD, 0xC0, 0xC1,
0xC4, 0xC5, 0xC8, 0xC9, 0xCC, 0xCD, 0xD0, 0xD1, 0xD4, 0xD5, 0xD8, 0xD9, 0xDC, 0xDD, 0xE0, 0xE1,
0xE4, 0xE5, 0xE8, 0xE9, 0xEC, 0xED, 0xF0, 0xF1, 0xF4, 0xF5, 0xF8, 0xF9, 0xFC, 0xFD, 0xFE, 0xFF
};

PAIRED_PAGE_MAP_t ga_tPairedPageMap[512] = {{0,0}};

#define VENDOR_FUJITSU            0x04
#define VENDOR_RENESAS            0x07
#define VENDOR_ST                 0x20
#define VENDOR_MICRON             0x2C
#define VENDOR_NATIONAL           0x8F
#define VENDOR_TOSHIBA            0x98
#define VENDOR_HYNIX              0xAD
#define VENDOR_SAMSUNG            0xEC
#define VENDOR_MXIC               0xC2
#define VENDOR_ZENTEL             0x92
#define VENDOR_ZENTEL1            0xC8



int drvNAND_CHECK_FLASH_TYPE(void)
{
  NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    
  U16 u16_i,u16_j=0;
  U8 u8_more_maker=0;

  if(pNandDrv->au8_ID[0] ==0x7F) 
  {
      u8_more_maker=1;
  }
  
  if(u8_more_maker)
  { 
      for (u16_i=1; u16_i<NAND_ID_BYTE_CNT;u16_i++) 
      {	  
         if (pNandDrv->au8_ID[u16_i] == 0x7F)
         {
             u16_j = u16_i;
         }
         else
         {
       	     pNandDrv->au8_ID[u16_i-u16_j-1] = pNandDrv->au8_ID[u16_i];
         }		         
      }
      pNandDrv->u8_IDByteCnt -= (u16_j+1);
  }
  
  printf("NAND ID:");
  for(u16_i = 0; u16_i < pNandDrv->u8_IDByteCnt; u16_i++)
      printf("0x%X ", pNandDrv->au8_ID[u16_i]);
  printf("\n");

	
  if( (pNandDrv->au8_ID[0] != VENDOR_SAMSUNG) &&
    (pNandDrv->au8_ID[0] != VENDOR_TOSHIBA) &&
    (pNandDrv->au8_ID[0] != VENDOR_RENESAS) &&
    (pNandDrv->au8_ID[0] != VENDOR_HYNIX)  &&
    (pNandDrv->au8_ID[0] != VENDOR_FUJITSU) &&
    (pNandDrv->au8_ID[0] != VENDOR_MICRON) &&
    (pNandDrv->au8_ID[0] != VENDOR_NATIONAL)  &&
    (pNandDrv->au8_ID[0] != VENDOR_ST) &&   
    (pNandDrv->au8_ID[0] != VENDOR_MXIC) &&
    (pNandDrv->au8_ID[0] != VENDOR_ZENTEL) &&
    (pNandDrv->au8_ID[0] != VENDOR_ZENTEL1))
  {
    pNandDrv->u16_BlkCnt = 0;
    pNandDrv->u16_BlkPageCnt = 0;
    pNandDrv->u16_PageByteCnt = 0;
    pNandDrv->u16_SectorByteCnt = 0;
    pNandDrv->u16_SpareByteCnt = 0;


    printf("Unsupport Vendor %02X\n", pNandDrv->au8_ID[0] );

    return -1; // unsupported flash maker
  }

  pNandDrv->u16_ECCType = ECC_TYPE_4BIT;
	pNandDrv->u8_PlaneCnt = 1;

	/*Default NAND Timing Setting*/
  	pNandDrv->u16_tRC = 30;
  	pNandDrv->u8_tREA = 20;
  	pNandDrv->u8_tRP	= 12;
  	pNandDrv->u8_tREH = 10;
  	pNandDrv->u16_tWC = 25;
  	pNandDrv->u8_tWP = 12;
  	pNandDrv->u8_tWH = 10;	
	pNandDrv->u16_tWW = 100;
  switch(u8FSTYPE[pNandDrv->au8_ID[1]])
  {
    case 0:
      pNandDrv->u16_BlkCnt = 0;
      pNandDrv->u16_BlkPageCnt = 0;
      pNandDrv->u16_PageByteCnt = 0;
      pNandDrv->u16_SectorByteCnt = 0;
      pNandDrv->u16_SpareByteCnt = 0;
      break;
    case 2:
      pNandDrv->u16_BlkCnt = 512;
      pNandDrv->u16_BlkPageCnt = 16;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      break;
    case 4:
      pNandDrv->u16_BlkCnt = 1024;
      pNandDrv->u16_BlkPageCnt = 16;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C3TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
      break;
    case 6:
      pNandDrv->u16_BlkCnt = 1024;
      pNandDrv->u16_BlkPageCnt = 32;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C3TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
      break;
    case 8:
      pNandDrv->u16_BlkCnt = 2048;
      pNandDrv->u16_BlkPageCnt = 32;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C3TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
      break;
    case 10:
      pNandDrv->u16_BlkCnt = 4096;
      pNandDrv->u16_BlkPageCnt = 32;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
      break;
    case 12:
      pNandDrv->u16_BlkCnt = 8192;
      pNandDrv->u16_BlkPageCnt = 32;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
      break;
    case 13:
      pNandDrv->u16_BlkCnt = 1024;
      pNandDrv->u16_BlkPageCnt = 64;
      pNandDrv->u16_PageByteCnt = 2048;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 64;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
      break;
    case 14:
	  pNandDrv->u16_PageByteCnt = 1024 << (pNandDrv->au8_ID[3] & 3);
	  pNandDrv->u16_BlkPageCnt  = ((64 * 1024)  << ((pNandDrv->au8_ID[3] >> 4) & 3)) /  pNandDrv->u16_PageByteCnt;
	  pNandDrv->u16_SectorByteCnt = 512;
	  pNandDrv->u16_SpareByteCnt = (8 << (( pNandDrv->au8_ID[3] >> 2)& 0x01)) * ( pNandDrv->u16_PageByteCnt>>9);
	  pNandDrv->u16_BlkCnt = 4096 / (pNandDrv->u16_PageByteCnt/2048);
	  pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
	  pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
	  break;

    case 15:
      pNandDrv->u16_BlkCnt = 2048;
      pNandDrv->u16_BlkPageCnt = 64;
      pNandDrv->u16_PageByteCnt = 2048;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 64;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
      break;
    case 16:

      if((pNandDrv->au8_ID[0]==VENDOR_SAMSUNG))
       {
         pNandDrv->u16_BlkCnt = 4096;
         pNandDrv->u16_BlkPageCnt = 64;
         pNandDrv->u16_PageByteCnt = 4096;
         pNandDrv->u16_SectorByteCnt = 512;
         pNandDrv->u16_SpareByteCnt = 128;
       }
      else if((pNandDrv->au8_ID[0]==VENDOR_HYNIX))
       {
         pNandDrv->u16_BlkCnt = 4096;
         pNandDrv->u16_BlkPageCnt = 128;
         pNandDrv->u16_PageByteCnt = 2048;
         pNandDrv->u16_SectorByteCnt = 512;
         pNandDrv->u16_SpareByteCnt = 64;
       }
      else if((pNandDrv->au8_ID[0]==VENDOR_MICRON))
      {
		pNandDrv->u16_BlkCnt = 2048;
		pNandDrv->u16_BlkPageCnt = 128;
		pNandDrv->u16_PageByteCnt = 4096;
		pNandDrv->u16_SectorByteCnt = 512;
		pNandDrv->u16_SpareByteCnt = 128;
      }
      else if((pNandDrv->au8_ID[0]==VENDOR_TOSHIBA))
      {
		pNandDrv->u16_BlkCnt = 4096;
		pNandDrv->u16_BlkPageCnt = 64;
		pNandDrv->u16_PageByteCnt = 4096;
		pNandDrv->u16_SectorByteCnt = 512;
		pNandDrv->u16_SpareByteCnt = 128;
      }
      else if(pNandDrv->au8_ID[0] != VENDOR_ST)
      {
        pNandDrv->u16_BlkCnt = 2048;
        pNandDrv->u16_BlkPageCnt = 64;
        pNandDrv->u16_PageByteCnt = 2048;
        pNandDrv->u16_SectorByteCnt = 512;
        pNandDrv->u16_SpareByteCnt = 64;
      }
      else
      {
        pNandDrv->u16_BlkCnt = 8192;
        pNandDrv->u16_BlkPageCnt = 64;
        pNandDrv->u16_PageByteCnt = 2048;
        pNandDrv->u16_SectorByteCnt = 512;
        pNandDrv->u16_SpareByteCnt = 64;
      }
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
      break;
    case 17:
	  if ((pNandDrv->au8_ID[2] & 0xC) == 0)  // for SLC
	  {
	      if(pNandDrv->au8_ID[0] != VENDOR_SAMSUNG)
		  {
		      pNandDrv->u16_BlkCnt = 8192;
			  pNandDrv->u16_BlkPageCnt = 128;
			  pNandDrv->u16_PageByteCnt = 2048;
			  pNandDrv->u16_SectorByteCnt = 512;
			  pNandDrv->u16_SpareByteCnt = 64;
		  }
	      else
		  {
			  pNandDrv->u16_BlkCnt = 16384;
			  pNandDrv->u16_BlkPageCnt = 64;
			  pNandDrv->u16_PageByteCnt = 2048;
			  pNandDrv->u16_SectorByteCnt = 512;
			  pNandDrv->u16_SpareByteCnt = 64;
		  }
	  }
	  else  // for MLC
	  {
	      U8 u8PageSize, u8Value;

		  pNandDrv->u16_PageByteCnt = 2048 << (pNandDrv->au8_ID[3] & 0x3);
		  u8PageSize = (2 << (pNandDrv->au8_ID[3] & 0x3));
		  u8Value = ((pNandDrv->au8_ID[3] >> 4) & 0x3) | ((pNandDrv->au8_ID[3] >> 5) & 0x4);

		  if (pNandDrv->au8_ID[0] == VENDOR_SAMSUNG)
		  {
		      pNandDrv->u16_BlkCnt = 2076;
			  pNandDrv->u16_BlkPageCnt = (128 << u8Value) / u8PageSize;
		  }
		  else if (pNandDrv->au8_ID[0] == VENDOR_HYNIX)
		  {
			  pNandDrv->u16_BlkCnt = 1024;

			  if (u8Value == 0x0)
			  {
			      pNandDrv->u16_BlkPageCnt = 128 / u8PageSize;
			  }
			  else if (u8Value == 0x1)
			  {
				  pNandDrv->u16_BlkPageCnt = 256 / u8PageSize;
			  }
			  else if (u8Value == 0x2)
			  {
				  pNandDrv->u16_BlkPageCnt = 512 / u8PageSize;
			  }
			  else if (u8Value == 0x3) // 768 is not power of 2, should fix according to specific chip
			  {
				  pNandDrv->u16_BlkPageCnt = 768 / u8PageSize;
			  }
			  else if (u8Value == 0x4)
			  {
				  pNandDrv->u16_BlkPageCnt = 1024 / u8PageSize;
			  }
			  else if (u8Value == 0x5)
			  {
				  pNandDrv->u16_BlkPageCnt = 2048 / u8PageSize;
			  }
		   }

		   pNandDrv->u16_SectorByteCnt = 1024;
		   pNandDrv->u16_SpareByteCnt = 432;
		   pNandDrv->u16_ECCType = ECC_TYPE_24BIT1KB;
	  }
	  pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
	  break;
    case 18:
      pNandDrv->u16_BlkCnt = 16384;
      pNandDrv->u16_BlkPageCnt = 32;
      pNandDrv->u16_PageByteCnt = 512;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 16;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
      break;
    case 20:
      pNandDrv->u16_BlkCnt = 1024;
      pNandDrv->u16_BlkPageCnt = 64;
      pNandDrv->u16_PageByteCnt = 2048;
      pNandDrv->u16_SectorByteCnt = 512;
      pNandDrv->u16_SpareByteCnt = 64;
      pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
      pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
      break;
    default:
      pNandDrv->u16_BlkCnt = 0;
      pNandDrv->u16_BlkPageCnt = 0;
      pNandDrv->u16_PageByteCnt = 0;
      pNandDrv->u16_SectorByteCnt = 0;
      pNandDrv->u16_SpareByteCnt = 0;
	  return -1;
      break;
  }

  return 0;
}

U32 NC_PlatformResetPre(void)
{
  //HalMiuMaskReq(MIU_CLT_FCIE);
  return UNFD_ST_SUCCESS;
}

U32 NC_PlatformResetPost(void)
{
	U16 u16_Reg=0;
	u16_Reg=u16_Reg;
	#if defined(NC_SEL_FCIE3) && NC_SEL_FCIE3
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    
	REG_READ_UINT16(FCIE_NC_CIFD_BASE, u16_Reg); // dummy read for CIFD clock
	
	REG_WRITE_UINT16(NC_WIDTH, FCIE_REG41_VAL);  // duty cycle 3:1 in 86Mhz (depends on chip config)
	
	
	#if !defined(CONFIG_MSTAR_KRONUS)
	// miu eco
	#if !(defined(CONFIG_MSTAR_AMBER5) || ENABLE_AMBER3 || ENABLE_AGATE || ENABLE_EAGLE)
	REG_SET_BITS_UINT16(NC_REG_2Fh, BIT0);
	#endif
	#else
	// Why miu eco is different in K1?
	// 1. The register is changed from 0x2F BIT[0] to 0x2D BIT[7].
	// 2. There is a bug in 0x2D:
	//    The original value of 0x2D is 0x1F.
	//    If we set the BIT[1:0] = 0x3, the BIT[9:8] will also be 0x3
	//    If the the BIT[9:8] is set to 0x3, the byte reorder will be enabled. So we will fail in READ_ID.
	//    So we need to force setting 0x2D as 0x90.
	REG_WRITE_UINT16(FCIE_NC_REORDER, 0x90);
	#endif
	
    #if defined (DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	#else
	#if defined(REG57_ECO_FIX_INIT_VALUE)
	pNandDrv->u16_Reg57_RELatch = REG57_ECO_FIX_INIT_VALUE;
	REG_WRITE_UINT16(NC_LATCH_DATA, REG57_ECO_FIX_INIT_VALUE);
	#endif
	
	#endif

    #elif defined(NC_SEL_FCIE5) && NC_SEL_FCIE5

    REG_WRITE_UINT16(NC_FUN_CTL, BIT_NC_EN);

    REG_READ_UINT16(FCIE_NC_WBUF_CIFD_BASE, u16_Reg); // dummy read for WCIFD clock
    REG_READ_UINT16(FCIE_NC_WBUF_CIFD_BASE, u16_Reg); // dummy read for WCIFD clock

    REG_READ_UINT16(FCIE_NC_RBUF_CIFD_BASE, u16_Reg); // dummy read for RCIFD clock twice said by designer
    REG_READ_UINT16(FCIE_NC_RBUF_CIFD_BASE, u16_Reg); // dummy read for RCIFD clock twice said by designer

    #endif  //defined(NC_SEL_FCIE3) && NC_SEL_FCIE3


	//HalMiuUnMaskReq(MIU_CLT_FCIE);
	
	return UNFD_ST_SUCCESS;
}


U32 NC_PlatformInit(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	nand_pads_init();
	pNandDrv->u8_WordMode = 0; // TV/Set-Top Box projects did not support x16 NAND flash
	nand_pads_switch(1);

	#if defined(NC_SEL_FCIE3) && NC_SEL_FCIE3 
	REG_WRITE_UINT16(NC_PATH_CTL, 0x20);
	#if defined (DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	pNandDrv->u32_Clk =FCIE3_SW_SLOWEST_CLK;
	nand_clock_setting(FCIE3_SW_SLOWEST_CLK);
	#else
	pNandDrv->u32_Clk =FCIE3_SW_DEFAULT_CLK;
	nand_clock_setting(FCIE3_SW_DEFAULT_CLK);
	#endif
	// print clock setting
	//nand_debug(UNFD_DEBUG_LEVEL, 0,"reg_ckg_fcie(%08X)=%08X\n", reg_ckg_fcie, REG(reg_ckg_fcie));
	#elif defined(NC_SEL_FCIE5) && NC_SEL_FCIE5
 	pNandDrv->u8_MacroType = MACRO_TYPE_32BIT;
  	pNandDrv->u32_Clk =FCIE3_SW_SLOWEST_CLK;
    nand_clock_setting(FCIE3_SW_SLOWEST_CLK);
    pNandDrv->u16_Reg57_RELatch = BIT_NC_LATCH_DATA_2_0_T|BIT_NC_LATCH_STS_2_0_T;
    REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
    #endif


	pNandDrv->pu8_PageDataBuf= (U8*)drvNandPageBuf;
	pNandDrv->pu8_PageSpareBuf = (U8*)drvNandSpareBuf;
	// no shared-bus with Disp
	pNandDrv->u8_SwPatchWaitRb= 0;
	pNandDrv->u8_SwPatchJobStart= 0;

	return UNFD_ST_SUCCESS;
}

/* 
void HWTimer_Start(void) 
{
    // reset HW timer
    REG_WRITE_UINT16(TIMER0_MAX_LOW, 0xFFFF);
    REG_WRITE_UINT16(TIMER0_MAX_HIGH, 0xFFFF);
    REG_WRITE_UINT16(TIMER0_ENABLE, 0);

    // start HW timer
    REG_SET_BITS_UINT16(TIMER0_ENABLE, 0x0001);

}

U32 HWTimer_End(void)
{
    U32 u32HWTimer = 0;
    U16 u16TimerLow = 0;
    U16 u16TimerHigh = 0;

    // Get HW timer
    REG_READ_UINT16(TIMER0_CAP_LOW, u16TimerLow);
    REG_READ_UINT16(TIMER0_CAP_HIGH, u16TimerHigh);

    u32HWTimer = (u16TimerHigh<<16) | u16TimerLow;

    return u32HWTimer;
}
*/
#else
  #error "Error! no platform functions."
#endif
