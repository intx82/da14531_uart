#include <systick.h>
#include "test_hnd.h"
#include "cmd_hnd.h"
#include "version.h"
#include "user_config_storage.h"

#define VARS_CNT 32

static uint16_t vars[VARS_CNT] = {0};
static uint8_t blob[256] = {0};
static uint64_t systime = 0;

uint8_t verify_value_cb (uint8_t* param);

static const user_config_elem_t conf_table[] = {
    {0x1000, sizeof(uint16_t) * VARS_CNT, sizeof(uint16_t) * VARS_CNT, vars, verify_value_cb, false},
};

uint64_t get_systime() 
{
    return systime;
}

static void systick_irq()
{
    systime += 1;
}

static uint8_t verify_value_cb (uint8_t* param)
{
    return 0;
}

void test_hnd_init()
{
    //systick
    systick_register_callback(systick_irq);
    systick_start(1000, true);
    //user_conf
    uint8_t version_len = strlen(SDK_VERSION);
    char version[16] = {0};
    strncpy(version, SDK_VERSION, sizeof(version));

    user_conf_storage_init((user_config_elem_t *)conf_table, sizeof(conf_table)/sizeof(user_config_elem_t), version, &version_len);
    vars[0] += 1;
    user_config_save((user_config_elem_t *)conf_table, sizeof(conf_table)/sizeof(user_config_elem_t), version, &version_len);
}


int8_t on_int_rd(uint16_t var, uint16_t count, uint16_t out[9]) 
{
    int8_t idx = 0;
    for(; idx < count; idx++)
    {
        if (idx >= 9 || (var + idx) >= VARS_CNT)
        {
            break;
        }   
        out[idx] = vars[var+idx];
    }
    
    return idx > 0 ? idx : CMD_ADDR_NOT_FOUND;
}


int8_t on_int_wr(uint16_t var, uint16_t val)
{
    if (var >= VARS_CNT)
    {
        return CMD_ADDR_NOT_FOUND;
    }
    
    vars[var] = val;
    return CMD_OK;
}


void on_blob_rd(uint32_t addr, uint8_t buf[16])
{
    if(addr < sizeof(blob))
    {
        memcpy(buf, &blob[addr], 16);
    }
}

int8_t on_blob_wr(uint32_t addr, uint8_t buf[16])
{
    if(addr < sizeof(blob))
    {
        memcpy(&blob[addr], buf, 16);
    }
    
    return 0;
}

