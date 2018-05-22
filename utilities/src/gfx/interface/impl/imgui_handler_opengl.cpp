#include "../imgui_handler.hpp"

namespace gfx
{
    constexpr const char* vertex_shader_source = R"(
layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec4 color;

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_color;

layout(binding=0) uniform Data{
    mat4 projection;
    sampler2D img;
};

out gl_PerVertex { vec4 gl_Position; };

void main() {
    out_uv = uv;
    out_color = color;
    
    gl_Position = projection * vec4(position.x, position.y, 1.f, 1);
})";

    constexpr const char* fragment_shader_source = R"(
layout(location=0) in vec2 uv;
layout(location=1) in vec4 color;

layout(location=0) out vec4 out_color;

layout(binding=0) uniform Data {
    mat4 projection;
    sampler2D img;
};

void main() {
    out_color = color * textureLod(img, uv, 0);
})";


    imgui_handler_opengl::imgui_handler_opengl(imgui& imgui)
        : imgui_handler(imgui), _vertex_buffer(GL_DYNAMIC_STORAGE_BIT), _index_buffer(GL_DYNAMIC_STORAGE_BIT), _render_data(GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
    {
        _graphics_pipeline[GL_VERTEX_SHADER]    = std::make_shared<gl::shader>(vertex_shader_source,    "gfx::imgui_handle_opengl::vertex_shader",   GL_VERTEX_SHADER);
        _graphics_pipeline[GL_FRAGMENT_SHADER]  = std::make_shared<gl::shader>(fragment_shader_source,  "gfx::imgui_handle_opengl::fragment_shader", GL_FRAGMENT_SHADER);

        unsigned char* pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        _fonts_atlas = std::make_unique<gl::texture>(GL_TEXTURE_2D, width, height, GL_RGBA8, 1);
        _fonts_atlas->assign(GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        _sampler = gl::sampler();
        _sampler.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        _sampler.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        _sampler.set(GL_TEXTURE_MAX_LOD, 1);
        ImGui::GetIO().Fonts->TexID = static_cast<uint64_t>(mygl::texture(*_fonts_atlas));

        _render_data.map(GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        _render_data.emplace_back();

        _vertex_array.attrib(0).enable(true).format(2, GL_FLOAT, false, offsetof(ImDrawVert, pos)).bind(0);
        _vertex_array.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(ImDrawVert, uv)).bind(0);
        _vertex_array.attrib(2).enable(true).format(4, GL_UNSIGNED_BYTE, true, offsetof(ImDrawVert, col)).bind(0);
    }

    void imgui_handler_opengl::upload(ImDrawData* data)
    {
        _vertex_buffer.clear();
        _index_buffer.clear();

        for (int list = 0; list < data->CmdListsCount; ++list)
        {
            const auto cmd_list = data->CmdLists[list];
            _vertex_buffer.insert(_vertex_buffer.end(), cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Data + cmd_list->VtxBuffer.Size);
            _index_buffer.insert(_index_buffer.end(), cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Data + cmd_list->IdxBuffer.Size);

            _vertex_array.vertex_buffer(0, _vertex_buffer);
            _vertex_array.element_buffer(_index_buffer);
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

        _render_data[0].projection = glm::mat4(1.f);
        _render_data[0].projection[0][0] = 2.0f / io.DisplaySize.x;
        _render_data[0].projection[1][1] = -2.0f / io.DisplaySize.y;
        _render_data[0].projection[2] = glm::vec4{ 0.0f, 0.0f, -1.0f, 0.0f };
        _render_data[0].projection[3] = glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f };
        _render_data[0].image = 0;

        _graphics_pipeline.bind();
        _render_data.bind(GL_UNIFORM_BUFFER, 0);
    }

    void imgui_handler_opengl::draw(const ImDrawCmd& cmd, const uint32_t index_offset, const uint32_t vertex_offset)
    {
        _vertex_array.bind();
        _render_data[0].image = _sampler.sample(mygl::texture(cmd.TextureId));
        glMemoryBarrier(GL_UNIFORM_BARRIER_BIT);

        glScissor(int(cmd.ClipRect.x), int(int(ImGui::GetIO().DisplaySize.y * ImGui::GetIO().DisplayFramebufferScale.y) - cmd.ClipRect.w),
            int(cmd.ClipRect.z - cmd.ClipRect.x), int(cmd.ClipRect.w - cmd.ClipRect.y));

        glDrawElementsBaseVertex(GL_TRIANGLES, cmd.ElemCount, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(index_offset * sizeof(ImDrawIdx)), vertex_offset);
        glFinish();
    }

    void imgui_handler_opengl::finalize()
    {
        glClipControl(GLenum(_last_clip_origin), GLenum(_last_clip_depth_mode));
        glUseProgram(mygl::shader_program(_last_program));
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