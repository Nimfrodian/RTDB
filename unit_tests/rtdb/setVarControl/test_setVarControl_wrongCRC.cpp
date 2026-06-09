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

    configVar<int32_t>(SOME_MODULE_MYVAR, 0, -100, 100);
}
void tearDown(){}


static void test_setVarControl_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_CRC_MISMATCH;

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