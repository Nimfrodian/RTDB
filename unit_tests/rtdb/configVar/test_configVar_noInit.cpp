#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"
#include "test_types.hpp"

using namespace rtdb;

void setUp()
{
    // NO standard_init();
}
void tearDown(){}

template<typename T>
static void test_configVar_signed_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    rtdb_error_t err = configVar<T>(
        SOME_MODULE_MYVAR,
        static_cast<T>(-5),
        static_cast<T>(-10),
        static_cast<T>(+10)
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

template<typename T>
static void test_configVar_unsigned_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    rtdb_error_t err = configVar<T>(
        SOME_MODULE_MYVAR,
        static_cast<T>(5),
        static_cast<T>(2),
        static_cast<T>(10)
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

template<typename T>
static void test_configVar_special_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    rtdb_error_t err = configVar<T>(
        SOME_MODULE_MYVAR,
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}


#define X(T) \
void test_configVar_signed_##T() \
{ \
    test_configVar_signed_impl<T>(); \
}
RTDB_SIGNED_TYPES
#undef X

#define X(T) \
void test_configVar_unsigned_##T() \
{ \
    test_configVar_unsigned_impl<T>(); \
}
RTDB_UNSIGNED_TYPES
#undef X

#define X(T) \
void test_configVar_special_##T() \
{ \
    test_configVar_special_impl<T>(); \
}
RTDB_SPECIAL_TYPES
#undef X



void register_configVar_tests()
{
#define X(T) RUN_TEST(test_configVar_signed_##T);
RTDB_SIGNED_TYPES
#undef X

#define X(T) RUN_TEST(test_configVar_unsigned_##T);
RTDB_UNSIGNED_TYPES
#undef X

#define X(T) RUN_TEST(test_configVar_special_##T);
RTDB_SPECIAL_TYPES
#undef X
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(register_configVar_tests);

    return UNITY_END();
}