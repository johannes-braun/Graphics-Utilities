#pragma once

#include "font.hpp"

#include <glm/glm.hpp>
#include <opengl/buffer.hpp>
#include <opengl/texture.hpp>
#include <io/window.hpp>
#include <functional>
#include <memory>

namespace gfx::ui
{
    using draw_idx = uint16_t;
    using draw_cmd = std::function<void(const gl::pipeline& pipeline)>;
    struct draw_prop
    {
        glm::mat4 screen_matrix;
        uintptr_t texture;
        uint32_t has_texture;
    };
    struct draw_vtx
    {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::u8vec4 col;
    };

    enum text_align
    {
        ALIGN_LEFT      = 0x1<<0,
        ALIGN_CENTER    = 0x1<<1,
        ALIGN_RIGHT     = 0x1<<2,
        ALIGN_JUSTIFY   = 0x1<<3,
        ALIGN_DEFAULT   = ALIGN_LEFT,
    };

    enum gradient
    {
        GRADIENT_NONE = 0,
        GRADIENT_HORIZONTAL = 1,
        GRADIENT_VERTICAL = 2
    };

    enum corner
    {
        CORNER_NONE = 0,
        CORNER_TOP_LEFT = 1<<0,
        CORNER_TOP_RIGHT = 1<<1,
        CORNER_BOTTOM_LEFT = 1<<2,
        CORNER_BOTTOM_RIGHT = 1<<3,
        CORNER_ALL = 0b1111,
    };

    inline corner operator|(corner a, corner b) noexcept
    {
        return corner(int(a) | int(b));
    }

    class draw_list
    {
    public:
        draw_list(const std::shared_ptr<io::window>& window);
        void push_triangle(const draw_vtx& a, const draw_vtx& b, const draw_vtx& c, gl_texture_t texture = gl_texture_t::zero);
        void push_quad(const glm::vec2& min, const glm::vec2& max, const glm::vec2& min_uv, const glm::vec2& max_uv, const glm::u8vec4& color1, const glm::u8vec4& color2, gradient gradient, gl_texture_t texture = gl_texture_t::zero);
        void push_scissor(int x, int y, int width, int height);
        glm::vec2 push_text(const std::wstring& text, const font& font, float y_offset, glm::vec2 bmin, glm::vec2 bmax, text_align align, glm::u8vec4 color ={ 255, 255, 255, 255 }, int max_lines = std::numeric_limits<int>::max());
        void push_rounding(const glm::vec2& center, float radius, float begin, float end, int subdivision = 3, glm::u8vec4 color_center ={ 255, 255, 255, 255 }, glm::u8vec4 color_outer ={ 255, 255, 255, 255 });
        void push_rounded_quad(glm::vec2 min, glm::vec2 max, const glm::vec2& min_uv, const glm::vec2& max_uv, const glm::u8vec4& color1, const glm::u8vec4& color2, float radius, corner corners, gl_texture_t texture = gl_texture_t::zero);

        void draw(const gl::pipeline& pipeline);

        void push_vertices(std::initializer_list<draw_vtx> vertices);
        void push_indices(std::initializer_list<draw_idx> indices);
        void push_draw_command(int count, int base_index, int base_vertex, gl_texture_t texture = gl_texture_t::zero);
        void push_command(draw_cmd command);

        size_t index_count() const noexcept;
        size_t vertex_count() const noexcept;

    private:
        std::shared_ptr<io::window> _window;

        std::vector<draw_cmd> _commands;
        std::vector<draw_idx> _indices;
        std::vector<draw_vtx> _vertices;

        gl::sampler _sampler;
        gl::buffer<draw_vtx> _vertex_buffer;
        gl::buffer<draw_idx> _index_buffer;
        gl::buffer<draw_prop> _prop_buffer;
    };
}