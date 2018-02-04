#include "gui_gl.hpp"

namespace io::impl
{
    gui_gl::gui_gl()
    {
        _graphics_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _graphics_pipeline->use_stages(
            jpu::make_ref<gl::shader>(gl::shader_root / "gui/gui.vert"),
            jpu::make_ref<gl::shader>(gl::shader_root / "gui/gui.frag")
        );
    }

    ImTextureID gui_gl::build_font_atlas()
    {
        unsigned char* pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        _fonts_atlas = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        _fonts_atlas->storage_2d(width, height, GL_RGBA8, 1);
        _fonts_atlas->assign_2d(GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        _sampler = jpu::make_ref<gl::sampler>();
        _sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        _sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return reinterpret_cast<ImTextureID>(static_cast<uint64_t>(_sampler->sample_texture(_fonts_atlas)));
    }

    void gui_gl::pre_render(ImDrawData* draw_data)
    {
        const auto vertex_count = draw_data->TotalVtxCount;
        const auto index_count = draw_data->TotalIdxCount;
        if (vertex_count > _last_vertex_buffer_size || index_count > _last_index_buffer_size)
        {
            _vertex_array = jpu::make_ref<gl::vertex_array>();
            _last_vertex_buffer_size = std::max<size_t>(1, _last_vertex_buffer_size);
            _last_index_buffer_size = std::max<size_t>(1, _last_index_buffer_size);

            while (_last_vertex_buffer_size < vertex_count) _last_vertex_buffer_size <<= 1;
            while (_last_index_buffer_size < index_count) _last_index_buffer_size <<= 1;

            _vertex_buffer = jpu::make_ref<gl::buffer>(_last_vertex_buffer_size * sizeof(ImDrawVert), gl::buffer_flag_bits::dynamic_storage);
            _index_buffer = jpu::make_ref<gl::buffer>(_last_index_buffer_size * sizeof(ImDrawIdx), gl::buffer_flag_bits::dynamic_storage);

            _vertex_array->add_bindings({
                gl::vertex_attribute_binding(0, *_vertex_buffer, 0, 2, GL_FLOAT, sizeof(ImDrawVert), offsetof(ImDrawVert, pos), false),
                gl::vertex_attribute_binding(1, *_vertex_buffer, 0, 2, GL_FLOAT, sizeof(ImDrawVert), offsetof(ImDrawVert, uv), false),
                gl::vertex_attribute_binding(2, *_vertex_buffer, 0, 4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), offsetof(ImDrawVert, col), true)
                });
            _vertex_array->set_element_buffer(*_index_buffer);
        }

        for (int list = 0, vtx = 0, idx = 0; list < draw_data->CmdListsCount;
            vtx += draw_data->CmdLists[list]->VtxBuffer.Size,
            idx += draw_data->CmdLists[list]->IdxBuffer.Size, ++list)
        {
            const auto cmd_list = draw_data->CmdLists[list];
            _vertex_buffer->assign(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size, vtx * sizeof(ImDrawVert));
            _index_buffer->assign(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size, idx * sizeof(ImDrawIdx));
        }

        auto&& io = ImGui::GetIO();
        const int fb_width = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
        const int fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        glGetIntegerv(GL_POLYGON_MODE, _last_polygon_mode);
        glGetIntegerv(GL_VIEWPORT, _last_viewport);
        glGetIntegerv(GL_SCISSOR_BOX, _last_scissor_box);
        const auto get_i = [](GLenum e) { int i; glGetIntegerv(e, &i); return i; };
        _last_program = get_i(GL_CURRENT_PROGRAM);
        _last_blend_src_rgb = get_i(GL_BLEND_SRC_RGB);
        _last_blend_dst_rgb = get_i(GL_BLEND_DST_RGB);
        _last_blend_src_alpha = get_i(GL_BLEND_SRC_ALPHA);
        _last_blend_dst_alpha = get_i(GL_BLEND_DST_ALPHA);
        _last_blend_equation_rgb = get_i(GL_BLEND_EQUATION_RGB);
        _last_blend_equation_alpha = get_i(GL_BLEND_EQUATION_ALPHA);
        _last_clip_depth_mode = get_i(GL_CLIP_DEPTH_MODE);
        _last_clip_origin = get_i(GL_CLIP_ORIGIN);
        _last_enable_blend = glIsEnabled(GL_BLEND);
        _last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        _last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        _last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);

        glViewport(0, 0, static_cast<GLsizei>(fb_width), static_cast<GLsizei>(fb_height));

        glm::mat4 ortho_projection = glm::mat4(1.f);
        ortho_projection[0][0] = 2.0f / io.DisplaySize.x;
        ortho_projection[1][1] = -2.0f / io.DisplaySize.y;
        ortho_projection[2] = glm::vec4{ 0.0f, 0.0f, -1.0f, 0.0f };
        ortho_projection[3] = glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f };
        _vertex_array->bind();
        _graphics_pipeline->bind();
        _graphics_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("projection") = ortho_projection;
    }

    void gui_gl::render(const ImDrawCmd& cmd, const int index_offset, const int vertex_offset) const
    {
        assert(GL_UNSIGNED_INT == GL_UNSIGNED_BYTE + 4 && GL_UNSIGNED_SHORT == GL_UNSIGNED_BYTE + 2);
        _graphics_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("img") = reinterpret_cast<uint64_t>(cmd.TextureId);
        glScissor(static_cast<int>(cmd.ClipRect.x), static_cast<int>(static_cast<int>(ImGui::GetIO().DisplaySize.y * ImGui::GetIO().DisplayFramebufferScale.y) - cmd.ClipRect.w),
            static_cast<int>(cmd.ClipRect.z - cmd.ClipRect.x), static_cast<int>(cmd.ClipRect.w - cmd.ClipRect.y));
        glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(cmd.ElemCount), GL_UNSIGNED_BYTE + sizeof(ImDrawIdx),
            reinterpret_cast<const void*>(index_offset * sizeof(ImDrawIdx)), static_cast<int>(vertex_offset));
    }

    void gui_gl::post_render()
    {
        glClipControl(_last_clip_origin, _last_clip_depth_mode);
        glUseProgram(_last_program);
        glBlendEquationSeparate(_last_blend_equation_rgb, _last_blend_equation_alpha);
        glBlendFuncSeparate(_last_blend_src_rgb, _last_blend_dst_rgb, _last_blend_src_alpha, _last_blend_dst_alpha);
        _last_enable_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        _last_enable_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        _last_enable_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        _last_enable_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, _last_polygon_mode[0]);
        glViewport(_last_viewport[0], _last_viewport[1], static_cast<GLsizei>(_last_viewport[2]), static_cast<GLsizei>(_last_viewport[3]));
        glScissor(_last_scissor_box[0], _last_scissor_box[1], static_cast<GLsizei>(_last_scissor_box[2]), static_cast<GLsizei>(_last_scissor_box[3]));
    }
}
