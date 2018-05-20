#pragma once
/* This header contains the function loader functionalities.
*/
#include "mygl_functions.hpp"

namespace mygl { 
    void load(); 
}

#if defined(MYGL_IMPLEMENTATION)
    #include "mygl_loader.inl"
#endif
