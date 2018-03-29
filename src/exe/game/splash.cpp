#include "host.hpp"
#include "states.hpp"
#include "res/image.hpp"

namespace game
{
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
    };

    struct splash_info
    {
        gl::sampler smp;
        gl::texture image{ GL_TEXTURE_2D };
        gl::texture sec_img{ GL_TEXTURE_2D };
        gl::buffer<data> databuf;
        gl::buffer<vt> vbo;
        gl::buffer<uint8_t> ibo;
        gl::pipeline ppl;
    };

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
            const res::image sec = res::load_image("../res/ui/icons/ic_image.png", res::image_type::u8, res::RGBA);
            ingo().sec_img = gl::texture(GL_TEXTURE_2D, sec.width, sec.height, GL_RGBA8);
            ingo().sec_img.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
            ingo().sec_img.generate_mipmaps();
            ingo().databuf = gl::buffer<data>(1, GL_DYNAMIC_STORAGE_BIT);
            ingo().vbo = gl::buffer<vt>(8, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
            ingo().vbo.map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
            ingo().ibo = { 0, 1, 2, 0, 2, 3 };
            ingo().ppl[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("splash/vs.vert");
            ingo().ppl[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("splash/fs.frag");
            return 0;
        }();
        _splash_time = glfwGetTime();
        game::host::set_state(loop_splash);
        return true;
    }

    bool run_splash()
    {
        float mx = sin(std::min(1.0f*(glfwGetTime() - _splash_time), glm::pi<double>()));

        int size = 256 * mx;
        glm::vec4 start(0, 0, 0, 1);
        glm::vec4 end(0.1f, 0.4f, 0.01f, 1.f);
        glm::vec4 mid = mix(start, end, mx);
        glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, &mid[0]);

        ingo().vbo[0] = vt{ { -size / 2, size / 2 }, {1, 0}, {255, 255, 255, 255} };
        ingo().vbo[1] = vt{ { -size / 2, -size / 2 }, {1, 1}, { 255, 255, 255, 255 } };
        ingo().vbo[2] = vt{ { size / 2, -size / 2 }, {0, 1}, { 255, 255, 255, 255 } };
        ingo().vbo[3] = vt{ { size / 2, size / 2 }, {0, 0}, { 255, 255, 255, 255 } };

        ingo().vbo[4] = vt{ { -32, -200.f + 32},{ 1, 0 },{ 255, 255, 255, 255 * pow(mx, 2) } };
        ingo().vbo[5] = vt{ { -32, -200.f - 32 },{ 1, 1 },{ 255, 255, 255, 255 * pow(mx, 2) } };
        ingo().vbo[6] = vt{ { 32, -200.f - 32 },{ 0, 1 },{ 255, 255, 255, 255 * pow(mx, 2) } };
        ingo().vbo[7] = vt{ { 32, -200.f + 32 },{ 0, 0 },{ 255, 255, 255, 255 * pow(mx, 2) } };

        ingo().databuf[0].img = ingo().smp.sample(ingo().image);
        ingo().databuf[0].smat = glm::scale(glm::vec3(1.f/1280, 1.f/720, 1));
        ingo().databuf.synchronize();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ingo().ppl.bind();
        ingo().ppl.bind_attribute(0, ingo().vbo, 2, GL_FLOAT);
        ingo().ppl.bind_attribute(1, ingo().vbo, 2, GL_FLOAT, offsetof(vt, uv));
        ingo().ppl.bind_attribute(2, ingo().vbo, 4, GL_UNSIGNED_BYTE, true, offsetof(vt, col));
        ingo().ppl.bind_uniform_buffer(0, ingo().databuf);
        ingo().ppl.draw(GL_TRIANGLES, ingo().ibo, GL_UNSIGNED_BYTE, 6, 0, 0);

        ingo().databuf[0].img = ingo().smp.sample(ingo().image);
        ingo().databuf.synchronize();
        ingo().ppl.draw(GL_TRIANGLES, ingo().ibo, GL_UNSIGNED_BYTE, 6, 0, 4);
        glDisable(GL_BLEND);

        if (glfwGetTime() - _splash_time > glm::pi<double>() || glfwGetKey(*game::host::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            game::host::set_state(loop_menu);

        return true;
    }
}