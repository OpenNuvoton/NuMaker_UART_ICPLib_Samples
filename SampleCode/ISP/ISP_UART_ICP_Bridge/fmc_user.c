/***************************************************************************//**
 * @file     fmc_user.c
 * @brief    M480 series FMC driver source file
 * @version  2.0.0
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "fmc_user.h"
#include "ICPLib.h"

int FMC_Write_User(uint32_t u32Addr, uint32_t u32Data)
{
		ICP_ProgramFlash(u32Addr, 4, &u32Data);
    return 0;
}

int FMC_Read_User(uint32_t u32Addr, uint32_t *data)
{
		ICP_ReadFlash(u32Addr, 4, data);
    return 0;
}

int FMC_Erase_User(uint32_t u32Addr)
{
		ICP_PageEraseFlash(u32Addr);
		return 0;
}

void ReadData(uint32_t addr_start, uint32_t addr_end, uint32_t *data)    // Read data from flash
{
    ICP_ReadFlash(addr_start, addr_end - addr_start, data);
    return;
}

void WriteData(uint32_t addr_start, uint32_t addr_end, uint32_t *data)  // Write data into flash
{
		ICP_ProgramFlash(addr_start, addr_end - addr_start, data);
    return;
}

#define FMC_BLOCK_SIZE           (FMC_FLASH_PAGE_SIZE * 4UL)

int EraseAP(uint32_t addr_start, uint32_t size)
{
    uint32_t u32Addr, u32Size;
    u32Addr = addr_start;

    while (size > 0)
    {
        if ((size >= FMC_BANK_SIZE) && !(u32Addr & (FMC_BANK_SIZE - 1)))
        {
            u32Size = FMC_BANK_SIZE;
						ICP_BankErase(u32Addr);
        }
        else if ((size >= FMC_BLOCK_SIZE) && !(u32Addr & (FMC_BLOCK_SIZE - 1)))
        {
            u32Size = FMC_BLOCK_SIZE;
					  ICP_BlockErase(u32Addr);
        }
        else
        {
            u32Size = FMC_FLASH_PAGE_SIZE;
						ICP_PageEraseFlash(u32Addr);
        }
				
        u32Addr += u32Size;
        size -= u32Size;
    }

    return 0;
}

void UpdateConfig(uint32_t *data, uint32_t *res)
{
		ICP_ProgramFlash(Config0, 16, data);

    if (res)
    {
				ICP_ReadFlash(Config0, 16, res);
    }
}
