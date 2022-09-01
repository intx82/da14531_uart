/**
 ****************************************************************************************
 *
 * @file UART_PROFILE.h
 *
 * @brief Nordic UART service
 * @see https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.4.0/nrf/include/bluetooth/services/nus.html#id3
 * @see https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend/uart-service
 *
 ****************************************************************************************
 */

#ifndef _UART_PROFILE_H_
#define _UART_PROFILE_H_

#include "attm_db_128.h"

#include "gapc_task.h" // gap functions and messages
#include "gapm_task.h" // gap functions and messages
#include "custs1_task.h"

#define UART_TX_PORT GPIO_PORT_0
#define UART_RX_PORT GPIO_PORT_0
#define UART_TX_PIN GPIO_PIN_4
#define UART_RX_PIN GPIO_PIN_5
#define UART_RX_TIMEOUT 100 // * 10ms

/** @brief Service1 Dialog Semi DSPS UART impl: 07b3b03e-8535-b5a07140a304d2495cb7 */
// nordic 0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e
#define UART_PROFILE_UUID_128                                                                          \
    {                                                                                                  \
        0xb7, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07 \
    }

/** @brief  UART Dialog Semi DSPS */
#define UART_PROFILE_TX_UUID_128                                                                       \
    {                                                                                                  \
        0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07 \
    }

/** @brief  UART Dialog Semi DSPS */
#define UART_PROFILE_RX_UUID_128                                                                       \
    {                                                                                                  \
        0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07 \
    }

/** @brief  UART Dialog Semi DSPS */
#define UART_PROFILE_FLOW_UUID_128                                                                     \
    {                                                                                                  \
        0xb9, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07 \
    }

#define UART_PROFILE_RX_LEN 20
#define UART_PROFILE_TX_LEN 20

#define DEF_UART_PROFILE_RX_USER_DESC "RX"
#define DEF_UART_PROFILE_TX_USER_DESC "TX"

/// UART Service Data Base Characteristic enum
enum
{
    UART_PROFILE_IDX_SVC = 0,

    UART_PROFILE_TX_CHAR,
    UART_PROFILE_TX_VAL,
    UART_PROFILE_TX_USER_DESC,

    UART_PROFILE_RX_CHAR,
    UART_PROFILE_RX_VAL,
    UART_PROFILE_RX_NTF_CFG,
    UART_PROFILE_RX_USER_DESC,

    UART_PROFILE_IDX_NB
};

/**
 ****************************************************************************************
 * @brief Default value attribute info request handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
 */
void def_att_info_req_handler(ke_msg_id_t const msgid, struct custs1_att_info_req const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t status);

/**
 ****************************************************************************************
 * @brief Handles the messages for transmitting via UART
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
 */
void uart_profile_tx_wr_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Read the value from UART
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
 */
void uart_profile_rx_val_handler(ke_msg_id_t const msgid, struct custs1_value_req_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Initialize hardware UART (uart1)
 * @remark Use P0.4/P0.5 as TX/RX pins; speed 115200 8n1
 ****************************************************************************************
 */
void uart_hw_init(void);

#endif // _UART_PROFILE_H_
