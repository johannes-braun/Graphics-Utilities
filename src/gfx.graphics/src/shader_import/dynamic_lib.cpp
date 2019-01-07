#include "shader_import/dynamic_lib.hpp"
#if __has_include(<windows.h>)
#include <windows.h>
#endif

namespace gfx {
inline namespace v1 {
dynamic_library_t::dynamic_library_t(std::string name)
{
    load(std::move(name));
}

dynamic_library_t::~dynamic_library_t()
{
    unload();
}

void dynamic_library_t::load(std::string name)
{
    _name = std::move(name);
    reload();
}

void dynamic_library_t::reload()
{
    unload();
    load_impl();
}

void dynamic_library_t::unload()
{
    if (_handle) unload_impl();
    _handle = nullptr;
}


void* dynamic_library_t::get_proc_address_impl(const char* name) const
{
#if defined(_WIN32)
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(_handle), name));
#else
#error "Not implemented."
#endif
}

void dynamic_library_t::load_impl()
{
#if defined(_WIN32)
    _handle = LoadLibraryA(_name.c_str());
#else
#error "Not implemented."
#endif
}

void dynamic_library_t::unload_impl() const
{
#if defined(_WIN32)
    FreeLibrary(static_cast<HMODULE>(_handle));
#else
#error "Not implemented."
#endif
}

const imported_shader_t* import_shader(const dynamic_library_t& lib, std::string path)
{
    for (auto& c : path)
        if (c == '\\' || c == '/' || c == '.' || c == ' ' || c == '-') c = '_';
    return lib.get_address<imported_shader_t>(path);
}
}    // namespace v1
}    // namespace gfx