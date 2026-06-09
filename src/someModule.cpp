#include "someModule.h"
#include <cstdint>
#include <iostream>

#define MY_ARR_SIZE 12

void someModule_init(void)
{
    rtdb::configVar<int32_t>(rtdb::SOME_MODULE_MYVAR, 0, -3, 5);
    rtdb::configVar<uint32_t>(rtdb::SOME_MODULE_MYARR, 0, 0, 1234, MY_ARR_SIZE);
}

void someModule_run(void)
{
    static int counter = 0;
    rtdb::setVar(rtdb::SOME_MODULE_MYVAR, -counter);
    rtdb::setVar(rtdb::SOME_MODULE_MYARR, counter*2, counter);

    int32_t temp_var;
    std::cout << "SOME_MODULE_MYVAR: " << rtdb::getVar<int32_t>(rtdb::SOME_MODULE_MYVAR, temp_var) << std::endl;

    uint32_t temp_arr[MY_ARR_SIZE];
    for (int i = 0; i < MY_ARR_SIZE; i++) {
        std::cout << "SOME_MODULE_MYARR[" << i << "]: " << rtdb::getVar<uint32_t>(rtdb::SOME_MODULE_MYARR, temp_arr[i], i) << std::endl;
    }

    counter++;
    std::cout << "---------------------" << std::endl;
}
