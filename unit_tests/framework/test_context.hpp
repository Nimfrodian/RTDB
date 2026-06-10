#pragma once
#include "rtdb.h"

using namespace rtdb;

void rtdb_error_handler(
    rtdb_error_t err,
    const char* context)
{
}

uint32_t test_crc32(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    uint32_t c = crc;
    for (uint32_t i = 0; i < len; i++) {
        c ^= buf[i];
        for (int j = 0; j < 8; j++) {
            if (c & 1) {
                c = 0xEDB88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
    }
    return c ^ 0xFFFFFFFFL;
}

void standard_init()
{
    rtdb_init_t cfg{};
    cfg.crc_handler = test_crc32;
    cfg.error_handler = rtdb_error_handler;
    init(cfg);
}