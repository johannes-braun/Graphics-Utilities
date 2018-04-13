#include "../impl/gui_gl.hpp"
#include <glm/glm.hpp>

namespace io::impl
{
    gui_gl::gui_gl(const bool init)
    {
        if (!init)
            return;

        _graphics_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("gui/gui.vert");
        _graphics_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("gui/gui.frag");
    }

    ImTextureID gui_gl::build_font_atlas()
    {
        unsigned char* pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        _fonts_atlas = std::make_unique<gl::texture>(GL_TEXTURE_2D, width, height, GL_RGBA8, 1);
        _fonts_atlas->assign(GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        _sampler = gl::sampler();
        _sampler.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        _sampler.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return reinterpret_cast<ImTextureID>(static_cast<uint64_t>(_sampler.sample(*_fonts_atlas)));
    }

    void gui_gl::pre_render(ImDrawData* draw_data)
    {
        _vertex_buffer.clear();
        _index_buffer.clear();

        for (int list = 0; list < draw_data->CmdListsCount; ++list)
        {
            const auto cmd_list = draw_data->CmdLists[list];
            _vertex_buffer.insert(_vertex_buffer.end(), cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Data + cmd_list->VtxBuffer.Size);
            _index_buffer.insert(_index_buffer.end(), cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Data + cmd_list->IdxBuffer.Size);
        }

        auto&& io = ImGui::GetIO();
        const float fb_width = io.DisplaySize.x * io.DisplayFramebufferScale.x;
        const float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
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

        glViewportIndexedf(0, 0, 0, fb_width, fb_height);

        glm::mat4 ortho_projection = glm::mat4(1.f);
        ortho_projection[0][0] = 2.0f / io.DisplaySize.x;
        ortho_projection[1][1] = -2.0f / io.DisplaySize.y;
        ortho_projection[2] = glm::vec4{ 0.0f, 0.0f, -1.0f, 0.0f };
        ortho_projection[3] = glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f };

        _graphics_pipeline.bind();
        _graphics_pipeline.bind_attribute(0, _vertex_buffer, 2, GL_FLOAT, offsetof(ImDrawVert, pos));
        _graphics_pipeline.bind_attribute(1, _vertex_buffer, 2, GL_FLOAT, offsetof(ImDrawVert, uv));
        _graphics_pipeline.bind_attribute(2, _vertex_buffer, 4, GL_UNSIGNED_BYTE, true, offsetof(ImDrawVert, col));

        _graphics_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("projection") = ortho_projection;
    }

    void gui_gl::render(const ImDrawCmd& cmd, const int index_offset, const int vertex_offset) const
    {
        assert(GL_UNSIGNED_INT == GL_UNSIGNED_BYTE + 4 && GL_UNSIGNED_SHORT == GL_UNSIGNED_BYTE + 2);

        _graphics_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("img") = reinterpret_cast<uint64_t>(cmd.TextureId);

        glScissor(static_cast<int>(cmd.ClipRect.x), 
            static_cast<int>(static_cast<int>(ImGui::GetIO().DisplaySize.y * 
                ImGui::GetIO().DisplayFramebufferScale.y) - cmd.ClipRect.w),
            static_cast<int>(cmd.ClipRect.z - cmd.ClipRect.x), 
            static_cast<int>(cmd.ClipRect.w - cmd.ClipRect.y));

        _graphics_pipeline.draw(GL_TRIANGLES, _index_buffer, GLenum(GL_UNSIGNED_BYTE + sizeof(ImDrawIdx)), cmd.ElemCount, index_offset, vertex_offset);
    }

    void gui_gl::post_render()
    {
        glClipControl(GLenum(_last_clip_origin), GLenum(_last_clip_depth_mode));
        glUseProgram(gl_shader_program_t(_last_program));
        glBlendEquationSeparate(GLenum(_last_blend_equation_rgb), GLenum(_last_blend_equation_alpha));
        glBlendFuncSeparate(GLenum(_last_blend_src_rgb), GLenum(_last_blend_dst_rgb), GLenum(_last_blend_src_alpha), GLenum(_last_blend_dst_alpha));
        _last_enable_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        _last_enable_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        _last_enable_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        _last_enable_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GLenum(_last_polygon_mode[0]));
        glViewportIndexedf(0, float(_last_viewport[0]), float(_last_viewport[1]), float(_last_viewport[2]), float(_last_viewport[3]));
        glScissor(_last_scissor_box[0], _last_scissor_box[1], _last_scissor_box[2], _last_scissor_box[3]);
    }
}
