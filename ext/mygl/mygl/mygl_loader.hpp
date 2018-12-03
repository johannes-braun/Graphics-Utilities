#pragma once
/* This header contains the function loader functionalities.
*/
#include "mygl_functions.hpp"

namespace mygl { 
void load();
void load(loader_function fun);
void load(dispatch* d);
void load(dispatch* d, loader_function fun);
}

#if defined(MYGL_IMPLEMENTATION)
    #include "mygl_loader.inl"
#endif
