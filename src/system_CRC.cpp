#include "stdint.h"

uint32_t esp_rom_crc32_le(uint32_t crc, uint8_t const *buf, uint32_t len)
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