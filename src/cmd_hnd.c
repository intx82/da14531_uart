#include <string.h>
#include "cmd_hnd.h"
#include "cmd_mux.h"


#define CHECK_SZ(S, T) do { \
    if (S != sizeof(T)) return 0; \
} while(0)

/**
 * @brief device commands
 */
typedef enum
{
    CMD_INT_RD = 0x10, /**< Integer read */
    CMD_INT_WR,        /**< Integer write */
    CMD_BLOB_RD,       /**< BLOB (binary large object) read */
    CMD_BLOB_WR        /**< BLOB (binary large object) write */
} cmds_t;

typedef struct
{
    int8_t err;
} __err_rsp_t;

typedef struct __packed
{
    uint32_t nb;
} __int_rd_cmd_t;

typedef struct __packed
{
    uint32_t nb;
} __int_rd_rsp_t;

size_t __on_int_rd(uint8_t *data, size_t sz)
{

    CHECK_SZ(sz, __int_rd_cmd_t);
    ((__int_rd_rsp_t *)data)->nb = on_int_rd(((__int_rd_cmd_t *)data)->nb);
    return sizeof(__int_rd_rsp_t);
}

__weak uint32_t on_int_rd(uint32_t var)  {
    (void)var;
    return 0;
}

typedef struct __packed
{
    uint32_t nb;
    uint32_t val;
} __int_wr_cmd_t;

size_t __on_int_wr(uint8_t *data, size_t sz)
{
    CHECK_SZ(sz, __int_wr_cmd_t);

    ((__err_rsp_t *)data)->err = on_int_wr(((__int_wr_cmd_t *)data)->nb, ((__int_wr_cmd_t *)data)->val);
    return sizeof(__err_rsp_t);
}

__weak int8_t on_int_wr(uint32_t var, uint32_t val)  {
    (void)var;
    (void)val;
    return CMD_ERR_NOT_FOUND;
}

typedef struct __packed
{
    uint8_t addr[3];
} __blob_rd_cmd_t;

typedef struct
{
    uint8_t addr[3];
    uint8_t data[16];
} __blob_rd_rsp_t;

size_t __on_blob_rd(uint8_t *data, size_t sz)
{
    CHECK_SZ(sz, __blob_rd_cmd_t);

    uint32_t addr = ((__blob_rd_cmd_t *)data)->addr[0] | (((__blob_rd_cmd_t *)data)->addr[1] << 8) | (((__blob_rd_cmd_t *)data)->addr[2] << 16);
    addr <<= 4;
    on_blob_rd(addr, ((__blob_rd_rsp_t*)data)->data);
    return sizeof(__blob_rd_rsp_t);
}

__weak void on_blob_rd(uint32_t addr, uint8_t buf[16])  {
    (void)addr;
    memset(buf, 0, 16);
}

typedef struct __packed
{
    uint8_t addr[3];
    uint8_t data[16];
} __blob_wr_cmd_t;

size_t __on_blob_wr(uint8_t *data, size_t sz)
{
    CHECK_SZ(sz, __blob_wr_cmd_t);

    uint32_t addr = ((__blob_wr_cmd_t *)data)->addr[0] | (((__blob_wr_cmd_t *)data)->addr[1] << 8) | (((__blob_wr_cmd_t *)data)->addr[2] << 16);
    addr <<= 4;
    ((__err_rsp_t *)data)->err = on_blob_wr(addr, ((__blob_wr_cmd_t *)data)->data);
    return sizeof(__err_rsp_t);
}

__weak int8_t on_blob_wr(uint32_t addr, uint8_t buf[16]) {
    (void)addr;
    (void)buf;
    return CMD_ERR_NOT_FOUND;
}

CMD_LIST_START
CMD_LIST_REG_HND(CMD_INT_RD, __on_int_rd)
CMD_LIST_REG_HND(CMD_INT_WR, __on_int_wr)
CMD_LIST_REG_HND(CMD_BLOB_RD, __on_blob_rd)
CMD_LIST_REG_HND(CMD_BLOB_WR, __on_blob_wr)
CMD_LIST_END
