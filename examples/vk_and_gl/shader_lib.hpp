#pragma once
#include <vector>

namespace gfx
{
    inline namespace v1
    {
        void* load_lib(const std::string& lib);
        const std::vector<std::uint32_t>* load_shader(void* lib_handle, std::string path);
    }
}
