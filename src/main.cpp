#include "stdint.h"
#include "rtdb.h"
#include "someModule.h"
#include "system_CRC.h"


int main()
{
    rtdb::rtdb_init_t initCfg = {
        .crc_handler = esp_rom_crc32_le
    };
    rtdb::init(initCfg);
    someModule_init();

    for (int  i = 0; i < 10; i++)
    someModule_run();

    return 0;
}