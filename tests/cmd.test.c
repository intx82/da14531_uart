#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "libs/cmd_hnd.h"
#include "libs/cmd_mux.h"

void hex(uint8_t *str, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        printf("%#x ", str[i]);
    }
    printf("\r\n");
}

int8_t on_int_rd(uint16_t var, uint16_t count, uint16_t out[9])
{
    printf("on_int_rd: var: %x, count: %x \r\n", var, count);
    assert(var == 0xaa55);
    assert(count == 2);

    out[0] = 0xabcd;
    out[1] = 0x1234;

    return count;
}

void __int_rd_test(void)
{
    uint8_t __int_rd_tst[] = {0x03, 0xaa, 0x55, 0x00, 0x02, 0x00};
    size_t __int_rd_sz = 5;
    cmd_mux(__int_rd_tst, &__int_rd_sz);

    printf("__int_rd_tst: [%d] ", (int)__int_rd_sz);
    hex(__int_rd_tst, __int_rd_sz);

    assert(__int_rd_sz == 6);
    assert(__int_rd_tst[0] == 0x03);
    assert(__int_rd_tst[1] == 0x04);
    assert(__int_rd_tst[2] == 0xcd);
    assert(__int_rd_tst[3] == 0xab);
    assert(__int_rd_tst[4] == 0x34);
    assert(__int_rd_tst[5] == 0x12);
}

int8_t on_int_wr(uint16_t var, uint16_t val)
{
    printf("on_int_wr: var: 0x%x val: 0x%x \r\n", var, val);
    assert(var == 0x55aa);
    assert(val == 0xaabb);
    return CMD_OK;
}

void __int_wr_test(void)
{
    uint8_t __int_wr_tst[] = {0x06, 0xaa, 0x55, 0xbb, 0xaa};
    size_t __int_wr_sz = sizeof(__int_wr_tst);
    cmd_mux(__int_wr_tst, &__int_wr_sz);

    printf("__int_wr_tst: [%d] 0x%x, 0x%x\r\n", (int)__int_wr_sz,  __int_wr_tst[0], __int_wr_tst[1]);
    assert(__int_wr_sz == 5);
    assert(__int_wr_tst[0] == 0x06);
    assert(__int_wr_tst[1] == 0xaa);
    assert(__int_wr_tst[2] == 0x55);
    assert(__int_wr_tst[3] == 0xbb);
    assert(__int_wr_tst[4] == 0xaa);
}

void on_blob_rd(uint32_t addr, uint8_t buf[16])
{
    printf("on_blob_rd: addr: %#x \r\n", addr);
    assert(addr == 0x55aa0);

    for (uint8_t i = 0; i < 16; i++)
    {
        buf[i] = 1 << (i & 0x7);
    }
}

void __blob_rd_test(void)
{
    uint8_t __blob_rd_tst[] = {0x12, 0xaa, 0x55, 0x0, 0, 0xaa, 0xbb, 0xcc, 0xdd, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    size_t __blob_rd_sz = 4;
    cmd_mux(__blob_rd_tst, &__blob_rd_sz);

    printf("__blob_rd_test: [%d] ", (int)__blob_rd_sz);
    hex(__blob_rd_tst, __blob_rd_sz);

    const uint8_t _dt[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

    assert(__blob_rd_sz == 20);
    assert(memcmp(_dt, &__blob_rd_tst[4], sizeof(_dt)) == 0);
}

int8_t on_blob_wr(uint32_t addr, uint8_t buf[16])
{
    printf("on_blob_wr: addr: %#x buf: \r\n", addr);
    hex(buf, 16);

    assert(addr == 0xcc55aa0);
    const uint8_t _dt[] = {0x11, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    assert(memcmp(_dt, buf, sizeof(_dt)) == 0);

    return 0;
}

void __blob_wr_test(void)
{
    uint8_t __blob_wr_tst[] = {0x13, 0xaa, 0x55, 0xcc, 0x11, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    size_t __blob_wr_sz = sizeof(__blob_wr_tst);
    cmd_mux(__blob_wr_tst, &__blob_wr_sz);

    printf("__blob_wr_test: [%d] ", (int)__blob_wr_sz);
    hex(__blob_wr_tst, __blob_wr_sz);

    assert(__blob_wr_sz == 2);
    assert(__blob_wr_tst[0] == 0x93);
    assert(__blob_wr_tst[1] == 5);
}

int main(void)
{
    printf("starting... \r\n");
    __int_rd_test();
    __int_wr_test();
    __blob_rd_test();
    __blob_wr_test();

    return 0;
}