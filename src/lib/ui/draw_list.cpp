#include "draw_list.hpp"

namespace gfx::ui
{
    draw_list::draw_list(const std::shared_ptr<io::window>& window)
        : _window(window), _prop_buffer(1, GL_DYNAMIC_STORAGE_BIT),
        _index_buffer(GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT),
        _vertex_buffer(GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT)
    {
        _index_buffer.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        _vertex_buffer.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        _sampler.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        _sampler.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        _sampler.set(GL_TEXTURE_MAX_LOD, 0);
    }

    void draw_list::push_scissor(int x, int y, int width, int height)
    {
        _commands.emplace_back([=](const gl::pipeline& pipeline) {
            glScissor(x, y, width, height);
        });
    }

    void draw_list::push_triangle(const draw_vtx& a, const draw_vtx& b, const draw_vtx& c, gl_texture_t texture)
    {
        push_vertices({ a, b, c });
        _commands.emplace_back([=, first = _vertices.size() - 3](const gl::pipeline& pipeline) {
            _prop_buffer[0].has_texture = texture != gl_texture_t::zero;
            _prop_buffer[0].texture = texture != gl_texture_t::zero ? _sampler.sample(texture) : 0;
            _prop_buffer.synchronize();
            pipeline.draw(GL_TRIANGLES, 3, first);
        });
    }

    void draw_list::push_quad(const glm::vec2& min, const glm::vec2& max, const glm::vec2& min_uv, const glm::vec2& max_uv, const glm::u8vec4& color1, const glm::u8vec4& color2, gradient gradient, gl_texture_t texture)
    {
        switch (gradient)
        {
        case GRADIENT_NONE:
        {
            push_vertices({
                draw_vtx{ { min.x, max.y },{ min_uv.x, max_uv.y }, color1 },
                draw_vtx{ { min.x, min.y },{ min_uv.x, min_uv.y }, color1 },
                draw_vtx{ { max.x, min.y },{ max_uv.x, min_uv.y }, color1 },
                draw_vtx{ { max.x, max.y },{ max_uv.x, max_uv.y }, color1 }
            });
        }
        break;
        case GRADIENT_HORIZONTAL:
        {
            push_vertices({
                draw_vtx{ { min.x, max.y },{ min_uv.x, max_uv.y }, color1 },
                draw_vtx{ { min.x, min.y },{ min_uv.x, min_uv.y }, color1 },
                draw_vtx{ { max.x, min.y },{ max_uv.x, min_uv.y }, color2 },
                draw_vtx{ { max.x, max.y },{ max_uv.x, max_uv.y }, color2 }
            });
        }
        break;
        case GRADIENT_VERTICAL:
        {
            push_vertices({
                draw_vtx{ { min.x, max.y },{ min_uv.x, max_uv.y }, color2 },
                draw_vtx{ { min.x, min.y },{ min_uv.x, min_uv.y }, color1 },
                draw_vtx{ { max.x, min.y },{ max_uv.x, min_uv.y }, color1 },
                draw_vtx{ { max.x, max.y },{ max_uv.x, max_uv.y }, color2 }
            });
        }
        break;
        }

        push_indices({0, 1, 2, 0, 2, 3});
        _commands.emplace_back([=, first = _vertices.size() - 4, base_idx = _indices.size() - 6](const gl::pipeline& pipeline) {
            _prop_buffer[0].has_texture = texture != gl_texture_t::zero;
            _prop_buffer[0].texture = texture != gl_texture_t::zero ? _sampler.sample(texture) : 0;
            _prop_buffer.synchronize();
            pipeline.draw(GL_TRIANGLES, _index_buffer, GL_UNSIGNED_SHORT, 6, base_idx, first);
        });
    }

    void draw_list::push_rounded_quad(glm::vec2 min, glm::vec2 max, const glm::vec2& min_uv, const glm::vec2& max_uv, const glm::u8vec4& color1, const glm::u8vec4& color2, float radius, corner corners, gl_texture_t texture)
    {
        const float rounding = radius;
        bool top_left = corners & CORNER_TOP_LEFT;
        bool top_right = corners & CORNER_TOP_RIGHT;
        bool bottom_right = corners & CORNER_BOTTOM_RIGHT;
        bool bottom_left = corners & CORNER_BOTTOM_LEFT;

        min += radius;
        max -= radius;

        if (bottom_left)    push_rounding({ min.x, min.y }, rounding, 1.f, 1.5f, 5, color1, color2);
        if (bottom_right)   push_rounding({ max.x, min.y }, rounding, 0.5f, 1.0f, 5, color1, color2);
        if (top_left)       push_rounding({ min.x, max.y }, rounding, 1.5f, 2.0f, 5, color1, color2);
        if (top_right)      push_rounding({ max.x, max.y }, rounding, 0.0f, 0.5f, 5, color1, color2);

        // left
        if (bottom_left && top_left) push_quad({ min.x-rounding, min.y }, { min.x, max.y }, { 0, 0 }, { 1, 1 }, color2, color1, gfx::ui::GRADIENT_HORIZONTAL);
        // top
        if (top_left && top_right) push_quad({ min.x, max.y }, { max.x, max.y+rounding }, { 0, 0 }, { 1, 1 }, color1, color2, gfx::ui::GRADIENT_VERTICAL);
        //right
        if (bottom_right && top_right) push_quad({ max.x, min.y }, { max.x+rounding, max.y }, { 0, 0 }, { 1, 1 }, color1, color2, gfx::ui::GRADIENT_HORIZONTAL);
        //bottom
        if (bottom_left && bottom_right) push_quad({ min.x, min.y-rounding }, { max.x, min.y }, { 0, 0 }, { 1, 1 }, color2, color1, gfx::ui::GRADIENT_VERTICAL);

        push_quad({ min.x - ((!(bottom_left && top_left)) * rounding), min.y - ((!(bottom_left && bottom_right)) * rounding) },
            { max.x + ((!(bottom_right && top_right)) * rounding), max.y + ((!(top_left && top_right)) * rounding) },
            { 0, 0 }, { 1, 1 }, color1, color2, gfx::ui::GRADIENT_NONE);
    }

    glm::vec2 draw_list::push_text(const std::wstring& text, const font& font, float y_offset, glm::vec2 bmin, glm::vec2 bmax, text_align align, glm::u8vec4 color, int max_lines)
    {
        const int first_vertex  = _vertices.size();
        const int first_index   = _indices.size();
        const gl_texture_t atlas = font.get_atlas();

        float offset_factor{ 0 };
        bool justify_base = false;
        bool justify = false;

        if (ALIGN_RIGHT & align)
            offset_factor = 1;
        else if (ALIGN_CENTER & align)
            offset_factor = 0.5f;
        if (ALIGN_JUSTIFY & align)
        {
            justify_base = true;
        }

        glm::vec2 base_pos{ bmin.x, bmax.y };
        float adv = 0;
        float height = -font.size();
        float width = 0;
        float advy = y_offset -font.size() + 0.2f*font.size();
        float full_width = bmax.x - bmin.x;
        auto lwp = font.line_width(text.data(), full_width);
        float max_width = lwp.width - (lwp.delim != '\0' ? 0.01f : 0);
        justify = justify_base && (lwp.delim != '\0' && lwp.delim != '\n');
        float scale_x = justify ? (full_width / max_width) : 1;
        max_width = justify ? full_width : max_width;
        float offset_x = offset_factor * (full_width - max_width);
        uint16_t id = 0;
        for (const auto& c : text)
        {
            glyph g = font[std::clamp<wchar_t>(c, L' ', 512)];
            if (adv > max_width)
            {
                if (--max_lines <=0)
                    break;
                const wchar_t* p = &c + 1;
                adv = 0;
                advy -= font.size();
                height -= font.size();
                auto lwp = font.line_width(p, full_width);
                max_width = lwp.width - (lwp.delim != '\0' ? 0.01f : 0);
                justify = justify_base && (lwp.delim != '\0' && lwp.delim != '\n');
                scale_x = justify ? (full_width / max_width) : 1;
                max_width =  justify ? full_width : max_width;
                offset_x = offset_factor * (full_width - max_width);
                continue;
            }

            if (-advy > -3.5f*font.size() && -advy < bmax.y-bmin.y+3.5f*font.size())
            {
                glm::vec2 min{ base_pos.x + g.x0 + adv, base_pos.y + g.y0 + advy };
                glm::vec2 max{ base_pos.x+g.x1 + adv, base_pos.y+g.y1 +advy };
                glm::vec2 uvmin{ g.s0, g.t0 };
                glm::vec2 uvmax{ g.s1, g.t1 };
                push_vertices({
                    draw_vtx{ { min.x + offset_x, max.y },{ uvmin.x, uvmax.y }, color },
                    draw_vtx{ { min.x + offset_x, min.y },{ uvmin.x, uvmin.y }, color },
                    draw_vtx{ { max.x + offset_x, min.y },{ uvmax.x, uvmin.y }, color },
                    draw_vtx{ { max.x + offset_x, max.y },{ uvmax.x, uvmax.y }, color }
                });
                push_indices({ id, uint16_t(id+1), uint16_t(id+2), uint16_t(id+0), uint16_t(id+2), uint16_t(id+3) });
                id += 4;
            }
            adv += g.offx * scale_x;
            width = std::max(width, adv);
        }

        _commands.emplace_back([=, count = (id/4) * 6](const gl::pipeline& pipeline) {
            _prop_buffer[0].has_texture = true;
            _prop_buffer[0].texture = _sampler.sample(atlas);
            _prop_buffer.synchronize();
            pipeline.draw(GL_TRIANGLES, _index_buffer, GL_UNSIGNED_SHORT, count, first_index, first_vertex);
        });
        return { width, -height };
    }

    void draw_list::push_rounding(const glm::vec2& center, float radius, float begin, float end, int subdivision, glm::u8vec4 color_center, glm::u8vec4 color_outer)
    {
        int base_index  = index_count();
        int base_vertex = vertex_count();
        push_vertices({ gfx::ui::draw_vtx{ center,{ 0, 0 },color_center } });
        for (int i=0; i<subdivision+1; ++i)
        {
            float progress = begin * glm::pi<float>() + (end-begin) * glm::pi<float>() * i * (1/float(subdivision-1));
            const glm::vec2 pos{ glm::sin(progress), glm::cos(progress) };
            push_vertices({ gfx::ui::draw_vtx{ center + radius*pos,{ 0, 0 },color_outer } });
            if (i != 0)
            {
                push_indices({ uint16_t(0), uint16_t(i-1), uint16_t(i) });
            }
        }
        push_draw_command(index_count() - base_index, base_index, base_vertex);
    }

    void draw_list::draw(const gl::pipeline& pipeline)
    {
        int w, h; glfwGetFramebufferSize(*_window, &w, &h);
        _prop_buffer[0].screen_matrix = glm::scale(glm::translate(glm::vec3(-1, -1, 0)), glm::vec3(2.f / w, 2.f / h, 1));

        _vertex_buffer.clear();
        _vertex_buffer.insert(_vertex_buffer.begin(), _vertices.begin(), _vertices.end());
        _index_buffer.clear();
        _index_buffer.insert(_index_buffer.begin(), _indices.begin(), _indices.end());

        pipeline.bind_attribute(0, _vertex_buffer, 2, GL_FLOAT, offsetof(draw_vtx, pos));
        pipeline.bind_attribute(1, _vertex_buffer, 2, GL_FLOAT, offsetof(draw_vtx, uv));
        pipeline.bind_attribute(2, _vertex_buffer, 4, GL_UNSIGNED_BYTE, true, offsetof(draw_vtx, col));
        pipeline.bind_uniform_buffer(0, _prop_buffer);

        glEnable(GL_BLEND);
        glEnable(GL_SCISSOR_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& command : _commands)
            command(pipeline);

        glScissor(0, 0, w, h);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_BLEND);
        _vertex_buffer.clear();
        _index_buffer.clear();
        _vertices.clear();
        _indices.clear();
        _commands.clear();
    }

    void draw_list::push_draw_command(int count, int base_index, int base_vertex, gl_texture_t texture)
    {
        _commands.emplace_back([=](const gl::pipeline& pipeline) {
            _prop_buffer[0].has_texture = texture != gl_texture_t::zero;
            _prop_buffer[0].texture = texture != gl_texture_t::zero ? _sampler.sample(texture) : 0;
            _prop_buffer.synchronize();
            pipeline.draw(GL_TRIANGLES, _index_buffer, GL_UNSIGNED_SHORT, count, base_index, base_vertex);
        });
    }

    void draw_list::push_vertices(std::initializer_list<draw_vtx> vertices)
    {
        _vertices.insert(_vertices.end(), vertices.begin(), vertices.end());
    }

    void draw_list::push_indices(std::initializer_list<draw_idx> indices)
    {
        _indices.insert(_indices.end(), indices.begin(), indices.end());
    }

    void draw_list::push_command(draw_cmd command)
    {
        _commands.emplace_back(command);
    }

    size_t draw_list::index_count() const noexcept
    {
        return _indices.size();
    }

    size_t draw_list::vertex_count() const noexcept
    {
        return _vertices.size();
    }
}