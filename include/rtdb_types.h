#pragma once

namespace rtdb
{
typedef enum {
    RTDB_OK = 0,
    RTDB_ERR_INDEX_OUT_OF_RANGE,
    RTDB_ERR_VALUE_OUT_OF_RANGE,
    RTDB_ERR_DEFAULT_VALUE_OUT_OF_RANGE,
    RTDB_ERR_CRC_MISMATCH,
    RTDB_ERR_MODULE_UNINITIALIZED,
    RTDB_ERR_VAR_NOT_CONFIGURED,
    RTDB_ERR_INVALID_ID,

    RTDB_ERR_SIZE,
    RTDB_ERR_UNDEFINED
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
}