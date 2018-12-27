#include "app.hpp"
#include "opengl.hpp"
#include "vulkan.hpp"
#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
    using namespace std::string_view_literals;

    glfwInit();

    opengl_app opengl;
    vulkan_app vulkan;

    if (argc <= 1 || argv[1] == "gl"sv) opengl.run();
    if (argc <= 1 || argv[1] == "vk"sv) vulkan.run();

    opengl.wait_until_closed();
    vulkan.wait_until_closed();
}
