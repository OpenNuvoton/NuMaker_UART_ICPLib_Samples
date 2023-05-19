/***************************************************************************//**
 * @file     main.c
 * @brief    ISP tool main function
 * @version  0x32
 * @date     14, June, 2017
 *
 * @note
 * Copyright (C) 2017-2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "targetdev.h"
#include "uart_transfer.h"
#include "ICPLib.h"

#define PLLCON_SETTING          CLK_PLLCTL_192MHz_HIRC
#define PLL_CLOCK               192000000
#define HCLK_DIV                        1

uint32_t CLK_GetPLLClockFreq(void)
{
    return PLL_CLOCK;
}

void SYS_Init(void)
{
    /* Enable Internal and External RC clock */
    CLK->PWRCTL |= (CLK_PWRCTL_HIRCEN_Msk | CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for Internal RC clock ready */
    while (!(CLK->STATUS & CLK_STATUS_HIRCSTB_Msk));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCTL = PLLCON_SETTING;

    while (!(CLK->STATUS & CLK_STATUS_PLLSTB_Msk));

    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_HCLKDIV_Msk;
    CLK->CLKDIV0 |= CLK_CLKDIV0_HCLK(HCLK_DIV);
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = PLL_CLOCK;                        // PLL
    SystemCoreClock = PLL_CLOCK / HCLK_DIV;             // HCLK
    CyclesPerUs     = SystemCoreClock / 1000000;  // For SYS_SysTickDelay()
    /* Enable UART module clock */
    CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk;
		CLK->APBCLK0 |= CLK_APBCLK0_UART1CKEN_Msk;
    /* Select UART module clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;
		CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART1SEL_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART1SEL_HIRC;
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPA multi-function pins for UART0 (Target) RXD and TXD */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk);
    SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA0MFP_UART0_RXD | SYS_GPA_MFPL_PA1MFP_UART0_TXD);		
		/* Set GPH multi-function pins for UART1 (Bridge) RXD and TXD */
    SYS->GPH_MFPH &= ~(SYS_GPH_MFPH_PH8MFP_Msk | SYS_GPH_MFPH_PH9MFP_Msk);
    SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH9MFP_UART1_RXD | SYS_GPH_MFPH_PH8MFP_UART1_TXD);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
    /* Init UART to 115200-8n1 */
    UART_Init();
    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;
    
    //GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL   = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;//using cpu clock
	
	  ICP_Init(ICP_RESET_ACTIVE_LOW);
		ICP_ModeEntry();
	
		g_apromSize = GetApromSize();
		GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);

    while (1)
    {
        if ((bufhead >= 4) || (bUartDataReady == TRUE))
        {
            uint32_t lcmd;
            lcmd = inpw(uart_rcvbuf);

            if (lcmd == CMD_CONNECT)
            {
                goto _ISP;
            }
            else
            {
                bUartDataReady = FALSE;
                bufhead = 0;
            }
        }

        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            /* Trap the CPU */
						while (1);
        }
    }

_ISP:

    while (1)
    {
        if (bUartDataReady == TRUE)
        {
            bUartDataReady = FALSE;
            ParseCmd(uart_rcvbuf, 64);
            PutString();
        }
    }
}
