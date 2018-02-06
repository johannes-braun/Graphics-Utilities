#pragma once

#include <GLFW/glfw3.h>

namespace io
{
    enum class api
    {
        opengl = GLFW_OPENGL_API,
        vulkan = GLFW_NO_API,
    };
}