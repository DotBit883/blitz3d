#include "linker.h"
#include <iostream>

void __attribute__((constructor)) linker_dll_init() { linkerGetLinker(); }
