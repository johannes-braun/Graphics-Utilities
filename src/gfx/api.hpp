#pragma once

#include <type_traits>
#include <any>

#if defined(GFX_EXPOSE_APIS)
#include <mygl/mygl.hpp>
#endif

namespace gfx
{
enum class gapi
{
    opengl = 0,
    vulkan
};

using handle = std::any;

template<typename T, typename X, typename = decltype(std::declval<X>().api_handle())>
T handle_cast(X&& x)
{
	return std::any_cast<T>(std::forward<X&&>(x).api_handle());
}
}