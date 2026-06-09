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
static void test_getVar_signed_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    for (int i = 0; i < 3; i++) {
        T var = -1;
        rtdb_error_t err = getVar<T>(
            SOME_MODULE_MYVAR,
            var,
            i
        );
        TEST_ASSERT_EQUAL_UINT32(expected_error, err);
        TEST_ASSERT_EQUAL(static_cast<T>(-1), var);
    }
}

template<typename T>
static void test_getVar_unsigned_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    for (int i = 0; i < 3; i++) {
        T var = 1;
        rtdb_error_t err = getVar<T>(
            SOME_MODULE_MYVAR,
            var,
            i
        );
        TEST_ASSERT_EQUAL_UINT32(expected_error, err);
        TEST_ASSERT_EQUAL(static_cast<T>(1), var); // unchanged
    }
}

template<typename T>
static void test_getVar_special_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    for (int i = 0; i < 3; i++) {
        T var = 1;
        rtdb_error_t err = getVar<T>(
            SOME_MODULE_MYVAR,
            var,
            i
        );
        TEST_ASSERT_EQUAL_UINT32(expected_error, err);
        TEST_ASSERT_EQUAL(static_cast<T>(1), var); // unchanged
    }
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

    return UNITY_END();
}