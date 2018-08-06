#pragma once

#include "image_view.hpp"
#include <gfx/api.hpp>
#include <optional>

#include "graphics_pipeline.hpp"

namespace gfx {
inline namespace v1 {
enum class attachment
{
    color,
    resolve,
    depth_stencil
};
struct depth_stencil
{
    depth_stencil(float d = 0.f, u32 s = 0);

    float    depth;
    u32 stencil;
};
using clear_value = std::variant<glm::vec4, depth_stencil>;

namespace detail {
class framebuffer_implementation
{
public:
    static std::unique_ptr<framebuffer_implementation> make();
    virtual ~framebuffer_implementation()                                                                        = default;
    virtual void create(u32 width, u32 height, u32 layers, const v2::renderpass_layout& layout)                                                                   = 0;
    virtual void attach(attachment att, u32 index, const image_view& img_view, std::optional<clear_value> clear) = 0;
    virtual void detach(attachment att, u32 index)                                                               = 0;
	virtual handle api_handle()                                                                                  = 0;
};
}    // namespace detail


class commands;
class framebuffer : public impl::implements<detail::framebuffer_implementation>
{
    friend class commands;
public:
    framebuffer(u32 width, u32 height, u32 layers, const v2::renderpass_layout& layout);

    framebuffer(framebuffer&& fbo) = default;
    framebuffer& operator=(framebuffer&& fbo) = default;

    void attach(attachment att, u32 index, const image_view& img_view, std::optional<clear_value> clear = {});
    void detach(attachment att, u32 index);

    const std::vector<std::optional<clear_value>>& color_clear_values() const noexcept;;
    const std::optional<clear_value>& depth_clear_value() const noexcept;;

private:
    u32                                _width;
    u32                                _height;
	u32 _layers;
    std::vector<std::optional<clear_value>> _color_clear_values;
    std::optional<clear_value>              _depth_clear_value;
};
}    // namespace v1
}    // namespace gfx