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

template<typename T>
static void test_setVar_signed_impl()
{
    rtdb_error_t expected_error = RTDB_ERR_MODULE_UNINITIALIZED;

    for (int i = 0; i < 3; i++) {
        rtdb_error_t err = setVar<T>(
            SOME_MODULE_MYVAR,
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