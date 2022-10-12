/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "syscntl.h"
#include "test_hnd.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif

    RESERVE_GPIO(LED, GPIO_LED_PORT, GPIO_LED_PIN, PID_GPIO);

#if !defined (__DA14586__)
    RESERVE_GPIO(SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
#endif
}

#endif

void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#else
    // Disallow spontaneous SPI Flash wake-up
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

    GPIO_ConfigurePin(GPIO_LED_PORT, GPIO_LED_PIN, OUTPUT, PID_GPIO, false);
}

#if defined (CFG_PRINTF_UART2)
// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};
#endif

void periph_init(void)
{
#if defined (__DA14531__)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
    
    test_hnd_init();
}


// Configuration struct for SPI
static const spi_cfg_t spi_cfg = {
    .spi_ms = SPI_MS_MODE,
    .spi_cp = SPI_CP_MODE,
    .spi_speed = SPI_SPEED_MODE,
    .spi_wsz = SPI_WSZ,
    .spi_cs = SPI_CS,
    .cs_pad.port = SPI_EN_PORT,
    .cs_pad.pin = SPI_EN_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_EDGE_CAPTURE,
#endif
};

static const spi_flash_cfg_t spi_flash_cfg = {
    .chip_size = SPI_FLASH_DEV_SIZE,
};

void user_spi_flash_init(uint32_t gpio_map)
{
    uint8_t dev_id = 0;
#if DEVELOPMENT_DEBUG
    RESERVE_GPIO( SPI_CLK, SPI_CLK_PORT, SPI_CLK_PIN, PID_SPI_CLK);
    RESERVE_GPIO( SPI_DO, SPI_DO_PORT, SPI_DO_PIN, PID_SPI_DO);
    RESERVE_GPIO( SPI_DI, SPI_DI_PORT, SPI_DI_PIN, PID_SPI_DI);
    RESERVE_GPIO( SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
#endif

    GPIO_ConfigurePin( SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true );
    GPIO_ConfigurePin( SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false );
    GPIO_ConfigurePin( SPI_DO_PORT, SPI_DO_PIN, OUTPUT, PID_SPI_DO, false );
    GPIO_ConfigurePin( SPI_DI_PORT, SPI_DI_PIN, INPUT, PID_SPI_DI, false );

    // Configure SPI Flash environment
    spi_flash_configure_env(&spi_flash_cfg);

    // Initialize SPI
    spi_initialize(&spi_cfg);


    // Release Flash from power down
    spi_flash_release_from_power_down();

    // Try to auto-detect the device
    spi_flash_auto_detect(&dev_id);

}

void user_spi_flash_release(void)
{
    spi_flash_power_down();
    spi_release();
}

int8_t user_erase_flash_sectors(uint32_t starting_address, uint32_t size)
{
    int i;
    uint32_t starting_sector;
    uint32_t sector;
    int8_t ret = -1;

    starting_sector = (starting_address/SPI_FLASH_SECTOR_SIZE) * SPI_FLASH_SECTOR_SIZE;

    sector=(size / SPI_FLASH_SECTOR_SIZE);
    if (size % SPI_FLASH_SECTOR_SIZE)
    {
        sector++;
    }
    if (!sector)
    {
        sector++;
    }
    for (i=0; i < sector; i++)
    {
        ret = spi_flash_block_erase(starting_sector, SPI_FLASH_OP_SE);
        starting_sector += SPI_FLASH_SECTOR_SIZE;
        if (ret != SPI_FLASH_ERR_OK)
        {
            break;
        }
    }
    return ret;
}

int32_t user_flash_write_data (uint8_t *data, uint32_t address, uint32_t size)
{
    uint32_t actual_size;

    uint32_t upper_limit = address + size;
    uint32_t starting_sector = (address/SPI_FLASH_SECTOR_SIZE) * SPI_FLASH_SECTOR_SIZE;
    uint32_t next_sector_start;

    if (starting_sector == address)
    {
        // We start at a place where new sector starts - erase from here.
        user_erase_flash_sectors(address, size);
    } else if ((upper_limit/SPI_FLASH_SECTOR_SIZE) != (address/SPI_FLASH_SECTOR_SIZE)) {
        // We start in the middle of already erased sector - start erase from next one.
        next_sector_start = starting_sector + SPI_FLASH_SECTOR_SIZE;

        user_erase_flash_sectors(next_sector_start, upper_limit - next_sector_start);
    }

    return spi_flash_write_data(data, address, size, &actual_size);
}
