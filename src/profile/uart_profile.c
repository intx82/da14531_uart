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

#include "gapc_task.h" // gap functions and messages
#include "gapm_task.h" // gap functions and messages
#include "custs1_task.h"

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

void uart_profile_tx_wr_ind_handler(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    (void)msgid;
    (void)param;
    (void)dest_id;
    (void)src_id;
}

void uart_profile_rx_val_handler(ke_msg_id_t const msgid, struct custs1_value_req_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
/*
    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC(CUSTS1_VALUE_REQ_RSP, prf_get_task_from_id(TASK_ID_CUSTS1), TASK_APP, custs1_value_req_rsp, UART_PROFILE_RX_LEN);
    const char ok_msg[] = "OK";
    rsp->conidx = app_env[param->conidx].conidx;
    rsp->att_idx = param->att_idx;
    rsp->length = sizeof(ok_msg);
    rsp->status = ATT_ERR_APP_ERROR;
    memcpy(rsp->value, ok_msg, rsp->length);

    ke_msg_send(rsp);
    */
    def_att_info_req_handler(msgid, (struct custs1_att_info_req const *)param, dest_id, src_id, ATT_ERR_APP_ERROR);
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

/// @} USER_CONFIG
