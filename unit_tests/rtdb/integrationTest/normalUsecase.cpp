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
        UNITY_TEST_ASSERT_EQUAL_UINT8(2+i, myArrVal, __LINE__, "Setting and getting returns different values!");
    }

}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(normalUsecase);

    return UNITY_END();
}