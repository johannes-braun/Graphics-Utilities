#pragma once

#include <gfx.core/types.hpp>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
template<typename T>
using cref_array_view = vk::ArrayProxy<std::reference_wrapper<T const> const>;
template<typename T>
using opt_ref = std::optional<std::reference_wrapper<T>>;
}    // namespace v1
}    // namespace gfx