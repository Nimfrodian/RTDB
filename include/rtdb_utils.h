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

template<typename T>
bool checkIfOutOfBounds(T value, rtdb_var_t metadata)
{
    bool isInBounds = false;
    if constexpr (std::is_floating_point_v<T>)
    {
        if (static_cast<double>(value) < metadata.minValue.f || static_cast<double>(value) > metadata.maxValue.f)
        {
            isInBounds = true;
        }
    }
    else if constexpr (std::is_unsigned_v<T>)
    {
        if (static_cast<uint64_t>(value) < metadata.minValue.u || static_cast<uint64_t>(value) > metadata.maxValue.u)
        {
            isInBounds = true;
        }
    }
    else if (static_cast<int64_t>(value) < metadata.minValue.i || static_cast<int64_t>(value) > metadata.maxValue.i)
    {
            isInBounds = true;
    }
    return isInBounds;
}

uint32_t softwareCRC(uint32_t crc, uint8_t const *buf, uint32_t len) noexcept;
}