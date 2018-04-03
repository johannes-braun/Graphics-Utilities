#pragma once

#include <GLFW/glfw3.h>
#include <opengl/gl.hpp>
#include <opengl/buffer.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/texture.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <functional>
#include <map>

#include "stb_rect_pack.h"
#include "stb_truetype.h"

namespace game
{
    enum cursor_type
    {
        CUR_NORMAL = 0,
        CUR_HAND = GLFW_HAND_CURSOR,
        CUR_VRESIZE = GLFW_VRESIZE_CURSOR,
        CUR_HRESIZE = GLFW_HRESIZE_CURSOR
    };

    class ui_window;
    struct ui_window_prioritizer
    {
        bool operator()(const std::pair<int, ui_window*>& w1, const std::pair<int, ui_window*>& w2) const
        {
            return w1.first < w2.first || (w1.first == w2.first && w1.second < w2.second);
        }
    };

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

        glyph at(wchar_t c) {
            glyph g;
            stbtt_GetPackedQuad(_characters.data(), _atlas_dimension, _atlas_dimension, c - first_char, &g.offx, &g.offy, reinterpret_cast<stbtt_aligned_quad*>(&g), 1);
            g.y1 = -g.y1;
            g.y0 = -g.y0;
            return g;
        }
        const glyph at(wchar_t c) const { return const_cast<font*>(this)->at(c); }
        glyph operator[](wchar_t c) { return at(c); }
        const glyph operator[](wchar_t c) const { return at(c); }

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
            std::function<void()> cmd;
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
            _sampler.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            _sampler.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            _sampler.set(GL_TEXTURE_MAX_LOD, 0);
        }

        void draw()
        {
            static std::map<cursor_type, GLFWcursor*> curs = []() {
                std::map<cursor_type, GLFWcursor*> mp;
                mp[CUR_NORMAL] = nullptr;
                mp[CUR_HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
                mp[CUR_HRESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                mp[CUR_VRESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                return mp;
            }();

            int w, h; glfwGetFramebufferSize(_win, &w, &h);
            glfwSetCursor(_win, curs[_cursor]);
            _pbuf[0].screen = glm::scale(glm::translate(glm::vec3(-1, -1, 0)), glm::vec3(2.f / w, 2.f / h, 1));

            _vbuf.clear();
            _vbuf.reserve(verts.size());
            _vbuf.insert(_vbuf.begin(), verts.begin(), verts.end());
            _ibuf.clear();
            _ibuf.reserve(idxes.size());
            _ibuf.insert(_ibuf.begin(), idxes.begin(), idxes.end());

            _pip.bind();
            _pip.bind_attribute(0, _vbuf, 2, GL_FLOAT, offsetof(vtx, pos));
            _pip.bind_attribute(1, _vbuf, 2, GL_FLOAT, offsetof(vtx, uv));
            _pip.bind_attribute(2, _vbuf, 4, GL_UNSIGNED_BYTE, true, offsetof(vtx, col));
            _pip.bind_uniform_buffer(0, _pbuf);

            glEnable(GL_BLEND);
            glEnable(GL_SCISSOR_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            for (const auto& list : _indirects)
            {
                for (const auto& ind : list.second)
                {
                    if (ind.cmd)
                    {
                        ind.cmd();
                    }
                    else
                    {
                        _pbuf[0].has_texture = ind.texture != gl_texture_t::zero;
                        _pbuf[0].tex = ind.texture != gl_texture_t::zero ? _sampler.sample(ind.texture) : 0;
                        _pbuf.synchronize();
                        _pip.draw(GL_TRIANGLES, _ibuf, GL_UNSIGNED_SHORT, ind.count, ind.base_index, ind.base_vertex);
                    }
                }
            }
            glScissor(0, 0, w, h);
            glDisable(GL_SCISSOR_TEST);
            glDisable(GL_BLEND);

            _vbuf.clear();
            _ibuf.clear();
            verts.clear();
            idxes.clear();
            _indirects.clear();
            set_cursor(CUR_NORMAL);
        }

        void draw_triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::u8vec4 color) {
            indirect ind;
            ind.base_vertex = verts.size();
            ind.base_index = idxes.size();
            ind.count = prim_triangle.size();
            ind.texture = gl_texture_t::zero;
            verts.emplace_back(vtx{ a,{ 0, 0 }, color });
            verts.emplace_back(vtx{ b,{ 0, 0 }, color });
            verts.emplace_back(vtx{ c,{ 0, 0 }, color });
            idxes.insert(idxes.end(), prim_triangle.begin(), prim_triangle.end());
            _indirects[{_current_prio, _current_window}].push_back(ind);
        }

        void draw_quad(glm::vec2 min, glm::vec2 max, glm::u8vec4 color) {
            draw_quad(min, max, { 0, 0 }, { 1, 1 }, gl_texture_t::zero, color);
        }

        void draw_quad(glm::vec2 min, glm::vec2 max, glm::vec2 uvmin, glm::vec2 uvmax, gl_texture_t texture, glm::u8vec4 color ={ 255, 255, 255, 255 }) {
            indirect ind;
            ind.base_vertex = verts.size();
            ind.base_index = idxes.size();
            ind.count = prim_quad.size();
            ind.texture = texture;
            verts.emplace_back(vtx{ { min.x, max.y },{ uvmin.x, uvmax.y }, color });
            verts.emplace_back(vtx{ { min.x, min.y },{ uvmin.x, uvmin.y }, color });
            verts.emplace_back(vtx{ { max.x, min.y },{ uvmax.x, uvmin.y }, color });
            verts.emplace_back(vtx{ { max.x, max.y },{ uvmax.x, uvmax.y }, color });
            idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());
            _indirects[{_current_prio, _current_window}].push_back(ind);
        }

        void draw_quad_shadow(glm::vec2 min, glm::vec2 max, glm::u8vec4 color ={ 0, 0, 0, 128 }) {

            float size = 3.f;
            glm::vec2 offset{ 0, -1.f };
            glm::u8vec4 transparent = color;
            transparent.a = 0;

            indirect ind;
            ind.base_vertex = verts.size();
            ind.base_index = idxes.size();
            ind.count = 5*6;
            ind.texture = gl_texture_t(0);

            verts.emplace_back(vtx{ { min.x+offset.x, max.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { min.x+offset.x, min.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { max.x+offset.x, min.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { max.x+offset.x, max.y+offset.y },{ 0,0 }, color });
            idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());

            int id = 4;
            // bottom
            verts.emplace_back(vtx{ { min.x+offset.x, min.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { min.x - size+offset.x, min.y - size+offset.y },{ 0,0 }, transparent });
            verts.emplace_back(vtx{ { max.x + size+offset.x, min.y - size+offset.y },{ 0,0 }, transparent });
            verts.emplace_back(vtx{ { max.x+offset.x, min.y+offset.y },{ 0,0 }, color });
            {
                std::array<idx, 6> prim_quad{ id+0, id+1, id+2, id+0, id+2, id+3 };
                idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());
            }
            id += 4;

            // top
            verts.emplace_back(vtx{ { min.x - size+offset.x, max.y + size+offset.y },{ 0,0 }, transparent });
            verts.emplace_back(vtx{ { min.x+offset.x, max.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { max.x+offset.x, max.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { max.x + size+offset.x, max.y + size+offset.y },{ 0,0 }, transparent });
            {
                std::array<idx, 6> prim_quad{ id+0, id+1, id+2, id+0, id+2, id+3 };
                idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());
            }
            id += 4;

            // left
            verts.emplace_back(vtx{ { min.x - size+offset.x, max.y + size+offset.y },{ 0,0 }, transparent });
            verts.emplace_back(vtx{ { min.x - size+offset.x, min.y - size+offset.y },{ 0,0 }, transparent });
            verts.emplace_back(vtx{ { min.x+offset.x, min.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { min.x+offset.x, max.y+offset.y },{ 0,0 }, color });
            {
                std::array<idx, 6> prim_quad{ id+0, id+1, id+2, id+0, id+2, id+3 };
                idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());
            }
            id += 4;

            // right
            verts.emplace_back(vtx{ { max.x+offset.x, max.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { max.x+offset.x, min.y+offset.y },{ 0,0 }, color });
            verts.emplace_back(vtx{ { max.x + size+offset.x, min.y - size+offset.y },{ 0,0 }, transparent });
            verts.emplace_back(vtx{ { max.x + size+offset.x, max.y + size+offset.y },{ 0,0 }, transparent });
            {
                std::array<idx, 6> prim_quad{ id+0, id+1, id+2, id+0, id+2, id+3 };
                idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());
            }
            id += 4;

            _indirects[{_current_prio, _current_window}].push_back(ind);
        }

        void scissor(glm::vec2 min, glm::vec2 max)
        {
            indirect ind;
            ind.base_vertex = 0;
            ind.base_index = 0;
            ind.count = 0;
            ind.texture = gl_texture_t(0);
            ind.cmd = [min, max]() { glScissor(std::ceil(min.x), std::ceil(min.y), std::ceil(max.x - min.x), std::ceil(max.y - min.y)); };
            _indirects[{_current_prio, _current_window}].push_back(ind);
        }

        std::pair<glm::vec2, glm::vec2> inset(glm::vec2 bmin, glm::vec2 bmax, glm::vec4 pad)
        {
            bmin += glm::vec2(pad.x, pad.y);
            bmax -= glm::vec2(pad.z, pad.w);
            return { bmin, bmax };
        }

        struct line_info
        {
            float width;
            wchar_t delim;
            int count;
        };

    private:
        static bool may_delim_line(const wchar_t* c) noexcept
        {
            return *c==' ' || *c=='-';
        }
    public:

        line_info line_width(const wchar_t* begin, const font& font, float max_width)
        {
            line_info info;
            info.width = 0;
            info.delim = L'\0';
            info.count = 0;

            line_info at_last_delim = info;

            while (true)
            {
                if (*begin == '\0')
                {
                    info.delim = L'\0';
                    return info;
                }
                if (*begin == '\n')
                {
                    info.delim = L'\n';
                    return info;
                }
                if (may_delim_line(begin))
                {
                    at_last_delim.delim = *begin;
                    at_last_delim.count = info.count;
                    at_last_delim.width = info.width;
                }

                const glyph g = font[*begin];
                info.width += g.offx;
                if (info.width > max_width)
                    return at_last_delim;

                ++info.count;
                ++begin;
            }

            /* float adv = 0;
             int id = 0;
             for (auto ch = begin; *ch != '\0'; ++ch)
             {
                 const wchar_t& c = *ch;
                 if (c == L' ')
                 {
                     float fwidth = adv;
                     const wchar_t* p = (&c);
                     do
                     {
                         ++p;
                         if (*p=='\0') break;
                         fwidth += font[*p].offx;
                     } while (*p != '\0' && *p != ' ');
                     if (fwidth > max_width)
                     {
                         return { adv, *p };
                     }
                 }
                 if (c == L'\n')
                 {
                     return { adv, L'\n' };
                 }

                 game::glyph g = font[c];
                 adv += g.offx;
             }
             return { adv, '\0' };*/
        }

        glm::vec2 text_bounds(const std::wstring& text, const font& font, float max_width)
        {
            glm::vec2 base{ 0, -font.size() };
            float adv = 0;
            int id = 0;
            for (const auto& c : text)
            {
                if (c == L' ')
                {
                    float fwidth = adv;
                    const wchar_t* p = (&c);
                    do
                    {
                        ++p;
                        if (*p=='\0') break;
                        fwidth += font[*p].offx;
                    } while (*p != '\0' && *p != ' ');
                    if (*p != '\0')
                        fwidth -= font[*p].offx;
                    if (fwidth > max_width)
                    {
                        base.y -= font.size();
                        adv = 0;
                        if (c==' ')
                            continue;
                    }
                }
                if (c == L'\n')
                {
                    base.y -= font.size();
                    adv = 0;
                    continue;
                }

                game::glyph g = font[c];
                adv += g.offx;
                base.x = std::max<float>(adv, base.x);
            }
            base.y = abs(base.y);
            return base;
        }

        enum text_align
        {
            ALIGN_LEFT      = 0x1<<0,
            ALIGN_CENTER    = 0x1<<1,
            ALIGN_RIGHT     = 0x1<<2,
            ALIGN_JUSTIFY   = 0x1<<3,
            ALIGN_DEFAULT   = ALIGN_LEFT,
        };

        void push_list(int prio, ui_window* window)
        {
            _current_window = window;
            _current_prio = prio;
        }

        void pop_list()
        {
            _current_window = nullptr;
            _current_prio = -1;
        }

        glm::vec2 draw_text(const std::wstring& text, const font& font, float y_offset, glm::vec2 bmin, glm::vec2 bmax, text_align align, glm::u8vec4 color ={ 255, 255, 255, 255 }, int max_lines = std::numeric_limits<int>::max())
        {
            indirect ind;
            ind.base_vertex = verts.size();
            ind.base_index = idxes.size();
            ind.texture = font.atlas;

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
            auto lwp = line_width(text.data(), font, full_width);
            float max_width = lwp.width - (lwp.delim != '\0' ? 0.01f : 0);
            justify = justify_base && (lwp.delim != '\0' && lwp.delim != '\n');
            float scale_x = justify ? (full_width / max_width) : 1;
            max_width = justify ? full_width : max_width;
            float offset_x = offset_factor * (full_width - max_width);
            int id = 0;
            for (const auto& c : text)
            {
                game::glyph g = font[std::clamp<wchar_t>(c, L' ', 512)];
                if (adv > max_width)
                {
                    if (--max_lines <=0)
                        break;
                    const wchar_t* p = &c + 1;
                    adv = 0;
                    advy -= font.size();
                    height -= font.size();
                    auto lwp = line_width(p, font, full_width);
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
                    verts.emplace_back(vtx{ { min.x + offset_x, max.y },{ uvmin.x, uvmax.y }, color });
                    verts.emplace_back(vtx{ { min.x + offset_x, min.y },{ uvmin.x, uvmin.y }, color });
                    verts.emplace_back(vtx{ { max.x + offset_x, min.y },{ uvmax.x, uvmin.y }, color });
                    verts.emplace_back(vtx{ { max.x + offset_x, max.y },{ uvmax.x, uvmax.y }, color });
                    std::array<idx, 6> prim_quad{ id+0, id+1, id+2, id+0, id+2, id+3 };
                    idxes.insert(idxes.end(), prim_quad.begin(), prim_quad.end());
                    id += 4;
                }
                adv += g.offx * scale_x;
                width = std::max(width, adv);
            }
            ind.count = (id/4) * prim_quad.size();
            _indirects[{_current_prio, _current_window}].push_back(ind);
            return { width, -height };
        }

        void set_cursor(cursor_type cur)
        {
            _cursor = cur;
        }

    private:
        constexpr static const std::array<idx, 3> prim_triangle{ 0, 1, 2 };
        constexpr static const std::array<idx, 6> prim_quad{ 0, 1, 2, 0, 2, 3 };

        cursor_type _cursor = CUR_NORMAL;
        GLFWwindow* _win;
        gl::sampler _sampler;
        int _current_prio = -1;
        ui_window* _current_window = nullptr;
        std::map<std::pair<int, ui_window*>, std::vector<indirect>, ui_window_prioritizer> _indirects;
        std::vector<vtx> verts;
        std::vector<idx> idxes;
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
constexpr game::ui::text_align operator|(game::ui::text_align a, game::ui::text_align b) noexcept { return game::ui::text_align(int(a)|int(b)); }