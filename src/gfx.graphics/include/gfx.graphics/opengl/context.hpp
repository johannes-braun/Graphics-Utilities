#pragma once

#include "enums.hpp"

#include "native_handle.hpp"
#include <cinttypes>
#include <vector>

namespace gfx {
inline namespace v1 {
class native_handle;
namespace opengl {
struct context_native_t;

class context
{
public:
    using get_address_fun = void* (*)(const char*);

    static context const* current();
    static void           clear_current() noexcept;
    static void                  make_current(context const* c) noexcept;

    context(native_handle window, std::vector<std::pair<context_attribs, int>> context_attributes = {},
            std::vector<std::pair<pixel_format_attribs, int>> attributes = {},
            const context* const shared = nullptr);
    ~context();

    context(context&& other) noexcept = default;
    context(const context& other)     = delete;
    context& operator=(context&& other) noexcept = default;
    context& operator=(const context& other) = delete;

    void make_current() const noexcept;
    void swap_buffers() const noexcept;
    void set_swap_interval(int i) const noexcept;

   get_address_fun get_loader() const noexcept;

private:
   void set_pixel_format(std::vector<std::pair<pixel_format_attribs, int>> attributes) const;
    static void check_handle(native_handle hnd);

    get_address_fun get_proc_address = nullptr;
    bool          _is_window_owner;
    native_handle _own_window;
    native_handle _device_context;
    native_handle _context;

    std::unique_ptr<context_native_t> _native;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx