#pragma once

#include <filesystem>
#include <GLFW/glfw3.h>
#include <jpu/memory>
#include <stb_image.h>

namespace io
{
    class cursor : public jpu::ref_count
    {
    public:
        cursor(const std::experimental::filesystem::path& image, const int hotspot_x, const int hotspot_y)
            : _hotspot_x(hotspot_x), _hotspot_y(hotspot_y)
        {
            _image.pixels = stbi_load(image.string().c_str(), &_image.width, &_image.height, nullptr, STBI_rgb_alpha);
            _cursor = glfwCreateCursor(&_image, _hotspot_x, _hotspot_y);
        }

        ~cursor()
        {
            free(_image.pixels);
            glfwDestroyCursor(_cursor);
        }

        operator GLFWcursor*() const { return _cursor; }

    private:
        int _hotspot_x;
        int _hotspot_y;
        GLFWimage _image;
        GLFWcursor* _cursor;
    };
}
