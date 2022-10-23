#include <systick.h>
#include <user_periph_setup.h>
#include "test_hnd.h"
#include "cmd_hnd.h"
#include "version.h"

#include "user_config_storage.h"

#define VARS_CNT 32

static uint16_t vars[VARS_CNT] = {0};
static uint64_t systime = 0;
static uint32_t _pre_minute_val = 0;
static uint32_t _pre_hour_val = 0;
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
    
    if ((systime % (60 * 1000)) == 0) {
        uint32_t* _total_cnt = (uint32_t*)&vars[2];
        uint32_t* _per_minute = (uint32_t*)&vars[4];
        *_per_minute  = *_total_cnt - _pre_minute_val;
        _pre_minute_val = *_total_cnt;
    }
    
    if ((systime % (60 * 60 * 1000)) == 0) {
        uint32_t* _total_cnt = (uint32_t*)&vars[2];
        uint32_t* _per_hour = (uint32_t*)&vars[6];
        *_per_hour  = *_total_cnt - _pre_hour_val;
        _pre_hour_val = *_total_cnt;
    }
}

static uint8_t verify_value_cb (uint8_t* param)
{
    return 0;
}

void on_sensor_light_on_cb(void)
{
    uint32_t* _total_cnt = (uint32_t*)&vars[2];
    *_total_cnt += 1;
}

void test_hnd_init()
{
    //systick
    systick_register_callback(systick_irq);
    systick_start(1000, true);
    RESERVE_GPIO(LIGHT_SENS, (GPIO_PORT) LIGHT_SENSOR_GPIO_PORT, (GPIO_PIN) LIGHT_SENSOR_GPIO_PIN, PID_GPIO);

    GPIO_ConfigurePin((GPIO_PORT) LIGHT_SENSOR_GPIO_PORT, (GPIO_PIN) LIGHT_SENSOR_GPIO_PIN, INPUT_PULLUP, PID_GPIO, false ); // active low. Set default to input high.
    
    GPIO_RegisterCallback(GPIO0_IRQn, on_sensor_light_on_cb);
	GPIO_EnableIRQ( LIGHT_SENSOR_GPIO_PORT, LIGHT_SENSOR_GPIO_PIN, GPIO0_IRQn, 1, 1, 10 );
    
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
}

int8_t on_blob_wr(uint32_t addr, uint8_t buf[16])
{
    return -1;
}

