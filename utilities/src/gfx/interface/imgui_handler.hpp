#pragma once

#include <imgui/imgui.h>

// OpenGL
#include <opengl/buffer.hpp>
#include <opengl/texture.hpp>
#include <opengl/pipeline.hpp>

namespace gfx
{
    class imgui;
    struct imgui_handler
    {
        friend imgui;
        imgui_handler(imgui& imgui) : _imgui(imgui) {}
        virtual ~imgui_handler() = default;

    protected:
        virtual void upload(ImDrawData* data) = 0;
        virtual void draw(const ImDrawCmd& cmd, uint32_t index_offset, uint32_t vertex_offset) = 0;
        virtual void finalize() = 0;

        imgui& get_imgui() const noexcept { return _imgui; }
    private:
        imgui& _imgui;
    };

    struct imgui_handler_opengl : imgui_handler
    {
        imgui_handler_opengl(imgui& imgui);

    protected:
        void upload(ImDrawData* data) override;
        void draw(const ImDrawCmd& cmd, uint32_t index_offset, uint32_t vertex_offset) override;
        void finalize() override;

    private:
        struct render_data
        {
            glm::mat4 projection;
            uint64_t image;
        };

        gl::pipeline _graphics_pipeline;
        gl::buffer<ImDrawVert> _vertex_buffer;
        gl::buffer<ImDrawIdx> _index_buffer;
        gl::buffer<render_data> _render_data;
        std::unique_ptr<gl::texture> _fonts_atlas;
        gl::sampler _sampler;
        int _last_program{ 0 };
        int _last_blend_src_rgb{ 0 };
        int _last_blend_dst_rgb{ 0 };
        int _last_blend_src_alpha{ 0 };
        int _last_blend_dst_alpha{ 0 };
        int _last_blend_equation_rgb{ 0 };
        int _last_blend_equation_alpha{ 0 };
        int _last_clip_depth_mode{ 0 };
        int _last_clip_origin{ 0 };
        int _last_enable_blend{ 0 };
        int _last_enable_cull_face{ 0 };
        int _last_enable_depth_test{ 0 };
        int _last_enable_scissor_test{ 0 };
        int _last_polygon_mode[2];
        int _last_viewport[4];
        int _last_scissor_box[4];
    };

    struct imgui_handler_vulkan : imgui_handler
    {
        imgui_handler_vulkan(imgui& imgui);

    protected:
        void upload(ImDrawData* data) override;
        void draw(const ImDrawCmd& cmd, uint32_t index_offset, uint32_t vertex_offset) override;
        void finalize() override;
    };
}
