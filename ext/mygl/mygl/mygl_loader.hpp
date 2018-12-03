#pragma once
/* This header contains the function loader functionalities.
*/
#include "mygl_functions.hpp"

namespace mygl { 
using loader_function = void*(*)(const char* name);
void load();
void load(loader_function fun);
}

#if defined(MYGL_IMPLEMENTATION)
    #include "mygl_loader.inl"
#endif
