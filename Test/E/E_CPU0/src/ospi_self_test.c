/*
 * ospi_self_test.c
 * RA8P1 + W25Q256JVEIQ OSPI Flash self-test module
 * Combines: JEDEC ID check + erase/write/verify on a safe test sector
 * Use this to quickly confirm OSPI Flash is working after hardware
 * or configuration changes.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "hal_data.h"
#include "bsp_api.h"

#define TEST_OFFSET      (0x10000UL)      /* 64KB offset into flash, safe test area */
#define TEST_SECTOR_SIZE (4096UL)         /* matches Sector Erase config */
#define TEST_WRITE_SIZE  (256UL)          /* one page */
#define WRITE_CHUNK_SIZE (16UL)           /* write in small chunks, avoids truncation */
#define OSPI_MAP_BASE    (0x80000000UL)   /* OSPI0_CS0 memory-mapped base */

static fsp_err_t ospi_wait_ready(void)
{
    /* Manually poll Read Status Register 1 (0x05), check bit0 (WIP) */
    fsp_err_t err;
    uint32_t timeout_ms = 5000;

    while (timeout_ms > 0)
    {
        spi_flash_direct_transfer_t transfer = {0};
        transfer.command        = 0x05;
        transfer.command_length = 1;
        transfer.data_length    = 1;

        err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &transfer,
                                             SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (err != FSP_SUCCESS)
        {
            return err;
        }

        uint8_t status_reg = (uint8_t)(transfer.data & 0xFF);
        if ((status_reg & 0x01) == 0)
        {
            return FSP_SUCCESS;
        }

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms -= 1;
    }
    return FSP_ERR_TIMEOUT;
}

static int ospi_check_jedec_id(void)
{
    fsp_err_t err;
    spi_flash_direct_transfer_t transfer = {0};
    transfer.command        = 0x9F;
    transfer.command_length = 1;
    transfer.data_length    = 3;

    err = g_ospi0.p_api->directTransfer(g_ospi0.p_ctrl, &transfer,
                                         SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (err != FSP_SUCCESS)
    {
        printf("JEDEC ID TRANSFER FAILED CODE %d\r\n", (int)err);
        return 0;
    }

    uint8_t id0 = (uint8_t)((transfer.data)       & 0xFF);
    uint8_t id1 = (uint8_t)((transfer.data >> 8)  & 0xFF);
    uint8_t id2 = (uint8_t)((transfer.data >> 16) & 0xFF);

    printf("JEDEC ID %02X %02X %02X\r\n", id0, id1, id2);
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);

    return (id0 == 0xEF) ? 1 : 0;
}

static int ospi_check_erase_write(void)
{
    fsp_err_t err;

    /* Erase */
    err = g_ospi0.p_api->erase(g_ospi0.p_ctrl, (uint8_t *)(OSPI_MAP_BASE + TEST_OFFSET),
                                TEST_SECTOR_SIZE);
    if (err != FSP_SUCCESS)
    {
        printf("ERASE CALL FAILED CODE %d\r\n", (int)err);
        return 0;
    }
    err = ospi_wait_ready();
    if (err != FSP_SUCCESS)
    {
        printf("ERASE WAIT FAILED CODE %d\r\n", (int)err);
        return 0;
    }

    /* Verify blank */
    volatile uint8_t *read_ptr = (volatile uint8_t *)(OSPI_MAP_BASE + TEST_OFFSET);
    for (uint32_t i = 0; i < TEST_WRITE_SIZE; i++)
    {
        if (read_ptr[i] != 0xFF)
        {
            printf("BLANK CHECK FAILED AT %lu\r\n", (unsigned long)i);
            return 0;
        }
    }

    /* Write in small chunks */
    uint8_t write_buf[TEST_WRITE_SIZE];
    for (uint32_t i = 0; i < TEST_WRITE_SIZE; i++)
    {
        write_buf[i] = (uint8_t)(i ^ 0x5A);
    }

    uint32_t written = 0;
    while (written < TEST_WRITE_SIZE)
    {
        uint32_t this_chunk = WRITE_CHUNK_SIZE;
        if (written + this_chunk > TEST_WRITE_SIZE)
        {
            this_chunk = TEST_WRITE_SIZE - written;
        }

        err = g_ospi0.p_api->write(g_ospi0.p_ctrl, &write_buf[written],
                                    (uint8_t *)(OSPI_MAP_BASE + TEST_OFFSET + written),
                                    this_chunk);
        if (err != FSP_SUCCESS)
        {
            printf("WRITE FAILED AT %lu CODE %d\r\n", (unsigned long)written, (int)err);
            return 0;
        }

        err = ospi_wait_ready();
        if (err != FSP_SUCCESS)
        {
            printf("WRITE WAIT FAILED AT %lu CODE %d\r\n", (unsigned long)written, (int)err);
            return 0;
        }

        written += this_chunk;
    }

    /* Verify */
    int fail = 0;
    for (uint32_t i = 0; i < TEST_WRITE_SIZE; i++)
    {
        if (read_ptr[i] != (uint8_t)(i ^ 0x5A))
        {
            fail++;
        }
    }

    printf("ERASE WRITE VERIFY FAIL COUNT %d\r\n", fail);
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);

    return (fail == 0) ? 1 : 0;
}

void ospi_self_test_run(void)
{
    fsp_err_t err;

    printf("OSPI SELF TEST START\r\n");
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    err = g_ospi0.p_api->open(g_ospi0.p_ctrl, g_ospi0.p_cfg);
    if (err != FSP_SUCCESS)
    {
        printf("OPEN FAILED CODE %d\r\n", (int)err);
        return;
    }
    printf("OPEN OK\r\n");
    R_BSP_SoftwareDelay(30, BSP_DELAY_UNITS_MILLISECONDS);

    int id_ok = ospi_check_jedec_id();
    int ew_ok = ospi_check_erase_write();

    printf("JEDEC ID CHECK %s\r\n", id_ok ? "PASS" : "FAIL");
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
    printf("ERASE WRITE CHECK %s\r\n", ew_ok ? "PASS" : "FAIL");
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);

    if (id_ok && ew_ok)
    {
        printf("OSPI FLASH READY\r\n");
    }
    else
    {
        printf("OSPI FLASH HAS ISSUES\r\n");
    }

    printf("OSPI SELF TEST END\r\n");
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
}
