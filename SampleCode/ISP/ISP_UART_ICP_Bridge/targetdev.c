/***************************************************************************//**
 * @file     targetdev.c
 * @brief    ISP support function source file
 * @version  0x32
 * @date     14, June, 2017
 *
 * @note
 * Copyright (C) 2017-2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "targetdev.h"
#include "isp_user.h"
#include "ICPLib.h"

uint32_t GetApromSize()
{
		// default value, may need update
		return FMC_APROM_SIZE;
}

// Data Flash is shared with APROM.
// The size and start address are defined in CONFIG1.
void GetDataFlashInfo(uint32_t *addr, uint32_t *size)
{
    uint32_t uData;
    *size = 0;

		ICP_ReadFlash(Config0, 4, &uData);

    if ((uData & 0x01) == 0)   //DFEN enable
    {
				ICP_ReadFlash(Config1, 4, &uData);

        // Filter the reserved bits in CONFIG1
        uData &= 0x000FFFFF;

        if (uData > g_apromSize || uData & (FMC_FLASH_PAGE_SIZE - 1))   //avoid config1 value from error
        {
            uData = g_apromSize;
        }

        *addr = uData;
        *size = g_apromSize - uData;
    }
    else
    {
        *addr = g_apromSize;
        *size = 0;
    }
}
