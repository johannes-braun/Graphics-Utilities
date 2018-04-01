#pragma once

#include <GLFW/glfw3.h>
#include <opengl/gl.hpp>
#include <opengl/buffer.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/texture.hpp>
#include <glm/glm.hpp>
#include <fstream>

#include "stb_rect_pack.h"
#include "stb_truetype.h"

namespace game
{

    namespace files = std::experimental::filesystem;
    struct glyph
    {
        float x0, y0, s0, t0; // top-left
        float x1, y1, s1, t1; // bottom-right
        float offx = 0, offy = 0;
    };
    struct font
    {
        constexpr static wchar_t first_char = L' ';
        font(const files::path& ttf, float size, int atlas_dimension = 1024)
            : _size(size), _atlas_dimension(atlas_dimension)
        {
            std::ifstream font_file(ttf, std::ios::binary);
            font_file.seekg(0, std::ios::end);
            size_t len = font_file.tellg();
            font_file.seekg(0, std::ios::beg);
            std::vector<uint8_t> data(len);
            _characters.resize(512);
            font_file.read(reinterpret_cast<char*>(data.data()), len);

            std::vector<uint8_t> tex_data(atlas_dimension * atlas_dimension);

            stbtt_pack_context pctx;
            if (!stbtt_PackBegin(&pctx, tex_data.data(), atlas_dimension, atlas_dimension, 0, 1, nullptr))
                throw "up";
            stbtt_PackSetOversampling(&pctx, 2, 2);
            if (!stbtt_PackFontRange(&pctx, data.data(), 0, size, first_char, 512, _characters.data()))
                throw "up";
            stbtt_PackEnd(&pctx);

            std::vector<glm::u8vec4> rgba(_atlas_dimension * _atlas_dimension);
            for (int i=0; i<rgba.size(); ++i)
                rgba[i] = glm::u8vec4{ 255, 255, 255, tex_data[i] };
            atlas = gl::texture(GL_TEXTURE_2D, atlas_dimension, atlas_dimension, GL_RGBA8, 1);
            atlas.assign(GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
        }

        glyph& at(wchar_t c) {
            glyph g;
            stbtt_GetPackedQuad(_characters.data(), _atlas_dimension, _atlas_dimension, c - first_char, &g.offx, &g.offy, reinterpret_cast<stbtt_aligned_quad*>(&g), 1);
            g.y1 = -g.y1;
            g.y0 = -g.y0;
            return g;
        }
        const glyph& at(wchar_t c) const { return const_cast<font*>(this)->at(c); }
        glyph& operator[](wchar_t c) { return at(c); }
        const glyph& operator[](wchar_t c) const { return at(c); }

        //glm::vec2 text_size(const std::wstring& text);

        float size() const noexcept
        {
            return _size;
        }

        gl::texture atlas{ GL_TEXTURE_2D };

    private:
        float _size;
        int _atlas_dimension;
        std::vector<stbtt_packedchar> _characters;
    };

    class ui
    {
    public:
        struct vtx
        {
            glm::vec2 pos;
            glm::vec2 uv;
            glm::u8vec4 col;
        };
        using idx = uint16_t;
        struct prop
        {
            glm::mat4 screen;
            uintptr_t tex;
            uint32_t has_texture;
        };
        struct indirect
        {
            gl_texture_t texture;
            uint32_t count;
            uint32_t base_index;
            uint32_t base_vertex;
        };

        ui(GLFWwindow* w = nullptr)
            : _pbuf(1, GL_DYNAMIC_STORAGE_BIT),
            _ibuf(GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT),
            _vbuf(GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT),
            _win(w ? w : glfwGetCurrentContext())
        {
            _pip[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("ui/vs.vert");
            _pip[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("ui/fs.frag");

            _ibuf.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
            _vbuf.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        }

        void draw()
        {
            int w, h; glfwGetFramebufferSize(_win, &w, &h);

            _pbuf[0].screen = glm::scale(glm::translate(glm::vec3(-1, -1, 0)), glm::vec3(2.f / w, 2.f / h, 1));

            _pip.bind();
            _pip.bind_attribute(0, _vbuf, 2, GL_FLOAT, offsetof(vtx, pos));
            _pip.bind_attribute(1, _vbuf, 2, GL_FLOAT, offsetof(vtx, uv));
            _pip.bind_attribute(2, _vbuf, 4, GL_UNSIGNED_BYTE, true, offsetof(vtx, col));
            _pip.bind_uniform_buffer(0, _pbuf);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            for (const auto& ind : _indirects)
            {
                _pbuf[0].has_texture = ind.texture != gl_texture_t::zero;
                _pbuf[0].tex = ind.texture != gl_texture_t::zero ? _sampler.sample(ind.texture) : 0;
                _pbuf.synchronize();
                _pip.draw(GL_TRIANGLES, _ibuf, GL_UNSIGNED_SHORT, ind.count, ind.base_index, ind.base_vertex);
            }
            glDisable(GL_BLEND);

            _vbuf.clear();
            _ibuf.clear();
            _indirects.clear();
        }

        void draw_triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::u8vec4 color) {
            indirect ind;
            ind.base_vertex = _vbuf.size();
            ind.base_index = _ibuf.size();
            ind.count = prim_triangle.size();
            ind.texture = gl_texture_t::zero;
            _vbuf.emplace_back(vtx{ a,{ 0, 0 }, color });
            _vbuf.emplace_back(vtx{ b,{ 0, 0 }, color });
            _vbuf.emplace_back(vtx{ c,{ 0, 0 }, color });
            _ibuf.insert(_ibuf.end(), prim_triangle.begin(), prim_triangle.end());
            _indirects.push_back(ind);
        }

        void draw_quad(glm::vec2 min, glm::vec2 max, glm::u8vec4 color) {
            draw_quad(min, max, { 0, 0 }, { 1, 1 }, gl_texture_t::zero, color);
        }

        void draw_quad(glm::vec2 min, glm::vec2 max, glm::vec2 uvmin, glm::vec2 uvmax, gl_texture_t texture, glm::u8vec4 color = { 255, 255, 255, 255 }) {
            indirect ind;
            ind.base_vertex = _vbuf.size();
            ind.base_index = _ibuf.size();
            ind.count = prim_quad.size();
            ind.texture = texture;
            _vbuf.emplace_back(vtx{ { min.x, max.y },{ uvmin.x, uvmax.y }, color });
            _vbuf.emplace_back(vtx{ { min.x, min.y },{ uvmin.x, uvmin.y }, color });
            _vbuf.emplace_back(vtx{ { max.x, min.y },{ uvmax.x, uvmin.y }, color });
            _vbuf.emplace_back(vtx{ { max.x, max.y },{ uvmax.x, uvmax.y }, color });
            _ibuf.insert(_ibuf.end(), prim_quad.begin(), prim_quad.end());
            _indirects.push_back(ind);
        }

        void draw_text(const std::wstring& text, font* font, glm::vec2 bmin, glm::vec2 bmax, glm::u8vec4 color = { 255, 255, 255, 255 })
        {
            glm::vec2 base_pos{ bmin.x, bmax.y };
            float adv = 0;
            float advy = -font->size() + 0.2f*font->size();
            for (const auto& c : text)
            {
                if (c == L' ')
                {
                    float fwidth = adv;
                    const wchar_t* p = (&c);
                    do
                    {
                        ++p;
                        fwidth += font->at(*p).offx;
                    } while (*p != '\0' && *p != ' ');
                    if (fwidth > bmax.x-bmin.x)
                    {
                        advy -= font->size();
                        adv = 0;
                        if (-advy > bmax.y - bmin.y)
                            break;
                        if (c==' ')
                            continue;
                    }
                }

                game::glyph g = font->at(c);
                draw_quad({ base_pos.x + g.x0 + adv, base_pos.y + g.y0 + advy }, { base_pos.x+g.x1 + adv, base_pos.y+g.y1 +advy }, { g.s0, g.t0 }, { g.s1, g.t1 }, font->atlas, color);
                adv += g.offx;
            }
        }

    private:
        constexpr static const std::array<idx, 3> prim_triangle{ 0, 1, 2 };
        constexpr static const std::array<idx, 6> prim_quad{ 0, 1, 2, 0, 2, 3 };

        GLFWwindow* _win;
        gl::sampler _sampler;
        std::vector<indirect> _indirects;
        gl::buffer<vtx> _vbuf;
        gl::buffer<idx> _ibuf;
        gl::buffer<prop> _pbuf;
        gl::pipeline _pip;
    };
    static ui& default_ui()
    {
        static ui x;
        return x;
    }
}