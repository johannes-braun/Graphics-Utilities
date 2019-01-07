#include "shader_lib.hpp"
#include <algorithm>
#include <windows.h>

namespace gfx {
inline namespace v1 {
void* load_lib(const std::string& lib)
{
    return LoadLibraryA(lib.c_str());
}

const std::vector<std::uint32_t>* load_shader(void* lib_handle, std::string path)
{
    for (auto& c : path)
        if (c == '\\' || c == '/' || c == '.' || c == ' ' || c == '-') c = '_';
    return reinterpret_cast<const std::vector<std::uint32_t>*>(GetProcAddress(static_cast<HMODULE>(lib_handle), path.c_str()));
}
}    // namespace v1
}    // namespace gfx
