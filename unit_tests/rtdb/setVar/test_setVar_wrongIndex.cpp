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
    rtdb_error_t expected_error = RTDB_ERR_INDEX_OUT_OF_RANGE;

    configVar<T>(SOME_MODULE_MYVAR, 0, -100, 100, ARR_SIZE); // Configure as array to allow index testing
    rtdb_error_t err = setVar<T>(
        SOME_MODULE_MYVAR,
        1,
        ARR_SIZE
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

#define X(T) \
void test_setVar_signed_##T() \
{ \
    test_setVar_signed_impl<T>(); \
}
RTDB_TYPES
#undef X



void register_setVar_tests()
{
#define X(T) RUN_TEST(test_setVar_signed_##T);
RTDB_TYPES
#undef X
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(register_setVar_tests);

    return UNITY_END();
}