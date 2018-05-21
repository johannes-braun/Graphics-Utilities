#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/query.hpp>
#include <mygl/mygl.hpp>
#include <gfx/log.hpp>

class gauss_blur
{
    gl::pipeline make_pipeline(int direction) const;
public:
    gauss_blur(int kernel_size, float sigma)
        : _kernel_size(kernel_size), _sigma(sigma)
    {
        std::stringstream kernel;
        for (int i=0; i<(_kernel_size>>1) + 1; ++i)
            kernel << 1.025f*glm::gauss(1.f * i, 0.f, _sigma) << ", ";
        _kernel = kernel.str();
        _blur_ppx[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("postfx/screen.vert");
        _blur_ppx[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("blurhw.frag", std::vector<gl::definition>{
            gl::definition("DIRECTION", 0),
                gl::definition("KERNEL_SIZE", _kernel_size),
                gl::definition("KERNEL", _kernel)
        });
        _blur_ppy[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("postfx/screen.vert");
        _blur_ppy[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("blurhw.frag", std::vector<gl::definition>{
            gl::definition("DIRECTION", 1),
                gl::definition("KERNEL_SIZE", _kernel_size),
                gl::definition("KERNEL", _kernel)
        });
    }

    void operator()(const std::shared_ptr<gl::texture>& in, const std::shared_ptr<gl::texture>& out, int level = 0)
    {
        mygl::texture texture_views[2];
        glGenTextures(2, texture_views);

        assert(out->width() == in->width()
            && out->height() == in->height()
            && out->internal_format() == in->internal_format());
        if (!_texture || !(_texture->width() == in->width()
            && _texture->height() == in->height()
            && _texture->internal_format() == in->internal_format()
            && _texture->width() == out->width()
            && _texture->height() == out->height()
            && _texture->internal_format() == out->internal_format()))
            build(in->width(), in->height(), in->internal_format());

        _fbos[0][GL_COLOR_ATTACHMENT0].set(_texture, level);
        _fbos[1][GL_COLOR_ATTACHMENT0].set(out, level);

        glViewport(0, 0, in->width()>>level, in->height()>>level);
        _blur_ppx.bind();
        glBindSampler(0, _sampler);
        glTextureView(texture_views[0],
                      GL_TEXTURE_2D,
                      *in,
                      in->internal_format(),
                      level,
                      1,
                      0,
                      1);
        glBindTextureUnit(0, texture_views[0]);
        _fbos[0].bind();
        _fbos[0].set_drawbuffer(GL_COLOR_ATTACHMENT0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        _blur_ppy.bind();
        glBindSampler(0, _sampler);
        glTextureView(texture_views[1],
                      GL_TEXTURE_2D,
                      *_texture,
                      _texture->internal_format(),
                      level,
                      1,
                      0,
                      1);
        glBindTextureUnit(0, texture_views[1]);
        _fbos[1].bind();
        _fbos[1].set_drawbuffer(GL_COLOR_ATTACHMENT0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDeleteTextures(2, texture_views);
    }

    const gl::framebuffer& fbo(int index) const
    {
        return _fbos[index];
    }

private:

    void build(int width, int height, GLenum format)
    {
        _texture = std::make_shared<gl::texture>(GL_TEXTURE_2D, width, height, format);
    }

    int _kernel_size;
    float _sigma;
    std::string _kernel;
    gl::pipeline _blur_ppx;
    gl::pipeline _blur_ppy;
    std::shared_ptr<gl::texture> _texture;
    gl::framebuffer _fbos[2];
    gl::sampler _sampler;
};

class copy_texture
{
public:
    void operator()(const std::shared_ptr<gl::texture>& in, const std::shared_ptr<gl::texture>& out, int src_level, int dst_level, GLenum filter)
    {
        _fbos[0].set_readbuffer(GL_COLOR_ATTACHMENT0);
        _fbos[1].set_drawbuffer(GL_COLOR_ATTACHMENT0);
        _fbos[0][GL_COLOR_ATTACHMENT0].set(in, src_level);
        _fbos[1][GL_COLOR_ATTACHMENT0].set(out, dst_level);
        _fbos[0].blit(_fbos[1], 0, 0, std::max(in->width()>>src_level, 1), std::max(in->height()>>src_level, 1), 0, 0, std::max(out->width()>>dst_level, 1), std::max(out->height()>>dst_level, 1), GL_COLOR_BUFFER_BIT, filter);
    }

private:
    gl::framebuffer _fbos[2];
};

void dbg(GLenum source, GLenum type, unsigned int id, GLenum severity, int length, const char *message, const void *userParam)
{
    log_i << message;
}

int main() {
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });
    auto win = std::make_shared<gfx::window>(gfx::apis::opengl::name, "OpenGL Playground", 1280, 720);
    //gfx::imgui gui(win);

    glDebugMessageCallback(&dbg, nullptr);

    gfx::image_file img("HD.png", gfx::bits::b8, 4);
    auto tex = std::make_shared<gl::texture>(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8);
    tex->assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
    auto output = std::make_shared<gl::texture>(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8);

    gauss_blur blur(315, 50.f);
    copy_texture copy;
    
    gl::framebuffer out_fbo;
    out_fbo[GL_COLOR_ATTACHMENT0].set(output, 0);

    gl::query qu1(GL_TIME_ELAPSED);
   /* while (win->update())
    {
        qu1.start();
        for (int i=0; i<2; ++i)
        {
            blur(tex, tex, 0);
        }
        qu1.finish();
        std::cout << "Frag: " << qu1.get<uint64_t>() << "\n";

        std::cin.ignore();
        out_fbo.blit(gl::framebuffer::zero(), 0, 0, output->width(), output->height(), 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT);
    }*/

    qu1.start();
    for (int i=0; i<1; ++i)
    {
        blur(tex, tex, 0);
    }
    qu1.finish();
    std::cout << "Frag: " << qu1.get<uint64_t>() << "\n";

    std::vector<glm::u8vec4> data((img.width) * (img.height));
    tex->get_data(GL_RGBA, GL_UNSIGNED_BYTE, (img.width) * (img.height) * 4, data.data());
    gfx::image_file::save_png("output.png", (img.width), (img.height), 4, &(data[0][0]));


    return 0;
}
