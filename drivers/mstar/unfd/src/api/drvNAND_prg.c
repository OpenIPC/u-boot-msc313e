#include "drvNAND.h"
#include "drvNAND_utl.h"
#include <linux/string.h>

extern int sprintf(char *buf, const char *fmt, ...)
__attribute__ ((format (__printf__, 2, 3)));

U32 drvNAND_GetNandID(U8 *pu8IDByteCnt, U8 *pu8ID)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();

	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "\n");

	*pu8IDByteCnt = pNandDrv->u8_IDByteCnt;
	memcpy(pu8ID, pNandDrv->au8_ID, NAND_ID_BYTE_CNT);
 
	return UNFD_ST_SUCCESS;
}

U32 drvNAND_CheckCIS(U8 *pu8_CISData)
{
	NAND_FLASH_INFO_t *pNandInfo = (NAND_FLASH_INFO_t*)pu8_CISData;
	PARTITION_INFO_t *pPartInfo = (PARTITION_INFO_t*)(pu8_CISData + 0x200);
	U32 u32_ChkSum;

	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "\n");

	if (drvNAND_CompareCISTag(pNandInfo->au8_Tag)) {
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Tag mismatch\n");
		return UNFD_ST_ERR_CIS_NAND_ERR;
	}

	u32_ChkSum = drvNAND_CheckSum(pu8_CISData + 0x24, 0x32 - 0x24);
	if (u32_ChkSum != pNandInfo->u32_ChkSum) {

		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NAND Info chksum mismatch: 0x%08x, 0x%08x\n",
				   u32_ChkSum, pNandInfo->u32_ChkSum);

		dump_mem(pu8_CISData, 0x200);
		return UNFD_ST_ERR_CIS_NAND_ERR;
	}

	u32_ChkSum = drvNAND_CheckSum(pu8_CISData + 0x200 + 0x04, 0x200 - 0x04);
	if (u32_ChkSum != pPartInfo->u32_ChkSum) {

		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Part Info chksum mismatch: 0x%08x, 0x%08x\n",
				   u32_ChkSum, pPartInfo->u32_ChkSum);

		dump_mem(pu8_CISData + 0x200, 0x200);
		return UNFD_ST_ERR_CIS_PART_ERR;
	}

	dump_nand_info(pNandInfo);
	dump_part_info(pPartInfo);

	return UNFD_ST_SUCCESS;
}

//U32 drvNAND_WriteCIS_for_ROM_2(NAND_FLASH_INFO_t * pNandInfo)
U32 drvNAND_WriteCIS_for_ROM_2(U8 *pu8_CISData)  
{
    NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
    U8 *au8_PageBuf = (U8*)pNandDrv->pu8_PageDataBuf;
    U8 *au8_SpareBuf = (U8*)pNandDrv->pu8_PageSpareBuf;
  	NAND_FLASH_INFO_t *pNandInfo = (NAND_FLASH_INFO_t*)pu8_CISData;
		PARTITION_INFO_t *pPartInfo = (PARTITION_INFO_t*)(pu8_CISData + 0x200);
    BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
    U32 u32_Err = UNFD_ST_SUCCESS;
    U16 u16_PBA;
    U32 u32_PageIdx;
    U8 u8_CISIdx;

    nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "\n");

	
//    memset(pNandDrv->au8_ID, '\0', NAND_ID_BYTE_CNT);
//    memcpy(pNandDrv->au8_ID, pNandInfo->au8_ID, pNandInfo->u8_IDByteCnt);
    pNandDrv->u8_IDByteCnt     	= pNandInfo->u8_IDByteCnt;
    pNandDrv->u16_SpareByteCnt 	= pNandInfo->u16_SpareByteCnt;
    pNandDrv->u16_PageByteCnt  	= pNandInfo->u16_PageByteCnt;
    pNandDrv->u16_BlkPageCnt   	= pNandInfo->u16_BlkPageCnt;
    pNandDrv->u16_BlkCnt       	= pNandInfo->u16_BlkCnt;
    pNandDrv->u16_ECCType      	= pNandInfo->u16_ECCType;
    pNandDrv->u32_Config       	= pNandInfo->u32_Config;
    pNandDrv->u16_tRC			= pNandInfo->u16_tRC;
    //memcpy(pNandDrv->u8_Vendor, pNandInfo->u8_Vendor, 16);
    //memcpy(pNandDrv->u8_PartNumber, pNandInfo->u8_PartNumber, 16);
    pNandDrv->u8_CellType 		= pNandInfo->u32_Config&0x01;
    pNandDrv->u8_PairPageMapLoc = pNandInfo->u8_PairPageMapLoc = ga_tPairedPageMap[3].u16_LSB;
	

#if 1
//		u32_Err = NC_EraseBlk(0);
//        if (u32_Err != UNFD_ST_SUCCESS)
//            nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase blk 0 failed with EC: 0x%08lx\n",u32_Err);
#else
		for (u16_PBA = 0; u16_PBA < pNandDrv->u16_BlkCnt; u16_PBA++)
		{
				printf("NC_EraseBlk %X\r\n", u16_PBA);
		    u32_Err = NC_EraseBlk(u16_PBA << pNandDrv->u8_BlkPageCntBits);
        if (u32_Err != UNFD_ST_SUCCESS) {
            nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase blk 0x%04x failed with EC: 0x%08lx\n",
                       u16_PBA, u32_Err);
        }
		}
		//while(1);
#endif

//    dump_nand_info(pNandInfo);
    NC_ConfigNandFlashContext();
    NC_RegInit();
		NC_Config();
	
    nand_config_clock(pNandInfo->u16_tRC); 
	
    memset(pNandDrv->au8_ID, '\0', NAND_ID_BYTE_CNT);
    memcpy(pNandDrv->au8_ID, pNandInfo->au8_ID, pNandInfo->u8_IDByteCnt);

	#if defined(FCIE4_DDR) && FCIE4_DDR
	memcpy((void *) &pNandInfo->tDefaultDDR, (const void *) &pNandDrv->tDefaultDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandInfo->tMaxDDR, (const void *) &pNandDrv->tMaxDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandInfo->tMinDDR, (const void *) &pNandDrv->tMinDDR, sizeof(DDR_TIMING_GROUP_t));
	#endif
	
	u8_CISIdx = 0;

    /* Search for two good blocks within the first 10 physical blocks */
    for (u16_PBA = 0; u16_PBA < 10; u16_PBA++) 
    {
        /* Reeset NAND driver and FCIE to the original settings */
        pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
        pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
        pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
        NC_ConfigNandFlashContext();
        NC_RegInit();
        pNandDrv->u16_Reg48_Spare &= ~(1 << 12);
				NC_Config();

        /* Check first page of block */
        u32_PageIdx = u16_PBA << pNandDrv->u8_BlkPageCntBits;
        u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
        if (u32_Err != UNFD_ST_SUCCESS)
            nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_ReadSectors(0x%lX)=0x%lX\n", u32_PageIdx, u32_Err);
            
        if (au8_SpareBuf[0] != 0xFF)
        {
            nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip bad blk 0x%04x\n", u16_PBA);
            continue;
        }

        u32_Err = NC_EraseBlk(u16_PBA << pNandDrv->u8_BlkPageCntBits);
        if (u32_Err != UNFD_ST_SUCCESS) {
            nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase blk 0x%04x failed with EC: 0x%08lx\n",
                       u16_PBA, u32_Err);

            drvNAND_MarkBadBlk(u16_PBA);
            continue;
        }

        pNandDrv->u16_PageByteCnt = 2048;
        pNandDrv->u16_SpareByteCnt = 256;
        pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;
        
        NC_ConfigNandFlashContext();
        NC_RegInit();

        pNandDrv->u16_Reg48_Spare |= (1 << 12);
				NC_Config();

        memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
        memcpy(au8_PageBuf, pNandInfo, 512);
        memset(au8_SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt);

        pBlkInfo->u8_BadBlkMark = 0xFF;
        pBlkInfo->u8_PartType = 0;

        u32_Err = NC_WriteSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
        
        if (u32_Err != UNFD_ST_SUCCESS)
        {
            nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Write Nand Info failed with EC: 0x%08lx\n", u32_Err);
            drvNAND_MarkBadBlk(u16_PBA);
            continue;
        }

        /* Reset NAND driver and FCIE to the original settings */
		pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
        pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
        pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
        NC_ConfigNandFlashContext();
        NC_RegInit();
        pNandDrv->u16_Reg48_Spare &= ~BIT_NC_HW_AUTO_RANDOM_CMD_DISABLE;
		NC_Config();
		
#if 1
		/*
		 *  Write Partition Info to the 2nd page
		 */
		memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
		memcpy(au8_PageBuf, pPartInfo, 512);
		u32_Err = NC_WriteSectors(u32_PageIdx+1, 0, au8_PageBuf, au8_SpareBuf, 1);
#endif
		
		
        /*
		**  Write Paired Page Map to the 4th page
		**/
        if(pNandDrv->u8_CellType == 1)  // MLC
        {
            memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
            memcpy(au8_PageBuf, &ga_tPairedPageMap, 2048);
			u32_Err = NC_WritePages(u32_PageIdx+pNandInfo->u8_PairPageMapLoc, au8_PageBuf, au8_SpareBuf, 1);
            if (u32_Err != UNFD_ST_SUCCESS) {
                nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Write Paired-Page Map failed with EC: 0x%08lx\n", u32_Err);
                drvNAND_MarkBadBlk(u16_PBA);
                continue;
            }
        }

        printf("CIS%d is written to blk 0x%04x\n", u8_CISIdx, u16_PBA);

		pNandDrv->u16_CISPBA[u8_CISIdx] = u16_PBA;

        if ((++u8_CISIdx) == 2)
            break;
    }

    /* Reset NAND driver and FCIE to the original settings */
    pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
    pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
    pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
    NC_ConfigNandFlashContext();
    NC_RegInit();

    pNandDrv->u16_Reg48_Spare &= ~(1 << 12);
	NC_Config();

    switch (u8_CISIdx) {
    case 0:
        u32_Err = UNFD_ST_ERR_NO_BLK_FOR_CIS0;
        break;
    case 1:
        u32_Err = UNFD_ST_ERR_NO_BLK_FOR_CIS1;
        break;
    case 2:
        u32_Err = UNFD_ST_SUCCESS;
        break;
    }

    return u32_Err;
}


U32 drvNAND_GetNandInfo(NAND_INFO_t *pNandInfo_t)
{
    NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
    const char *str = "MSTARSEMIUNFDCIS";

    memcpy(pNandInfo_t->au8_Tag, 	str, 16);
    pNandInfo_t->u8_IDByteCnt = pNandDrv->u8_IDByteCnt;
    memcpy(pNandInfo_t->au8_ID, pNandDrv->au8_ID, pNandDrv->u8_IDByteCnt);
    pNandInfo_t->u16_SpareByteCnt = pNandDrv->u16_SpareByteCnt;
    pNandInfo_t->u16_PageByteCnt = pNandDrv->u16_PageByteCnt;
    pNandInfo_t->u16_BlkPageCnt = pNandDrv->u16_BlkPageCnt;
    pNandInfo_t->u16_BlkCnt = pNandDrv->u16_BlkCnt;
    pNandInfo_t->u32_Config = pNandDrv->u32_Config;
    pNandInfo_t->u16_ECCType = pNandDrv->u16_ECCType;
    pNandInfo_t->u32_ChkSum=  drvNAND_CheckSum((U8*)&pNandInfo_t->u16_SpareByteCnt, 0x32 - 0x24);
    pNandInfo_t->u16_tRC = pNandDrv->u16_tRC;

    memcpy(pNandInfo_t->u8_Vendor, pNandDrv->u8_Vendor, 16);
    memcpy(pNandInfo_t->u8_PartNumber, pNandDrv->u8_PartNumber, 16);

	pNandInfo_t->u16_ECCCodeByteCnt = pNandDrv->u16_ECCCodeByteCnt;
	pNandInfo_t->u16_PageSectorCnt = pNandDrv->u16_PageSectorCnt;
	pNandInfo_t->u8_WordMode = pNandDrv->u8_WordMode;
    return UNFD_ST_SUCCESS;
}


U8* drvNAND_SearchNandInfo(U8 *pu8_NandInfoArray, U32 u32_ArraySize)
{
    NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	NAND_FLASH_INFO_t *pNandInfo=NULL;
	U16 u16_NandInfoArrayCnt;
	U16 u16_i;
	const char *str = "MSTARSEMIUNFDCIS";
	
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "\n");

	u16_NandInfoArrayCnt = u32_ArraySize/512;
	for(u16_i=0 ; u16_i<u16_NandInfoArrayCnt ; u16_i++)
	{
		pNandInfo = (NAND_FLASH_INFO_t*)(pu8_NandInfoArray+u16_i*512);
		if( memcmp(pNandInfo->au8_Tag, str, 16) != 0)
		{
			goto _NOT_FOUND;
		}
		if( memcmp(pNandInfo->au8_ID, pNandDrv->au8_ID, pNandInfo->u8_IDByteCnt) == 0 )
			break;
	}

	if(u16_i==u16_NandInfoArrayCnt)
	{
		goto _NOT_FOUND;
	}

	return((U8*)pNandInfo);

_NOT_FOUND:
	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Cannot find a matched NandInfo\n");
	return NULL;

}

U32 drvNAND_GetPartOffset(U16 u16_PartType, U32* u32_Offset)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	PARTITION_INFO_t *pPartInfo = pNandDrv->pPartInfo;
	PARTITION_RECORD_t *pRecord = pPartInfo->records;

	pRecord = drvNAND_SearchPartition(pRecord, u16_PartType);
	if (!pRecord)
	{
		*u32_Offset = 0;
		return UNFD_ST_ERR_INVALID_PART;
	}

	*u32_Offset = pRecord->u16_StartBlk * pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt;
	return UNFD_ST_SUCCESS;
}


void drvNAND_GetMtdParts(char *buf)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	PARTITION_INFO_t *pPartInfo = pNandDrv->pPartInfo;
	U8 u8_i,u8_PartNo;
	int len, Maxlen = 512;
	U32 PartSize = 0;
	char *mtd_buf=buf, tmp[32], PartName[10];
	U16 u16_LastPartType;
	sprintf(mtd_buf, "mtdparts=nand0:");
	//printf("%s\r\n",buf);
	mtd_buf += 15;
	if (Maxlen < 15)
		goto cleanup;
	Maxlen -= 15;
	#if 0
	sprintf(tmp, "0x%X(CIS)", 10 * pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt);
	len = strlen(tmp);
	memcpy(mtd_buf, tmp, len);
	mtd_buf += len;
	if (Maxlen < len)
		goto cleanup;
	Maxlen -= len;
	#endif
	
	u16_LastPartType = 0;
	for(u8_i = 0; u8_i < pPartInfo->u16_PartCnt; u8_i ++)
	{
		if( (pPartInfo->records[u8_i].u16_PartType & UNFD_LOGI_PART) == UNFD_LOGI_PART)
			break;

		PartSize = 0;
		if( u8_i )
		{
		memcpy(mtd_buf, ",", 1);
		mtd_buf ++;
		Maxlen --;
		}
		

		PartSize = pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt *
			(pPartInfo->records[u8_i].u16_BackupBlkCnt + pPartInfo->records[u8_i].u16_BlkCnt);

		u8_PartNo = u16_LastPartType != pPartInfo->records[u8_i].u16_PartType ? 0:1;

		u16_LastPartType = pPartInfo->records[u8_i].u16_PartType;

		switch(pPartInfo->records[u8_i].u16_PartType)
		{
			case UNFD_PART_IPL_CUST:
				sprintf(PartName,"IPL_CUST%d",u8_PartNo );
				break;
			case UNFD_PART_BOOTLOGO:
				sprintf(PartName,"BOOTLOGO" );
				break;				
			case UNFD_PART_IPL:
				sprintf(PartName,"IPL%d",u8_PartNo );
				break;
			case UNFD_PART_CUS:
				sprintf(PartName,"CUS" );
				break;
			case UNFD_PART_UBOOT:
				sprintf(PartName,"UBOOT%d",u8_PartNo );				
				break;
			case UNFD_PART_OS:
				sprintf(PartName,"KERNEL" );
				break;
			case UNFD_PART_SECINFO:
				sprintf(PartName,"SECINFO" );
				break;
			case UNFD_PART_OTP:
				sprintf(PartName,"OTP" );
				break;
			case UNFD_PART_RECOVERY:
				sprintf(PartName,"RECOVERY" );
				break;
			case UNFD_PART_E2PBAK:
				sprintf(PartName,"E2PBAK" );
				break;
			case UNFD_PART_NVRAMBAK:
				sprintf(PartName,"NVRAMBAK" );
				break;
			case UNFD_PART_NPT:
				sprintf(PartName,"NPT" );	
				break;
			case UNFD_PART_ENV:
				sprintf(PartName,"ENV" );				
				break;
			case UNFD_PART_MISC:
				sprintf(PartName,"MISC" );				
				break;				
			default:
				sprintf(PartName,"UNKNOWN%d",u8_i );	
				break;
		}
		if (u8_i)
			sprintf(tmp, "0x%X(%s)", (unsigned int)PartSize, PartName); //,pPartInfo->records[u8_i].u16_PartType);
		else 
			sprintf(tmp, "0x%X@0x%X(%s)", (unsigned int)PartSize, 10 * pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt, PartName); //,pPartInfo->records[u8_i].u16_PartType);
		
		len = strlen(tmp);
		memcpy(mtd_buf, tmp, len);
		mtd_buf += len;
		if (Maxlen < len)
			goto cleanup;
		Maxlen -= len;
		
	}
	sprintf(tmp,",-(UBI)");
	len= strlen(tmp);
	memcpy(mtd_buf, tmp, len);
	mtd_buf += len;
	if (Maxlen < len)
		goto cleanup;
	Maxlen -= len;

	*mtd_buf = '\0';
	return ;
	cleanup:
	buf[0] = '\0';
	return ;
	//printf("%s\r\n",mtd_buf);
}

int drvNAND_ReadCISBlk(U8* pu8_DataBuf)
{
    NAND_DRIVER * pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    U32 u32_Ret, i;
    U32 u32_Row;
    U16 u16_PageByteCntTmp, u16_SpareByteCntTmp, u16_ECCTypeTmp;
	U32 page_off = pNandDrv->u16_PageByteCnt + pNandDrv->u16_SpareByteCnt;
	U8 u8_blkidx;

	for(u8_blkidx = 0; u8_blkidx < 10;  u8_blkidx ++)
	{
		u32_Row = u8_blkidx << pNandDrv->u8_BlkPageCntBits;
		//set 0xff for nni page
		memset((void*)pu8_DataBuf, 0xff, page_off * pNandDrv->u16_BlkPageCnt);
		u16_PageByteCntTmp = pNandDrv->u16_PageByteCnt;
		u16_SpareByteCntTmp = pNandDrv->u16_SpareByteCnt;
		u16_ECCTypeTmp = pNandDrv->u16_ECCType;

		pNandDrv->u16_PageByteCnt = 0x800;
		pNandDrv->u16_SpareByteCnt = 0x100;
		pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;

		#if defined(FCIE_LFSR) && FCIE_LFSR
		if(pNandDrv->u8_RequireRandomizer)
			NC_DisableLFSR();
		#endif

		NC_ConfigNandFlashContext();
		NC_RegInit();
		pNandDrv->u16_Reg48_Spare |= (1 << 12);
		NC_Config();

		u32_Ret = NC_ReadSectors(u32_Row, 0, pu8_DataBuf, (U8*)((U32)pu8_DataBuf + pNandDrv->u16_SectorByteCnt), 1);
		if(u32_Ret != 0)
			return u32_Ret;

		//restore original setting
		pNandDrv->u16_PageByteCnt = u16_PageByteCntTmp;
		pNandDrv->u16_SpareByteCnt = u16_SpareByteCntTmp;
		pNandDrv->u16_ECCType = u16_ECCTypeTmp;

		NC_ConfigNandFlashContext();
		NC_RegInit();
		pNandDrv->u16_Reg48_Spare &= ~(1 << 12);
		NC_Config();

		for(i = 1; i < pNandDrv->u16_BlkPageCnt; i++){
		    u32_Ret = NC_ReadPages(u32_Row + i, (U8*) ((U32)pu8_DataBuf + page_off * i),
		                        (U8*) ((U32)pu8_DataBuf + (page_off * i) + pNandDrv->u16_PageByteCnt), 1);
		    if(u32_Ret)
		        return u32_Ret;
		}
		pu8_DataBuf += pNandDrv->u16_BlkPageCnt * page_off;
	}
	printf("Read total size 0x%X\n", 10*pNandDrv->u16_BlkPageCnt*page_off);
	return UNFD_ST_SUCCESS;
}
