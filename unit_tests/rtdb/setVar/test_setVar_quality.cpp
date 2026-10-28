#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"

using namespace rtdb;

static uint32_t crc_call_count = 0;

static uint32_t counting_crc(uint32_t crc, const uint8_t* buf, uint32_t len)
{
    ++crc_call_count;
    return test_crc32(crc, buf, len);
}

void setUp()
{
    standard_init();
}
void tearDown() {}

static void test_configVar_metadata_defaults()
{
    constexpr rtdb_id_t id = SOME_MODULE_MYVAR;

    TEST_ASSERT_EQUAL(RTDB_OK, configVar<int32_t>(id, 5, 0, 10));
    TEST_ASSERT_EQUAL_STRING("", rtdb_vars[id].unit);
    TEST_ASSERT_EQUAL(RTDB_QUALITY_UNRELIABLE, rtdb_vars[id].quality);
    TEST_ASSERT_EQUAL_STRING("", rtdb_vars[id].comment);
    TEST_ASSERT_EQUAL(RTDB_OK, removeVar(id));
}

static void test_configVar_accepts_metadata()
{
    constexpr rtdb_id_t id = SOME_MODULE_MYVAR;

    TEST_ASSERT_EQUAL(
        RTDB_OK,
        configVar<int32_t>(id, 5, 0, 10, 1, "V", "Sensor voltage", RTDB_QUALITY_GOOD));
    TEST_ASSERT_EQUAL_STRING("V", rtdb_vars[id].unit);
    TEST_ASSERT_EQUAL_STRING("Sensor voltage", rtdb_vars[id].comment);
    TEST_ASSERT_EQUAL(RTDB_QUALITY_GOOD, rtdb_vars[id].quality);
    TEST_ASSERT_EQUAL(RTDB_OK, removeVar(id));
}

static void test_setVar_sets_quality_with_one_crc_update()
{
    constexpr rtdb_id_t id = SOME_MODULE_MYVAR;
    int32_t value = 0;
    rtdb_init_t cfg{};
    cfg.crc_handler = counting_crc;
    cfg.error_handler = rtdb_error_handler;

    init(cfg);
    TEST_ASSERT_EQUAL(RTDB_OK, configVar<int32_t>(id, 5, 0, 10));
    crc_call_count = 0;

    TEST_ASSERT_EQUAL(RTDB_OK, setVar(id, 7, RTDB_QUALITY_BAD));
    TEST_ASSERT_EQUAL_UINT32(4, crc_call_count);
    TEST_ASSERT_EQUAL(RTDB_QUALITY_BAD, rtdb_vars[id].quality);
    TEST_ASSERT_EQUAL(RTDB_OK, getVar(id, value));
    TEST_ASSERT_EQUAL_INT32(7, value);

    TEST_ASSERT_EQUAL(RTDB_OK, setVar(id, 8));
    TEST_ASSERT_EQUAL(RTDB_QUALITY_GOOD, rtdb_vars[id].quality);
    TEST_ASSERT_EQUAL(RTDB_OK, setVarQuality(id, RTDB_QUALITY_BAD));
    TEST_ASSERT_EQUAL(RTDB_QUALITY_BAD, rtdb_vars[id].quality);
    TEST_ASSERT_EQUAL(RTDB_OK, removeVar(id));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_configVar_metadata_defaults);
    RUN_TEST(test_configVar_accepts_metadata);
    RUN_TEST(test_setVar_sets_quality_with_one_crc_update);
    return UNITY_END();
}