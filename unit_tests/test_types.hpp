#pragma once

#define RTDB_TYPES \
    X(int8_t)      \
    X(uint8_t)     \
    X(int16_t)     \
    X(uint16_t)    \
    X(int32_t)     \
    X(uint32_t)    \
    X(int64_t)     \
    X(uint64_t)    \
    X(float)       \
    X(double)

#define RTDB_UNSIGNED_TYPES \
    X(uint8_t)     \
    X(uint16_t)    \
    X(uint32_t)    \
    X(uint64_t)

#define RTDB_SIGNED_TYPES \
    X(int8_t)      \
    X(int16_t)     \
    X(int32_t)     \
    X(int64_t)     \
    X(float)       \
    X(double)

#define RTDB_SPECIAL_TYPES X(bool)