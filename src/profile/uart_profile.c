/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (C) 2016-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom server 1 (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_utils.h"
#include "prf_types.h"
#include "attm_db_128.h"

#include "gpio.h"
#include "syscntl.h"
#include "uart.h"

#include "gapc_task.h" // gap functions and messages
#include "gapm_task.h" // gap functions and messages
#include "custs1_task.h"

#include "app_easy_timer.h"
#include "uart_profile.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Service 1 of the custom server 1
static const att_svc_desc128_t uart_top_svc = UART_PROFILE_UUID_128;

static const uint8_t UART_PROFILE_RX_UUID[ATT_UUID_128_LEN] = UART_PROFILE_RX_UUID_128;
static const uint8_t UART_PROFILE_TX_UUID[ATT_UUID_128_LEN] = UART_PROFILE_TX_UUID_128;

// Attribute specifications
static const uint16_t att_decl_svc = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const uint8_t custs1_services[] = {UART_PROFILE_IDX_SVC, UART_PROFILE_IDX_NB};
const uint8_t custs1_services_size = ARRAY_LEN(custs1_services) - 1;
const uint16_t custs1_att_max_nb = UART_PROFILE_IDX_NB;

/// Full CUSTS1 Database Description - Used to add attributes into the database
const struct attm_desc_128 custs1_att_db[UART_PROFILE_IDX_NB] =
    {
        /**
        * @brief UART top profile
        */
        [UART_PROFILE_IDX_SVC] = {(uint8_t *)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                  sizeof(uart_top_svc), sizeof(uart_top_svc), (uint8_t *)&uart_top_svc},

        /**
        * @brief UART TX attr 
        */
        [UART_PROFILE_TX_CHAR] = {(uint8_t *)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},

        [UART_PROFILE_TX_VAL] = {UART_PROFILE_TX_UUID, ATT_UUID_128_LEN, PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                 PERM(RI, ENABLE) | UART_PROFILE_TX_LEN, 0, NULL},

        [UART_PROFILE_TX_USER_DESC] = {(uint8_t *)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                       sizeof(DEF_UART_PROFILE_TX_USER_DESC) - 1, sizeof(DEF_UART_PROFILE_TX_USER_DESC) - 1,
                                       (uint8_t *)DEF_UART_PROFILE_TX_USER_DESC},

        /**
        * @brief UART RX attr 
        */

        [UART_PROFILE_RX_CHAR] = {(uint8_t *)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},

        [UART_PROFILE_RX_VAL] = {UART_PROFILE_RX_UUID, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(NTF, ENABLE),
                                 UART_PROFILE_RX_LEN, 0, NULL},

        [UART_PROFILE_RX_NTF_CFG] = {(uint8_t *)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                     sizeof(uint16_t), 0, NULL},

        [UART_PROFILE_RX_USER_DESC] = {(uint8_t *)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                       sizeof(DEF_UART_PROFILE_RX_USER_DESC) - 1, sizeof(DEF_UART_PROFILE_RX_USER_DESC) - 1,
                                       (uint8_t *)DEF_UART_PROFILE_RX_USER_DESC},
};

static uint8_t rxbuf[UART_PROFILE_RX_LEN] = {0};
static uint8_t* rxptr = rxbuf;
timer_hnd rx_timeout_tmr        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

static void uart_profile_rx_ntf_handler() 
{
    if (app_env[0].connection_active) 
    {
        struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ, prf_get_task_from_id(TASK_ID_CUSTS1), TASK_APP, custs1_val_ntf_ind_req, rxptr - rxbuf);

        req->conidx = app_env[0].conidx;
        req->notification = true;
        req->handle = UART_PROFILE_RX_VAL;
        req->length = rxptr - rxbuf;
        memcpy(req->value, rxbuf, req->length);
        ke_msg_send(req);
    }
   rxptr = rxbuf;
   rx_timeout_tmr = EASY_TIMER_INVALID_TIMER;
}

static void uart_rx_cb(uint16_t sz)
{
    if (rx_timeout_tmr != EASY_TIMER_INVALID_TIMER)
    {
       app_easy_timer_cancel(rx_timeout_tmr);
    }

    rx_timeout_tmr = app_easy_timer(UART_RX_TIMEOUT, uart_profile_rx_ntf_handler);
 
    if (rxptr >= &rxbuf[UART_PROFILE_RX_LEN])
    {
        rxptr = rxbuf;
    }
    else
    {
        rxptr ++;
    }

    uart_receive(UART1, rxptr, 1, UART_OP_INTR);
}


void uart_profile_tx_wr_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uart_send(UART1, param->value, param->length, UART_OP_BLOCKING);
}

void uart_profile_rx_val_handler(ke_msg_id_t const msgid, struct custs1_value_req_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP, prf_get_task_from_id(TASK_ID_CUSTS1), TASK_APP, custs1_value_req_rsp, UART_PROFILE_RX_LEN);
 
    rsp->conidx = app_env[param->conidx].conidx;
    rsp->att_idx = param->att_idx;
    rsp->length = rxptr - rxbuf;
    rsp->status = ATT_ERR_NO_ERROR;
    memcpy(rsp->value, rxbuf, rsp->length);

    ke_msg_send(rsp);
}

void def_att_info_req_handler(ke_msg_id_t const msgid, struct custs1_att_info_req const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t status)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP, src_id, dest_id, custs1_att_info_rsp);
    rsp->conidx = app_env[param->conidx].conidx;
    rsp->att_idx = param->att_idx;
    rsp->length = 0;
    rsp->status = status;
    ke_msg_send(rsp);
}


// Configuration struct for UART
static const uart_cfg_t uart_cfg = {
    // Set Baud Rate
    .baud_rate = UART_BAUDRATE_115200,
    // Set data bits
    .data_bits = UART_DATABITS_8,
    // Set parity
    .parity = UART_PARITY_NONE,
    // Set stop bits
    .stop_bits = UART_STOPBITS_1,
    // Set flow control
    .auto_flow_control = UART_AFCE_DIS,
    // Set FIFO enable
    .use_fifo = UART_FIFO_EN,
    // Set Tx FIFO trigger level
    .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0,
    // Set Rx FIFO trigger level
    .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0,
    // Set interrupt priority
    .intr_priority = 2,
    // uart rx callback
    .uart_rx_cb = uart_rx_cb,

#if defined (CFG_UART_DMA_SUPPORT)
    // Set UART DMA Channel Pair Configuration
    .uart_dma_channel = UART_DMA_CHANNEL_01,
    // Set UART DMA Priority
    .uart_dma_priority = DMA_PRIO_0,
#endif
};


void uart_hw_init()
{
    
#if DEVELOPMENT_DEBUG
    RESERVE_GPIO(UART_TX, UART_TX_PORT, UART_TX_PIN, PID_UART1_TX);
    RESERVE_GPIO(UART_RX, UART_RX_PORT, UART_RX_PIN, PID_UART1_RX);
#endif

    uart_initialize(UART1, &uart_cfg);
    GPIO_ConfigurePin(UART_TX_PORT, UART_TX_PIN, OUTPUT, PID_UART1_TX, false);
    GPIO_ConfigurePin(UART_RX_PORT, UART_RX_PIN, INPUT, PID_UART1_RX, false);
    GPIO_set_pad_latch_en(true);
    rxptr = rxbuf;
    uart_register_rx_cb(UART1, uart_rx_cb);
    uart_receive(UART1, rxptr, 1, UART_OP_INTR);
}


/// @} USER_CONFIG
