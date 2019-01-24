#pragma once
#include <type_traits>

namespace gfx {
inline namespace v1 {
class native_handle
{
public:
    template<typename T, typename = std::enable_if_t<!std::is_same_v<T, nullptr_t> && (sizeof(T) <= sizeof(void*))>>
    native_handle(T hnd) : _hnd(reinterpret_cast<void*>(uint64_t(hnd)))
    {}
    native_handle(nullptr_t) : _hnd(nullptr) {}
    native_handle() = default;

    bool operator==(const native_handle& o) const noexcept { return o._hnd == _hnd; }
    bool operator!=(const native_handle& o) const noexcept { return o._hnd != _hnd; }

private:
    void* _hnd = nullptr;
};

}    // namespace v1
}    // namespace gfx
