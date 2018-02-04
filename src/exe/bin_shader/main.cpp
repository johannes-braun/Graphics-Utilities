#define IO_API_OPENGL

#include <jpu/memory>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <binshader/shader.hpp>
#include "io/window.hpp"

int main(int argc, const char** args)
{

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    auto win = jpu::make_ref<io::window>(200, 100, "sad");

    std::vector<std::experimental::filesystem::path> include_directories{ "../shaders" };
    std::vector<res::definition> definitions;

   /* auto src = "../shaders/gbuffer/gbuffer.frag";
    res::shader_format type = res::shader_format::gl_binary;*/
    auto src = "../shaders/gui/vk/imgui.frag";
    res::shader_format type = res::shader_format::spirv;

    auto tp = std::chrono::system_clock::now();
    auto shd_bin = res::load_binary_shader(type, src, include_directories, definitions);

    log_i << "Time passed: " << (std::chrono::system_clock::now() - tp).count();

    system("pause");
    return 0;
}
