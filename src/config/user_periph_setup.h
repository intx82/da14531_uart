/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "spi_flash.h"
#include "i2c.h"
#include "i2c_eeprom.h"



/*
 * DEFINES
 ****************************************************************************************
 */

#ifndef PRODUCT_HEADER_POSITION
#if defined (__DA14531__)
#define PRODUCT_HEADER_POSITION     0x1F000
#else
#define PRODUCT_HEADER_POSITION     0x38000
#endif
#endif

#define CFG1_DEFAULT_POSITION 0x39000
#define CFG2_DEFAULT_POSITION 0x3A000

/****************************************************************************************/
/* UART2 configuration                                                                  */
/****************************************************************************************/
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_6
#else
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#endif

// Define UART2 Settings
#define UART2_BAUDRATE              UART_BAUDRATE_115200
#define UART2_DATABITS              UART_DATABITS_8
#define UART2_PARITY                UART_PARITY_NONE
#define UART2_STOPBITS              UART_STOPBITS_1
#define UART2_AFCE                  UART_AFCE_DIS
#define UART2_FIFO                  UART_FIFO_EN
#define UART2_TX_FIFO_LEVEL         UART_TX_FIFO_LEVEL_0
#define UART2_RX_FIFO_LEVEL         UART_RX_FIFO_LEVEL_0


/****************************************************************************************/
/* LED configuration                                                                    */
/****************************************************************************************/
#if defined (__DA14531__)
    #define GPIO_LED_PORT           GPIO_PORT_0
    #define GPIO_LED_PIN            GPIO_PIN_9
#else
    #define GPIO_LED_PORT           GPIO_PORT_1
    #define GPIO_LED_PIN            GPIO_PIN_0
#endif

/****************************************************************************************/
/* SPI configuration                                                                    */
/****************************************************************************************/
// Define SPI Pads
#if defined (__DA14531__)
    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_1

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_4

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_0

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_3

#elif !defined (__DA14586__)
    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_3

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_0

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_5

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_6
#endif

// Define SPI Configuration
#define SPI_MS_MODE             SPI_MS_MODE_MASTER
#define SPI_CP_MODE             SPI_CP_MODE_0
#define SPI_WSZ                 SPI_MODE_8BIT
#define SPI_CS                  SPI_CS_0

#if defined(__DA14531__)
    #define SPI_SPEED_MODE          SPI_SPEED_MODE_4MHz
    #define SPI_EDGE_CAPTURE        SPI_MASTER_EDGE_CAPTURE
#else // (DA14585, DA14586)
    #define SPI_SPEED_MODE          SPI_SPEED_MODE_2MHz
#endif

#define SPI_FLASH_DEV_SIZE SPI_FLASH_MBIT_SIZE(8)
#define SPI_FLASH_MBIT_SIZE(MBIT) (MBIT * 131072)

/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
#if defined (__DA14531__)
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_0
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_11
#else
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_2
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_5
#endif
#endif



#define SPI_FLASH_GPIO_MAP   ((uint32_t)SPI_CLK_PIN | ((uint32_t)SPI_CLK_PORT << 4) | \
                        ((uint32_t)SPI_EN_PIN << 8) | ((uint32_t)SPI_EN_PORT << 12) | \
                        ((uint32_t)SPI_DO_PIN << 16) | ((uint32_t)SPI_DO_PORT << 20) | \
                        ((uint32_t)SPI_DI_PIN << 24) | ((uint32_t)SPI_DI_PORT << 28))
                        
#define CFG_CONFIG_STORAGE 1

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG
/**
 ****************************************************************************************
 * @brief   Reserves application's specific GPIOs
 * @details Used only in Development mode (#if DEVELOPMENT_DEBUG)
 *          i.e. to reserve P0_1 as Generic Purpose I/O:
 *          RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
 ****************************************************************************************
 */
void GPIO_reservations(void);
#endif

/**
 ****************************************************************************************
 * @brief   Sets the functionality of application pads
 * @details i.e. to set P0_1 as Generic purpose Output:
 *          GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
 ****************************************************************************************
 */
void set_pad_functions(void);

/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins
 ****************************************************************************************
 */
void periph_init(void);


#if (!SUOTAR_SPI_DISABLE) || defined CFG_CONFIG_STORAGE
/**
 ****************************************************************************************
 * @brief Initializes the SPI to access the flash.
 * @param[in] gpio_map  Bitmap of the GPIOs to be used
 * @return void
 ****************************************************************************************
 */
void user_spi_flash_init(uint32_t gpio_map);

/**
 ****************************************************************************************
 * @brief Powers down the flash and releases the SPI controller.
 * @return void
 ****************************************************************************************
 */
void user_spi_flash_release(void);

/**
 ****************************************************************************************
 * @brief Erases sectors of the flash. This functions can erase whole sectors, not part of it.
 * @param[in] starting_address  The address in the flash where we want to erase.
 * @param[in] size              Size of data we want to erase.
 * @return 0 if success, else error code.
 ****************************************************************************************
 */
int8_t user_erase_flash_sectors(uint32_t starting_address, uint32_t size);

/**
 ****************************************************************************************
 * @brief Writes data to flash.
 * @param[in] data      Pointer to the data to be written.
 * @param[in] address   Address in the flash.
 * @param[in] size      Size of data we want to write.
 * @return 0 if success, else error code.
 ****************************************************************************************
 */
int32_t user_flash_write_data (uint8_t *data, uint32_t address, uint32_t size);
#endif

#endif // _USER_PERIPH_SETUP_H_
