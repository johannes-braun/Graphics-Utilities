#pragma once

#include <mygl/mygl.hpp>

struct glcutoff_pass
{
    glcutoff_pass(glm::ivec2 size) : size(size)
    {
        glCreateFramebuffers(1, &fb);
        glCreateTextures(GL_TEXTURE_2D, 1, &fb_tex);
        glTextureStorage2D(fb_tex, 1, GL_RGBA16F, size.x, size.y);
        glNamedFramebufferTexture(fb, GL_COLOR_ATTACHMENT0, fb_tex, 0);

        glCreateSamplers(1, &tex_sampler);
        glSamplerParameteri(tex_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(tex_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(tex_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        create_pipeline();
    }

    void render(mygl::texture input)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);    // What the actual f*?
        glUseProgram(program);
        glViewportIndexedf(0, 0, 0, size.x, size.y);
        glScissorIndexed(0, 0, 0, size.x, size.y);
        glBindTextureUnit(0, input);
        glBindSampler(0, tex_sampler);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    const mygl::texture&     get_image() const { return fb_tex; }
    const mygl::framebuffer& get_framebuffer() const { return fb; }

private:
    void create_pipeline()
    {
        if (glIsProgram(program)) glDeleteProgram(program);
        program = glCreateProgram();

        auto vert = glCreateShader(GL_VERTEX_SHADER);
        auto frag = glCreateShader(GL_FRAGMENT_SHADER);

        shader_lib.load("vk_and_gl.blur_shaders_gl");

        const auto vs = gfx::import_shader(shader_lib, "shaders/screen.vert");
        glShaderBinary(1, &vert, GL_SHADER_BINARY_FORMAT_SPIR_V, vs->data(), vs->size() * sizeof(std::uint32_t));
        glSpecializeShader(vert, "main", 0, nullptr, nullptr);

        const auto fs = gfx::import_shader(shader_lib, "shaders/cutoff.frag");
        glShaderBinary(1, &frag, GL_SHADER_BINARY_FORMAT_SPIR_V, fs->data(), fs->size() * sizeof(std::uint32_t));
        glSpecializeShader(frag, "main", 0, nullptr, nullptr);

        std::int32_t ill = 0;
        glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &ill);
        if (ill > 1)
        {
            std::string l(ill, ' ');
            
            glGetShaderInfoLog(frag, ill, &ill, l.data());
            std::cout << l << '\n';
        }

        glAttachShader(program, vert);
        glAttachShader(program, frag);
        glLinkProgram(program);

        glDetachShader(program, vert);
        glDetachShader(program, frag);
        glDeleteShader(vert);
        glDeleteShader(frag);
        shader_lib.unload();

        res_uniform_index = glGetUniformLocation(program, "resolution_c");
    }

    glm::ivec2           size;
    mygl::framebuffer    fb;
    mygl::texture        fb_tex;
    mygl::sampler        tex_sampler;
    mygl::shader_program program;

    std::int32_t res_uniform_index;

    gfx::dynamic_library_t shader_lib;
};
