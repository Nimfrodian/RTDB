#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"
#include "test_types.hpp"

using namespace rtdb;

void setUp()
{
    standard_init();
}
void tearDown(){}

static void test_setVar_signed_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_TYPE_MISMATCH;


    rtdb_error_t err = configVar(SOME_MODULE_MYVAR, static_cast<uint8_t>(1), static_cast<uint8_t>(0), static_cast<uint8_t>(2));

    err = setVar(
        SOME_MODULE_MYVAR,
        static_cast<int8_t>(1)
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_setVar_signed_impl);

    return UNITY_END();
}