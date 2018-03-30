#pragma once

#include <GLFW/glfw3.h>
#include <opengl/gl.hpp>
#include <opengl/buffer.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/texture.hpp>
#include <glm/glm.hpp>

namespace game
{
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