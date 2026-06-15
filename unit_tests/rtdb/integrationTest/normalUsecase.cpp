#include "unity.h"
#include "rtdb.h"
#include "framework/test_context.hpp"
#include "test_types.hpp"

using namespace rtdb;


void setUp(){}
void tearDown(){}

void normalUsecase(void)
{

}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(normalUsecase);

    return UNITY_END();
}