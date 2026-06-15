#pragma once
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstdint>
#include <array>
#include "rtdb_vars.h"
#include "rtdb_types.h"
#include "rtdb_utils.h"


namespace rtdb  {

extern bool initialized;
extern std::array<rtdb_var_t, RTDB_SIZE> rtdb_vars;

using rtdb_crc_handler_t = uint32_t (*)(uint32_t crc, uint8_t const *buf, uint32_t len);
using rtdb_error_handler_t = void(*)(rtdb_error_t err, const char* context);

typedef struct {
    rtdb_crc_handler_t crc_handler;
    rtdb_error_handler_t error_handler;  // Optional error callback
} rtdb_init_t;

void report_error(rtdb_error_t err, const char* context);

void init(const rtdb_init_t& cfg);
uint32_t calculateCRC(const rtdb_var_t *var);
rtdb_error_t setVarControl(rtdb_id_t id, rtdb_control_t controlType);

template<typename T>
constexpr rtdb_type_t getTypeForT() {
    if constexpr (std::is_same_v<T, bool>) return RTDB_TYPE_BOOL;
    else if constexpr (std::is_same_v<T, int8_t>) return RTDB_TYPE_INT8;
    else if constexpr (std::is_same_v<T, uint8_t>) return RTDB_TYPE_UINT8;
    else if constexpr (std::is_same_v<T, int16_t>) return RTDB_TYPE_INT16;
    else if constexpr (std::is_same_v<T, uint16_t>) return RTDB_TYPE_UINT16;
    else if constexpr (std::is_same_v<T, int32_t>) return RTDB_TYPE_INT32;
    else if constexpr (std::is_same_v<T, uint32_t>) return RTDB_TYPE_UINT32;
    else if constexpr (std::is_same_v<T, int64_t>) return RTDB_TYPE_INT64;
    else if constexpr (std::is_same_v<T, uint64_t>) return RTDB_TYPE_UINT64;
    else if constexpr (std::is_same_v<T, float>) return RTDB_TYPE_FLOAT;
    else if constexpr (std::is_same_v<T, double>) return RTDB_TYPE_DOUBLE;
}

template<typename T>
rtdb_error_t configVar(rtdb_id_t id, T defaultValue, T min, T max, uint32_t size = 1)
{
    rtdb_error_t err = RTDB_ERR_UNDEFINED;
    if (!initialized) {
        err = RTDB_ERR_MODULE_UNINITIALIZED;
        report_error(err, "rtdb::configVar: RTDB not initialized");
    }
    else if (id >= RTDB_SIZE)    {
        err = RTDB_ERR_INVALID_ID;
        report_error(err, "rtdb::configVar: Invalid ID");
    }
    else if ((rtdb_vars[id].data != nullptr) && (calculateCRC(&rtdb_vars[id]) != rtdb_vars[id].crc))
    {
        err = RTDB_ERR_CRC_MISMATCH;
        report_error(err, "rtdb::configVar: CRC mismatch");
    }
    else if  (defaultValue < min || defaultValue > max)
    {
        err = RTDB_ERR_DEFAULT_VALUE_OUT_OF_RANGE;
        report_error(err, "rtdb::configVar: Default value out of range");
    }
    else
    {
        rtdb_var_t& oldVar = rtdb_vars[id];
        err = freeData(oldVar.type, oldVar.data);

        rtdb_var_t var;
        var.id = id;
        var.type = getTypeForT<T>();
        var.size = size;
        var.elementSize = sizeof(T);
        var.controlType = RTDB_CONTROL_NORMAL;
        var.data = new T[size];
        T* arr = static_cast<T*>(var.data);
        for (uint32_t i = 0; i < size; ++i)
        {
            arr[i] = defaultValue;
        }

        if constexpr (std::is_floating_point_v<T>)
        {
            var.minValue.f = static_cast<double>(min);
            var.maxValue.f = static_cast<double>(max);
            var.defValue.f = static_cast<double>(defaultValue);
        }
        else if constexpr (std::is_unsigned_v<T>)
        {
            var.minValue.u = static_cast<uint64_t>(min);
            var.maxValue.u = static_cast<uint64_t>(max);
            var.defValue.u = static_cast<uint64_t>(defaultValue);
        }
        else
        {
            var.minValue.i = static_cast<int64_t>(min);
            var.maxValue.i = static_cast<int64_t>(max);
            var.defValue.i = static_cast<int64_t>(defaultValue);
        }

        var.crc = calculateCRC(&var);
        rtdb_vars[id] = var;
        err = RTDB_OK;
    }
    return err;
}

template<typename T>
rtdb_error_t setVar(const rtdb_id_t id, T value, uint32_t index = 0)
{
    rtdb_error_t err = RTDB_ERR_UNDEFINED;
    if (!initialized) {
        err = RTDB_ERR_MODULE_UNINITIALIZED;
        report_error(err, "rtdb::setVar: RTDB not initialized");
    }
    else if (id >= RTDB_SIZE)
    {
        err = RTDB_ERR_INVALID_ID;
        report_error(err, "rtdb::setVar: Invalid ID");
    }
    else if (calculateCRC(&rtdb_vars[id]) != rtdb_vars[id].crc)
    {
        err = RTDB_ERR_CRC_MISMATCH;
        report_error(err, "rtdb::setVar: CRC mismatch");
    }
    else if (rtdb_vars[id].data == nullptr)
    {
        err = RTDB_ERR_VAR_NOT_CONFIGURED;
        report_error(err, "rtdb::setVar: Variable not configured");
    }
    else if (index >= rtdb_vars[id].size)
    {
        err = RTDB_ERR_INDEX_OUT_OF_RANGE;
        report_error(err, "rtdb::setVar: Index out of range");
    }
    else if (rtdb_vars[id].type != getTypeForT<T>())
    {
        err = RTDB_ERR_TYPE_MISMATCH;
        report_error(err, "rtdb::setVar: Variable type mismatch");
    }
    else if  (checkIfOutOfBounds(value, rtdb_vars[id]))
    {
        err = RTDB_ERR_VALUE_OUT_OF_RANGE;
        report_error(err, "rtdb::setVar: Value out of range");
    }
    else
    {
        if (rtdb_vars[id].controlType == RTDB_CONTROL_NORMAL)
        {
            T* arr = static_cast<T*>(rtdb_vars[id].data);
            arr[index] = value;
            rtdb_vars[id].crc = calculateCRC(&rtdb_vars[id]);
            err = RTDB_OK;
        }
    }
    return err;
}

template<typename T>
rtdb_error_t getVar(const rtdb_id_t id, T& outValue, uint32_t index = 0)
{
    rtdb_error_t err = RTDB_ERR_UNDEFINED;
    if (!initialized) {
        err = RTDB_ERR_MODULE_UNINITIALIZED;
        report_error(err, "rtdb::getVar: RTDB not initialized");
    }
    else if (id >= RTDB_SIZE)
    {
        err = RTDB_ERR_INVALID_ID;
        report_error(err, "rtdb::getVar: Invalid ID");
    }
    else if (calculateCRC(&rtdb_vars[id]) != rtdb_vars[id].crc)
    {
        err = RTDB_ERR_CRC_MISMATCH;
        report_error(err, "rtdb::getVar: CRC mismatch");
    }
    else if (rtdb_vars[id].data == nullptr)
    {
        err = RTDB_ERR_VAR_NOT_CONFIGURED;
        report_error(err, "rtdb::getVar: Variable not configured");
    }
    else if (index >= rtdb_vars[id].size)
    {
        err = RTDB_ERR_INDEX_OUT_OF_RANGE;
        report_error(err, "rtdb::getVar: Index out of range");
    }
    else if (rtdb_vars[id].type != getTypeForT<T>())
    {
        err = RTDB_ERR_TYPE_MISMATCH;
        report_error(err, "rtdb::getVar: Variable type mismatch");
    }
    else
    {
        T* arr = static_cast<T*>(rtdb_vars[id].data);
        outValue = arr[index];
        err = RTDB_OK;
    }
    return err;
}

}