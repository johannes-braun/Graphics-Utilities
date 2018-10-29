#pragma once

#include <api.hpp>

namespace gfx {
inline namespace v1 {
template<typename T>
struct movable_handle
{
    constexpr static T null_handle = T(0);

	movable_handle() noexcept : _hnd(null_handle) {}
	movable_handle(T h) noexcept : _hnd(h) {}
	movable_handle(const movable_handle&) noexcept = default;
	movable_handle& operator=(const movable_handle&) noexcept = default;
    ~movable_handle()                            = default;

	movable_handle(movable_handle&& o) noexcept
    {
        _hnd   = o._hnd;
        o._hnd = null_handle;
    }
	movable_handle& operator=(movable_handle&& o) noexcept
    {
        _hnd   = o._hnd;
        o._hnd = null_handle;
        return *this;
    }

    operator T&() noexcept { return _hnd; }
    operator const T&() const noexcept { return _hnd; }

	T& get() noexcept { return _hnd; }
	const T& get() const noexcept { return _hnd; }

    T*       operator&() noexcept { return &_hnd; }
    const T* operator&() const noexcept { return &_hnd; }

    operator handle() const noexcept { return _hnd; }

private:
    T _hnd;
};
}    // namespace v1
}    // namespace gfx