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


static void test_setVarControl_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    rtdb_error_t err = setVarControl(
        SOME_MODULE_MYVAR,
        RTDB_CONTROL_NORMAL
    );
    TEST_ASSERT_EQUAL_UINT32(expected_error, err);
}

void register_setVarControl_tests()
{
    test_setVarControl_impl();
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(register_setVarControl_tests);

    return UNITY_END();
}