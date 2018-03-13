#pragma once

#include <opengl/vertex_array.hpp>
#include <opengl/buffer.hpp>
#include <opengl/texture.hpp>
#include <opengl/pipeline.hpp>
#include <imgui/imgui.h>

namespace io::impl
{
    class gui_gl
    {
    public:
        explicit gui_gl(bool init);
        ImTextureID build_font_atlas();
        void pre_render(ImDrawData* draw_data);
        void render(const ImDrawCmd& cmd, int index_offset, int vertex_offset) const;
        void post_render();

    private:
        jpu::ref_ptr<gl::graphics_pipeline> _graphics_pipeline;
        gl::buffer<ImDrawVert> _vertex_buffer = gl::buffer<ImDrawVert>(GL_DYNAMIC_STORAGE_BIT);
        gl::buffer<ImDrawIdx> _index_buffer = gl::buffer<ImDrawIdx>(GL_DYNAMIC_STORAGE_BIT);
        std::unique_ptr<gl::texture> _fonts_atlas;
        gl::sampler _sampler;

        size_t _last_vertex_buffer_size = 0;
        size_t _last_index_buffer_size = 0;

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
}
