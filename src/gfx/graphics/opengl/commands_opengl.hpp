#pragma once
#include <mygl/mygl.hpp>
#include "../commands.hpp"

namespace gfx::opengl
{
    class commands_implementation : public detail::commands_implementation
    {
    public:
        using cmd = std::function<void()>;
        void bind_graphics_pipeline(graphics_pipeline& pipeline) override;
        void bind_vertex_buffer(uint32_t binding, std::any buffer_handle, ptrdiff_t offset, uint32_t stride) override;
        void bind_index_buffer(std::any buffer_handle, index_type type, ptrdiff_t offset) override;
        void draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) override;
        void draw_indexed(size_t index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
            ptrdiff_t first_instance) override;
        void reset() override;
        void execute(bool block) override;
        void bind_descriptors(descriptor_set* sets, int count) override;
        void begin_pass(framebuffer& fbo_handle) override;
		void end_pass() override;
        void set_viewports(gfx::viewport* vps, int count, int first) override;
        std::any api_handle() override;

    private:
        GLenum                  current_draw_mode() const;

		framebuffer* _curr_framebuffer = nullptr;
        graphics_pipeline* _curr_pipeline = nullptr;
        std::atomic<GLenum>     _element_type  = GLenum(0);
        std::atomic<size_t>     _element_type_size  = 1;
        std::atomic<ptrdiff_t>  _element_offset;
        std::vector<cmd>        _queue;
    };
}