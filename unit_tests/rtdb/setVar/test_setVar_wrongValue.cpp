#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"
#include "test_types.hpp"

using namespace rtdb;

#define ARR_SIZE 10

void setUp()
{
    standard_init();
}
void tearDown(){}

template<typename T>
static void test_setVar_signed_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_VALUE_OUT_OF_RANGE;

    configVar<T>(SOME_MODULE_MYVAR, 0, -100, 100, ARR_SIZE); // Configure as array to allow index testing
    rtdb_error_t err;
    err = setVar<T>(
        SOME_MODULE_MYVAR,
        101,
        ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);

    err = setVar<T>(
    SOME_MODULE_MYVAR,
    -101,
    ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);

    expected_error = RTDB_OK;
    err = setVar<T>(
    SOME_MODULE_MYVAR,
    -99,
    ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

template<typename T>
static void test_setVar_unsigned_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_VALUE_OUT_OF_RANGE;

    configVar<T>(SOME_MODULE_MYVAR, 11, 10, 100, ARR_SIZE); // Configure as array to allow index testing
    rtdb_error_t err;
    err = setVar<T>(
        SOME_MODULE_MYVAR,
        101,
        ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);

    err = setVar<T>(
    SOME_MODULE_MYVAR,
    9,
    ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);

    expected_error = RTDB_OK;
    err = setVar<T>(
    SOME_MODULE_MYVAR,
    12,
    ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

template<typename T>
static void test_setVar_special_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_VALUE_OUT_OF_RANGE;

    configVar<T>(SOME_MODULE_MYVAR, 0, 0, 0, ARR_SIZE); // Configure as array to allow index testing
    rtdb_error_t err;
    err = setVar<T>(
        SOME_MODULE_MYVAR,
        1,
        ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);

    configVar<T>(SOME_MODULE_MYVAR, 1, 1, 1, ARR_SIZE); // Configure as array to allow index testing
    err = setVar<T>(
    SOME_MODULE_MYVAR,
    0,
    ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);

    configVar<T>(SOME_MODULE_MYVAR, 0, 0, 1, ARR_SIZE); // Configure as array to allow index testing
    expected_error = RTDB_OK;
    err = setVar<T>(
    SOME_MODULE_MYVAR,
    1,
    ARR_SIZE-1
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

#define X(T) \
void test_setVar_signed_##T() \
{ \
    test_setVar_signed_impl<T>(); \
}
RTDB_SIGNED_TYPES
#undef X
#define X(T) \
void test_setVar_unsigned_##T() \
{ \
    test_setVar_unsigned_impl<T>(); \
}
RTDB_UNSIGNED_TYPES
#undef X
#define X(T) \
void test_setVar_special_##T() \
{ \
    test_setVar_special_impl<T>(); \
}
RTDB_SPECIAL_TYPES
#undef X


void register_setVar_tests()
{
#define X(T) RUN_TEST(test_setVar_signed_##T);
RTDB_SIGNED_TYPES
#undef X

#define X(T) RUN_TEST(test_setVar_unsigned_##T);
RTDB_UNSIGNED_TYPES
#undef X

#define X(T) RUN_TEST(test_setVar_special_##T);
RTDB_SPECIAL_TYPES
#undef X
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(register_setVar_tests);

    return UNITY_END();
}