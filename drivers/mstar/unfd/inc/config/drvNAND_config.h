#ifndef __UNFD_CONFIG_H__
#define __UNFD_CONFIG_H__


//=====================================================
// select a HW platform:
//   - 1: enable, 0: disable.
//   - only one platform can be 1, others have to be 0.
//=====================================================
#define NAND_DRV_TV_MBOOT     1


//=====================================================
// do NOT edit the following content.
//=====================================================
#include <config.h>
#if defined(CONFIG_ARCH_CEDRIC)
#define  ENABLE_CEDRIC		1
#elif defined(CONFIG_INFINITY) || defined(CONFIG_INFINITY3)
#define ENABLE_INFINITY		  1
#endif

#if defined(NAND_DRV_TV_MBOOT) && NAND_DRV_TV_MBOOT
  #include <config.h>
  #if (ENABLE_CEDRIC)
  #include "drvNAND_cedric_uboot.h"
  #elif (ENABLE_INFINITY)
  #include "drvNAND_infinity_uboot.h"
  #else
  #error "Wrong chip setting!"
  #endif
#else
  #error "Error! no platform selected."
#endif

//do NOT edit the following content, for HAL use.

//=====================================================
// misc. do NOT edit the following content.
//=====================================================
#define NAND_DMA_RACING_PATCH        1
#define NAND_DMA_PATCH_WAIT_TIME     10000 // us -> 10ms
#define NAND_DMA_RACING_PATTERN0     (((U32)'M'<<24)|((U32)0<<16)|((U32)'S'<<8)|(U32)1)
#define NAND_DMA_RACING_PATTERN1     (((U32)'T'<<24)|((U32)6<<16)|((U32)'A'<<8)|(U32)8)



//===========================================================
// Time Dalay, do NOT edit the following content, for NC_WaitComplete use.
//===========================================================
#define DELAY_100us_in_us  100
#define DELAY_500us_in_us  500
#define DELAY_1ms_in_us    1000
#define DELAY_10ms_in_us   10000
#define DELAY_100ms_in_us  100000
#define DELAY_500ms_in_us  500000
#define DELAY_1s_in_us     1000000

#define WAIT_ERASE_TIME    (DELAY_1s_in_us<<2)
#define WAIT_WRITE_TIME    (DELAY_1s_in_us<<2)
#define WAIT_READ_TIME     (DELAY_1ms_in_us<<10)  //DELAY_500us_in_us
#define WAIT_PROBE_TIME    (DELAY_100us_in_us<<2)
#define WAIT_RESET_TIME    (WAIT_ERASE_TIME)

//for system cache direction calls
#define WRITE_TO_NAND			0
#define READ_FROM_NAND			1
#define BIDIRECTIONAL			    2

extern void nand_reset_WatchDog(void);
extern U32 nand_clock_setting(U32 u32ClkParam);
extern U32 nand_config_clock(U16 u16_SeqAccessTime);
extern U32 nand_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int Mode);
extern U32 nand_translate_Spare_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode);
extern U32 nand_translate_DMA_MIUSel(U32 u32_DMAAddr, U32 u32_ByteCnt);
extern void nand_read_dma_post_flush(U32 u32_DMAAddr, U32 u32_ByteCnt);
extern void nand_Invalidate_data_cache_buffer(U32 u32_addr, S32 s32_size);
extern U32 nand_WaitCompleteIntr(U16 u16_WaitEvent, U32 u32_MicroSec);
extern void nand_enable_intr_mode(void);
extern void nand_flush_miu_pipe(void);



extern int drvNAND_CHECK_FLASH_TYPE(void);

#define NAND_PARTITAION_BYTE_CNT    0x200
#endif /* __UNFD_CONFIG_H__ */
