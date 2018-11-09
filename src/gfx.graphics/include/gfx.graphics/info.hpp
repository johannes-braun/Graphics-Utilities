#pragma once

#include <gfx.core/version.hpp>
#include <string_view>

namespace gfx {
inline namespace v1 {
namespace detail {
constexpr version_t        engine_version = version_t(1, 0, 0);
constexpr std::string_view engine_name    = "Graphics Utilities";
}    // namespace detail
}    // namespace v1
}    // namespace gfx