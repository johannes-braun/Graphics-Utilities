#include "host.hpp"
#include "states.hpp"
#include "res/image.hpp"

namespace game
{
    float _p = 0.f;
    extern float& progress = _p;

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
/*
    struct vt
    {
        glm::vec2 p;
        glm::vec2 uv;
        glm::u8vec4 col;
    };

    struct data
    {
        glm::mat4 smat;
        uintptr_t img;
    };*/

    struct splash_info
    {
        gl::sampler smp;
        gl::texture image{ GL_TEXTURE_2D };
        gl::texture bg_img{ GL_TEXTURE_2D };
        /*gl::buffer<data> databuf;
        gl::buffer<vt> vbo;
        gl::buffer<uint8_t> ibo;
        gl::pipeline ppl;*/
    };

    static ui& get_ui()
    {
        static ui x;
        return x;
    }

    static splash_info& ingo() {
        static splash_info info;
        return info;
    }
    double _splash_time;
    bool splash()
    {
        static int i = [&]() {
            const res::image img = res::load_svg_rasterized("../res/ui/logo.svg", 20.f);
            ingo().image = gl::texture(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8);
            ingo().image.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
            ingo().image.generate_mipmaps();
            const res::image bg = res::load_image("../res/board.png", res::image_type::u8, res::RGBA);
            ingo().bg_img = gl::texture(GL_TEXTURE_2D, bg.width, bg.height, GL_RGBA8);
            ingo().bg_img.assign(GL_RGBA, GL_UNSIGNED_BYTE, bg.data.get());
            ingo().bg_img.generate_mipmaps();
            return 0;
        }();

        host::window->set_max_framerate(60.0);
        _splash_time = glfwGetTime();
        game::host::set_state(loop_splash);
        return true;
    }

    bool run_splash()
    {
        float mx = sqrt(std::min(3.0f*(glfwGetTime() - _splash_time), 1.0));

        int size = 256 * mx;
        glm::vec4 start(0, 0, 0, 1);
        glm::vec4 end(0.1f, 0.4f, 0.01f, 1.f);
        glm::vec4 mid = mix(start, end, mx);
        glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, &mid[0]);

        int w, h; glfwGetFramebufferSize(*host::window, &w, &h);
        get_ui().draw_quad({ 0, 0 }, { w,h }, { 0, 0 }, { w / 8, h / 8 }, ingo().bg_img);
        get_ui().draw_quad({ w/2-size / 2, h/2-size / 2 }, { w / 2 + size / 2, h / 2 + size / 2 }, { 0, 1 }, { 1, 0 }, ingo().image);

        // progress bar
        get_ui().draw_quad({ 0, 0 }, { w, 24.f }, { 255, 255, 255, 64 });
        get_ui().draw_quad({ 0, 0 }, { _p * w, 24.f }, { 255, 255, 255, 255 });

        get_ui().draw();

       /* if (glfwGetTime() - _splash_time > glm::pi<double>() || glfwGetKey(*game::host::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            game::host::set_state(loop_menu);
*/
        return true;
    }
}