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

#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "custs1_task.h"


/** @brief Service1 Nordic UART impl:  6E400001-B5A3-F393-­E0A9-­E50E24DCCA9E */
#define UART_PROFILE_UUID_128                                                                          \
    {                                                                                                  \
        0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e \
    }

/** @brief  UART Nordic RX UUID 6E400002-B5A3-F393-E0A9-E50E24DCCA9E */
#define UART_PROFILE_TX_UUID_128                                                                       \
    {                                                                                                  \
        0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e \
    }


/** @brief  UART Nordic RX UUID 6E400003-B5A3-F393-E0A9-E50E24DCCA9E */
#define UART_PROFILE_RX_UUID_128                                                                       \
    {                                                                                                  \
        0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e \
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


#endif // _UART_PROFILE_H_
