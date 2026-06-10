#pragma once
#include "rtdb_types.h"
#include <cstdint>

namespace rtdb
{

static rtdb_error_t freeData(rtdb_type_t type, void* data) noexcept
{
    rtdb_error_t err = RTDB_ERR_UNDEFINED;
    if (data != nullptr)
    {
        switch (type)
        {
            case RTDB_TYPE_BOOL:
                delete[] static_cast<bool*>(data);
                break;
            case RTDB_TYPE_INT8:
                delete[] static_cast<int8_t*>(data);
                break;
            case RTDB_TYPE_UINT8:
                delete[] static_cast<uint8_t*>(data);
                break;
            case RTDB_TYPE_INT16:
                delete[] static_cast<int16_t*>(data);
                break;
            case RTDB_TYPE_UINT16:
                delete[] static_cast<uint16_t*>(data);
                break;
            case RTDB_TYPE_INT32:
                delete[] static_cast<int32_t*>(data);
                break;
            case RTDB_TYPE_UINT32:
                delete[] static_cast<uint32_t*>(data);
                break;
            case RTDB_TYPE_INT64:
                delete[] static_cast<int64_t*>(data);
                break;
            case RTDB_TYPE_UINT64:
                delete[] static_cast<uint64_t*>(data);
                break;
            case RTDB_TYPE_FLOAT:
                delete[] static_cast<float*>(data);
                break;
            case RTDB_TYPE_DOUBLE:
                delete[] static_cast<double*>(data);
                break;
            // No default - CRC and compiler guarantee it nevers gets to here
        }
    }
    return err;
}
uint32_t softwareCRC(uint32_t crc, uint8_t const *buf, uint32_t len) noexcept;
}