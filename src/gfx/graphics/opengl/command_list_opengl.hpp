#pragma once

#include "../command_list.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class commands_implementation : public detail::commands_implementation
{
public:
	using cmd = std::function<void()>;

    // Geerbt �ber commands_implementation
    void initialize(commands_type type) override;

    void reset() override;
    void begin() override;
    void end() override;

    void execute_sync_after(const commands& cmd, fence* f) override;
    void execute(fence* f) override;

    void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings) override;
    void dispatch(u32 x, u32 y, u32 z) override;

    void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area) override;
    void end_pass() override;

    void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings) override;
    void draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance) override;

private:
	GLenum current_draw_mode() const;

	bool _has_state = false;
	const framebuffer*           _curr_framebuffer  = nullptr;
	const graphics_pipeline*     _curr_pipeline     = nullptr;
	std::atomic<GLenum>    _element_type      = GLenum(0);
	std::atomic<size_t>    _element_type_size = 1;
	std::atomic<ptrdiff_t> _element_offset;
	std::vector<cmd>       _queue;
};
}    // namespace vulkan
}    // namespace v2
}    // namespace gfx