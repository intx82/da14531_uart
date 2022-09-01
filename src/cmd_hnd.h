#ifndef __CMD_HND_H
#define __CMD_HND_H

#include <stdint.h>
#include <stddef.h>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#define CMD_OK 0
#define CMD_FUNC_NOT_FOUND -1
#define CMD_ADDR_NOT_FOUND -2
#define CMD_VAL_INCORRECT -3
#define CMD_ACK -5

/**
 * @brief Called on read integer
 *
 * @param var Number of variable
 * @return uint32_t Returned value
 */
int8_t on_int_rd(uint16_t var, uint16_t count, uint16_t out[9]) __weak;

/**
 * @brief Called on write integer
 *
 * @param var Number variable
 * @param val Value of variable
 * @return 0 if ok
 */
int8_t on_int_wr(uint16_t var, uint16_t val) __weak;

/**
 * @brief Called on blob read
 * @warning output data size must not exceed 16 bytes (buf overflow)
 * @param addr Address
 * @param buf Output buffer
 */
void on_blob_rd(uint32_t addr, uint8_t buf[16]) __weak;

/**
 * @brief Called on blob write
 * @warning intput data size is 16 bytes, please don't read more
 * @param addr Address
 * @param buf Output buffer
 */
int8_t on_blob_wr(uint32_t addr, uint8_t buf[16]) __weak;

#endif

