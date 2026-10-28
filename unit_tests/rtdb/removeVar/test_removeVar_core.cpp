#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"

using namespace rtdb;

void setUp()
{
    standard_init();
}
void tearDown() {}

static void test_removeVar_clears_variable()
{
    constexpr rtdb_id_t id = SOME_MODULE_MYVAR;
    int32_t value = 0;

    TEST_ASSERT_EQUAL(RTDB_OK, configVar<int32_t>(id, 5, 0, 10));
    TEST_ASSERT_EQUAL(RTDB_OK, removeVar(id));
    TEST_ASSERT_EQUAL(RTDB_ERR_VAR_NOT_CONFIGURED, getVar(id, value));
    TEST_ASSERT_EQUAL(RTDB_ERR_VAR_NOT_CONFIGURED, removeVar(id));
}

static void test_zero_size_preserves_existing_variable()
{
    constexpr rtdb_id_t id = SOME_MODULE_MYVAR;
    int32_t value = 0;

    TEST_ASSERT_EQUAL(RTDB_OK, configVar<int32_t>(id, 5, 0, 10));
    TEST_ASSERT_EQUAL(RTDB_ERR_SIZE, configVar<int32_t>(id, 7, 0, 10, 0));
    TEST_ASSERT_EQUAL(RTDB_OK, getVar(id, value));
    TEST_ASSERT_EQUAL_INT32(5, value);
    TEST_ASSERT_EQUAL(RTDB_OK, removeVar(id));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_removeVar_clears_variable);
    RUN_TEST(test_zero_size_preserves_existing_variable);
    return UNITY_END();
}
