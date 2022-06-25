#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	/* arch number*/
	gd->bd->bi_arch_number = MACH_TYPE_CHICAGO;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = BOOT_PARAMS;

    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}
