#pragma once
#include "app.hpp"
#include <gfx.graphics/shader_import/dynamic_lib.hpp>

class opengl_app : public gfx::basic_app
{
public:
    opengl_app() : gfx::basic_app("OpenGL App") {}
    void on_run() override;

private:
    void create_program();

    gfx::dynamic_library_t shaders_lib;
    void*                  state;
    size_t                 texture_count = 0;
};