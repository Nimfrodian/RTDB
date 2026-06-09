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
    rtdb_error_t expected_error = RTDB_OK;

    configVar<T>(
        SOME_MODULE_MYVAR,
        static_cast<T>(-5),
        static_cast<T>(-10),
        static_cast<T>(ARR_SIZE),
        ARR_SIZE
    );

    for (int i = 0; i < ARR_SIZE; i++)
    {
        rtdb_error_t err = setVar<T>(
            SOME_MODULE_MYVAR,
            static_cast<T>(-5+i),
            i
        );
        TEST_ASSERT_EQUAL_UINT32(expected_error, err);
    }
}

template<typename T>
static void test_setVar_unsigned_impl()
{
    rtdb_error_t expected_error = RTDB_OK;

    configVar<T>(
        SOME_MODULE_MYVAR,
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(ARR_SIZE),
        ARR_SIZE
    );

    for (int i = 0; i < ARR_SIZE; i++)
    {
        rtdb_error_t err = setVar<T>(
            SOME_MODULE_MYVAR,
            static_cast<T>(i),
            i
        );
        TEST_ASSERT_EQUAL_UINT32(expected_error, err);
    }
}

template<typename T>
static void test_setVar_special_impl()
{
    rtdb_error_t expected_error = RTDB_OK;

    configVar<T>(
        SOME_MODULE_MYVAR,
        static_cast<T>(1),
        static_cast<T>(0),
        static_cast<T>(1),
        ARR_SIZE
    );

    for (int i = 0; i < ARR_SIZE; i++)
    {
        T var;
        rtdb_error_t err = setVar<T>(
            SOME_MODULE_MYVAR,
            var,
            i
        );
        TEST_ASSERT_EQUAL_UINT32(expected_error, err);
    }
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
    RUN_TEST(register_setVar_tests); // Run twice to check memory clearing

    return UNITY_END();
}