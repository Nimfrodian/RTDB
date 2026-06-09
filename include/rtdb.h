#pragma once
#include <cstdint>
#include <iostream>
#include <type_traits>
#include "rtdb_vars.h"


namespace rtdb  {

extern bool initialized;

typedef enum {
    RTDB_OK = 0,
    RTDB_ERR_INDEX_OUT_OF_RANGE,
    RTDB_ERR_VALUE_OUT_OF_RANGE,
    RTDB_ERR_DEFAULT_VALUE_OUT_OF_RANGE,
    RTDB_ERR_CRC_MISMATCH,
    RTDB_ERR_MODULE_UNINITIALIZED,
    RTDB_ERR_VAR_NOT_CONFIGURED,
    RTDB_ERR_INVALID_ID,

    RTDB_ERR_UNDEFINED,
    RTDB_ERR_SIZE
} rtdb_error_t;

typedef enum {
    RTDB_TYPE_UNDEFINED = 0,
    RTDB_TYPE_BOOL,
    RTDB_TYPE_INT8,
    RTDB_TYPE_UINT8,
    RTDB_TYPE_INT16,
    RTDB_TYPE_UINT16,
    RTDB_TYPE_INT32,
    RTDB_TYPE_UINT32,
    RTDB_TYPE_INT64,
    RTDB_TYPE_UINT64,
    RTDB_TYPE_FLOAT,
    RTDB_TYPE_DOUBLE,

    RTDB_TYPE_SIZE
} rtdb_type_t;

typedef enum {
    RTDB_CONTROL_UNDEFINED = 0,
    RTDB_CONTROL_OVERRIDE,
    RTDB_CONTROL_NORMAL,
} rtdb_control_t;

typedef union {
    int64_t i;
    uint64_t u;
    double f;
} rtdb_value_t;

typedef struct {
    rtdb_id_t id;
    rtdb_type_t type;
    uint32_t size;
    uint32_t elementSize;
    rtdb_value_t defValue;
    rtdb_value_t minValue;
    rtdb_value_t maxValue;
    rtdb_control_t controlType;
    void* data;
    uint32_t crc;
} rtdb_var_t;
extern rtdb_var_t rtdb_vars[RTDB_SIZE];

uint32_t calculateCRC(const rtdb_var_t *var);

// Error handler callback type
using rtdb_error_handler_t = void(*)(rtdb_error_t err, const char* context);

// Init structure passed to rtdb init
typedef struct {
    uint32_t (*crc_fn)(uint32_t crc, uint8_t const *buf, uint32_t len);
    rtdb_error_handler_t error_handler;  // Optional error callback
} rtdb_init_t;

extern rtdb_error_handler_t g_error_handler;
static void report_error(rtdb_error_t err, const char* context)
{
    if (g_error_handler) {
        g_error_handler(err, context);
    }
}

void init(const rtdb_init_t& cfg);
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

static rtdb_error_t freeData(rtdb_type_t type, void* data)
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
    else
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            if (static_cast<double>(defaultValue) < min || static_cast<double>(defaultValue) > max)
            {
                err = RTDB_ERR_DEFAULT_VALUE_OUT_OF_RANGE;
                report_error(err, "rtdb::configVar: Default value out of range");
            }
        }
        else if constexpr (std::is_unsigned_v<T>)
        {
            if (static_cast<uint64_t>(defaultValue) < min || static_cast<uint64_t>(defaultValue) > max)
            {
                err = RTDB_ERR_DEFAULT_VALUE_OUT_OF_RANGE;
                report_error(err, "rtdb::configVar: Default value out of range");
            }
        }
        else if (static_cast<int64_t>(defaultValue) < min || static_cast<int64_t>(defaultValue) > max)
        {
            err = RTDB_ERR_DEFAULT_VALUE_OUT_OF_RANGE;
            report_error(err, "rtdb::configVar: Default value out of range");
        }

        if (err == RTDB_ERR_UNDEFINED)
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
    else if (calculateCRC(&rtdb_vars[id]) != rtdb_vars[id].crc)
    {
        err = RTDB_ERR_CRC_MISMATCH;
        report_error(err, "rtdb::setVar: CRC mismatch");
    }
    else
    {
        rtdb_var_t var = rtdb_vars[id];
        if constexpr (std::is_floating_point_v<T>)
        {
            if (static_cast<double>(value) < var.minValue.f || static_cast<double>(value) > var.maxValue.f)
            {
                err = RTDB_ERR_VALUE_OUT_OF_RANGE;
                report_error(err, "rtdb::setVar: Index out of range");
            }
        }
        else if constexpr (std::is_unsigned_v<T>)
        {
            if (static_cast<uint64_t>(value) < var.minValue.u || static_cast<uint64_t>(value) > var.maxValue.u)
            {
                err = RTDB_ERR_VALUE_OUT_OF_RANGE;
                report_error(err, "rtdb::setVar: Value out of range");
            }
        }
        else if (static_cast<int64_t>(value) < var.minValue.i || static_cast<int64_t>(value) > var.maxValue.i)
        {
            err = RTDB_ERR_VALUE_OUT_OF_RANGE;
            report_error(err, "rtdb::setVar: Value out of range");
        }

        if (err == RTDB_ERR_UNDEFINED)
        {
            if (var.controlType == RTDB_CONTROL_NORMAL)
            {
                T* arr = static_cast<T*>(var.data);
                arr[index] = value;
                rtdb_vars[id].crc = calculateCRC(&var);
                err = RTDB_OK;
            }
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
    else if (calculateCRC(&rtdb_vars[id]) != rtdb_vars[id].crc)
    {
        err = RTDB_ERR_CRC_MISMATCH;
        report_error(err, "rtdb::getVar: CRC mismatch");
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