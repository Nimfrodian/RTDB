#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"
#include "test_types.hpp"

using namespace rtdb;

uint32_t corrupted_crc32(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    static uint32_t call_count = 0;
    call_count++;
    return call_count;
}

void setUp()
{
    rtdb_init_t cfg{};
    cfg.crc_fn = corrupted_crc32;
    cfg.error_handler = rtdb_error_handler;
    init(cfg);

}
void tearDown(){}

template<typename T>
static void test_getVar_signed_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_CRC_MISMATCH;

    configVar<T>(SOME_MODULE_MYVAR, 0, -100, 100);

    T var = -1;
    rtdb_error_t err = getVar<T>(
        SOME_MODULE_MYVAR,
        var
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
    TEST_ASSERT_EQUAL(var, static_cast<T>(-1)); // unchanged
}

template<typename T>
static void test_getVar_unsigned_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_CRC_MISMATCH;

    configVar<T>(SOME_MODULE_MYVAR, 0, 0, 100);
    T var = 10;
    rtdb_error_t err = getVar<T>(
        SOME_MODULE_MYVAR,
        var
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
    TEST_ASSERT_EQUAL(var, static_cast<T>(10)); // unchanged
}

template<typename T>
static void test_getVar_special_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_CRC_MISMATCH;

    configVar<T>(SOME_MODULE_MYVAR, 0, 0, 1);
    T var = 1;
    rtdb_error_t err = getVar<T>(
        SOME_MODULE_MYVAR,
        var
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
    TEST_ASSERT_EQUAL(var, static_cast<T>(1)); // unchanged
}


#define X(T) \
void test_getVar_signed_##T() \
{ \
    test_getVar_signed_impl<T>(); \
}
RTDB_SIGNED_TYPES
#undef X

#define X(T) \
void test_getVar_unsigned_##T() \
{ \
    test_getVar_unsigned_impl<T>(); \
}
RTDB_UNSIGNED_TYPES
#undef X

#define X(T) \
void test_getVar_special_##T() \
{ \
    test_getVar_special_impl<T>(); \
}
RTDB_SPECIAL_TYPES
#undef X



void register_getVar_tests()
{
#define X(T) RUN_TEST(test_getVar_signed_##T);
RTDB_SIGNED_TYPES
#undef X

#define X(T) RUN_TEST(test_getVar_unsigned_##T);
RTDB_UNSIGNED_TYPES
#undef X

#define X(T) RUN_TEST(test_getVar_special_##T);
RTDB_SPECIAL_TYPES
#undef X
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(register_getVar_tests);
    RUN_TEST(register_getVar_tests); // Run twice to check memory clearing

    return UNITY_END();
}