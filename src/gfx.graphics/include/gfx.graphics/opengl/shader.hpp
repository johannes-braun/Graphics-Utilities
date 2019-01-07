#pragma once

#include <filesystem>
#include <gfx.core/types.hpp>
#include <gsl/span>
#include <optional>
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class shader
{
public:
    explicit shader(gsl::span<u32 const> source);

    shader(shader const& other) = delete;
    shader& operator=(shader const& other) = delete;
    shader(shader&& other)                 = default;
    shader& operator=(shader&& other) = default;
    ~shader();

    [[nodiscard]] auto data() const noexcept -> std::vector<std::byte> const&;
    [[nodiscard]] auto get_module() const noexcept -> mygl::shader const&;

private:
    void load(gsl::span<u32 const> source);

    mygl::shader _module;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx