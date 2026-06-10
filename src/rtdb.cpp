#include "rtdb.h"
#include <cstddef>

namespace rtdb {
    bool initialized = false;
    rtdb_var_t rtdb_vars[RTDB_SIZE] = {};

    static uint32_t (*rtdb_crc_fn)(uint32_t, uint8_t const*, uint32_t) = software_CRC;

    // Error handler callback; can be nullptr
    rtdb_error_handler_t g_error_handler = nullptr;

    void init(const rtdb_init_t& cfg)
    {
        if (cfg.crc_fn != nullptr)
        {
            rtdb_crc_fn = cfg.crc_fn;
        }
        if (cfg.error_handler != nullptr)
        {
            g_error_handler = cfg.error_handler;
        }
        initialized = true;
    }

    uint32_t software_CRC(uint32_t crc, uint8_t const *buf, uint32_t len)
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

    uint32_t calculateCRC(const rtdb_var_t* var)
    {
        const uint8_t* metadata = reinterpret_cast<const uint8_t*>(var);
        uint32_t metadataLength = static_cast<uint32_t>(offsetof(rtdb_var_t, crc));
        uint32_t crc = rtdb_crc_fn(0, metadata, metadataLength);
        crc = rtdb_crc_fn(crc, static_cast<const uint8_t*>(var->data), var->size * var->elementSize);
        return crc;
    }

    rtdb_error_t setVarControl(rtdb_id_t id, rtdb_control_t controlType)
    {
        rtdb_error_t err = RTDB_ERR_UNDEFINED;
        if (!initialized) {
            err = RTDB_ERR_MODULE_UNINITIALIZED;
            report_error(err, "rtdb::setVarControl: RTDB not initialized");
        }
        else if (id >= RTDB_SIZE)    {
            err = RTDB_ERR_INVALID_ID;
            report_error(err, "rtdb::setVarControl: Invalid ID");
        }
        else if (rtdb_vars[id].data == nullptr)
        {
            err = RTDB_ERR_VAR_NOT_CONFIGURED;
            report_error(err, "rtdb::setVarControl: Variable not configured");
        }
        else if (calculateCRC(&rtdb_vars[id]) != rtdb_vars[id].crc)
        {
            err = RTDB_ERR_CRC_MISMATCH;
            report_error(err, "rtdb::setVarControl: CRC mismatch");
        }
        else
        {
            rtdb_var_t& var = rtdb_vars[id];
            var.controlType = controlType;
            var.crc = calculateCRC(&var);
            err = RTDB_OK;
        }
        return err;
    }
}
