#pragma once
#include <string>
#include <vector>

namespace gfx {
inline namespace v1 {
struct dynamic_library_t
{
    dynamic_library_t() = default;
    dynamic_library_t(std::string name);
    dynamic_library_t(const dynamic_library_t& other) = delete;
    dynamic_library_t(dynamic_library_t&& other)      = default;
    dynamic_library_t& operator=(const dynamic_library_t& other) = delete;
    dynamic_library_t& operator=(dynamic_library_t&& other) = default;
    ~dynamic_library_t();

    void load(std::string name);
    void reload();
    void unload();

    template<typename T>
    std::decay_t<T> const* get_address(const std::string& name) const;

private:
    void* get_proc_address_impl(const char* name) const;
    void  load_impl();
    void  unload_impl() const;

    std::string _name;
    void*       _handle = nullptr;
};

template<typename T>
std::decay_t<T> const* dynamic_library_t::get_address(const std::string& name) const
{
    return reinterpret_cast<std::decay_t<T> const*>(get_proc_address_impl(name.c_str()));
}

using imported_shader_t = std::vector<std::uint32_t>;
const imported_shader_t* import_shader(const dynamic_library_t& lib, std::string path);

}    // namespace v1
}    // namespace gfx