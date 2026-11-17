/*
 * sdram_self_test.c
 * RA8P1 + W9812G2KB SDRAM 自检模块
 * 全16MB空间读写校验，用来在硬件/配置变更后快速确认SDRAM工作正常
 */

#include <stdint.h>
#include <stdio.h>
#include "bsp_api.h"

#define SDRAM_BASE_ADDR   (0x68000000UL)
#define SDRAM_SIZE_WORDS  (4UL * 1024UL * 1024UL)

void sdram_self_test_run(void)
{
    volatile uint32_t *base = (volatile uint32_t *)SDRAM_BASE_ADDR;

    printf("TEST START\r\n");
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    printf("WRITE BEGIN\r\n");
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);

    for (uint32_t i = 0; i < SDRAM_SIZE_WORDS; i++)
    {
        base[i] = (i ^ 0x5A5A5A5AUL);

        /* 每写完256K个word，喘一口气(1ms)，避免长时间连续无间断访问 */
        if ((i & 0x3FFFFUL) == 0 && i != 0)
        {
            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        }
    }

    printf("WRITE DONE\r\n");
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    printf("VERIFY BEGIN\r\n");
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);

    uint32_t fail = 0;
    uint32_t first_fail = 0xFFFFFFFFUL;

    for (uint32_t i = 0; i < SDRAM_SIZE_WORDS; i++)
    {
        uint32_t expect = (i ^ 0x5A5A5A5AUL);
        uint32_t got = base[i];
        if (got != expect)
        {
            if (fail == 0) first_fail = i;
            fail++;
        }

        if ((i & 0x3FFFFUL) == 0 && i != 0)
        {
            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        }
    }

    printf("VERIFY DONE\r\n");
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    printf("RESULT FAIL COUNT %lu\r\n", (unsigned long)fail);
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);

    if (fail == 0)
    {
        printf("RESULT PASS\r\n");
    }
    else
    {
        printf("RESULT FIRST FAIL INDEX %lu\r\n", (unsigned long)first_fail);
    }
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    printf("TEST END\r\n");
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
}
