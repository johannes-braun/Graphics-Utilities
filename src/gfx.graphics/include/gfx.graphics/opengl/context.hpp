#pragma once

#include "enums.hpp"

#include "native_handle.hpp"
#include <cinttypes>
#include <vector>

namespace gfx {
inline namespace v1 {
class native_handle;
namespace opengl {
class context
{
public:
    using get_address_fun = void* (*)(const char*);

    context(native_handle window, std::vector<std::pair<context_attribs, int>> context_attributes = {},
            const context* const shared = nullptr);
    ~context();

    context(context&& other) noexcept = default;
    context(const context& other)     = delete;
    context& operator=(context&& other) noexcept = default;
    context& operator=(const context& other) = delete;

    void set_pixel_format(std::vector<std::pair<pixel_format_attribs, int>> attributes) const;

    void make_current() const noexcept;
    void clear_current() const noexcept;
    void swap_buffers() const noexcept;
    void set_swap_interval(int i) const noexcept;

   get_address_fun get_loader() const noexcept;

private:
    static void check_handle(native_handle hnd);

    int (*swapIntervalEXT)(int) = nullptr;
    get_address_fun get_proc_address = nullptr;
    bool          _is_window_owner;
    native_handle _own_window;
    native_handle _device_context;
    native_handle _context;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx