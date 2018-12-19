#include <GLFW/glfw3.h>
#include "opengl.hpp"
#include "vulkan.hpp"
#include "app.hpp"

int main(int argc, char** argv)
{
    glfwInit();

    opengl_app opengl;
    vulkan_app vulkan;

    // opengl.run();
    vulkan.run();
}
