#pragma once
#include "rtdb.h"

#define SOME_MODULE     5 // this value is defined somewhere else, on a list of modules, and is used to identify this module in the system. It is unique across all modules.

void someModule_init(void);
void someModule_run(void);