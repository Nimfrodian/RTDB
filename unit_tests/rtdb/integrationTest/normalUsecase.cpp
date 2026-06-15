#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"
#include "test_types.hpp"

using namespace rtdb;

rtdb_error_t expectedError = RTDB_OK;
void error_handler(rtdb_error_t err, const char* context)
{
    UNITY_TEST_ASSERT_EQUAL_UINT32(expectedError, err, __LINE__, context);
};


void setUp(){}
void tearDown(){}

void normalUsecase(void)
{
    rtdb_init_t rtdb_cfg =
    {
        .crc_handler = nullptr, // TODO: add actual functions that do something
        .error_handler = error_handler
    };
    init(rtdb_cfg);

    rtdb_error_t err = configVar(SOME_MODULE_MYARR_UINT8,
                                static_cast<uint8_t>(3),
                                static_cast<uint8_t>(2),
                                static_cast<uint8_t>(15),
                                5
                                );

    for (int i = 0; i < 5; i++)
    {
        setVar(SOME_MODULE_MYARR_UINT8, static_cast<uint8_t>(2+i), i);
        uint8_t myArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, myArrVal, i);
        UNITY_TEST_ASSERT_EQUAL_UINT8(2 + i, myArrVal, __LINE__, "Setting and getting values do not match expected values!");
    }

    setVarControl(SOME_MODULE_MYARR_UINT8, RTDB_CONTROL_OVERRIDE);
    for (int i = 0; i < 5; i++)
    {
        uint8_t prevMyArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, prevMyArrVal, i);
        setVar(SOME_MODULE_MYARR_UINT8, static_cast<uint8_t>(2 + i), i);
        uint8_t newMyArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, newMyArrVal, i);
        UNITY_TEST_ASSERT_EQUAL_UINT8(newMyArrVal, prevMyArrVal, __LINE__, "Value was updated despite override lock being active!");

        overrideVar(SOME_MODULE_MYARR_UINT8, static_cast<uint8_t>(2 + i), i);
        uint8_t overridenMyArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, overridenMyArrVal, i);
        UNITY_TEST_ASSERT_EQUAL_UINT8(2 + i, overridenMyArrVal, __LINE__, "Override operation did not update the value as expected!");
    }

    setVarControl(SOME_MODULE_MYARR_UINT8, RTDB_CONTROL_NORMAL);
    for (int i = 0; i < 5; i++)
    {
        uint8_t prevMyArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, prevMyArrVal, i);
        overrideVar(SOME_MODULE_MYARR_UINT8, static_cast<uint8_t>(3 + i), i);
        uint8_t newMyArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, newMyArrVal, i);
        UNITY_TEST_ASSERT_EQUAL_UINT8(newMyArrVal, prevMyArrVal, __LINE__, "Override operation should be disabled when control is set to normal!");

        setVar(SOME_MODULE_MYARR_UINT8, static_cast<uint8_t>(3 + i), i);
        uint8_t overridenMyArrVal;
        getVar(SOME_MODULE_MYARR_UINT8, overridenMyArrVal, i);
        UNITY_TEST_ASSERT_EQUAL_UINT8(3 + i, overridenMyArrVal, __LINE__, "Set operation failed to update value when control is normal!");
    }
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(normalUsecase);

    return UNITY_END();
}