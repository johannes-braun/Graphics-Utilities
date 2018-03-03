#pragma once

#include <GLFW/glfw3.h>
#include <jpu/memory>

namespace io
{
    class cursor : public jpu::ref_count
    {
    public:
        cursor(const int w, const int h, const void* data, const int hotspot_x, const int hotspot_y)
            : _hotspot_x(hotspot_x), _hotspot_y(hotspot_y)
        {
            struct img
            {
                int width;
                int height;
                const unsigned char* pixels;
            } icon_image;
            icon_image.width = w;
            icon_image.height = h;
            icon_image.pixels = static_cast<const unsigned char*>(data);
            _cursor = glfwCreateCursor(reinterpret_cast<const GLFWimage*>(&icon_image), _hotspot_x, _hotspot_y);
        }

        ~cursor()
        {
            glfwDestroyCursor(_cursor);
        }

        operator GLFWcursor*() const { return _cursor; }

    private:
        int _hotspot_x;
        int _hotspot_y;
        GLFWcursor* _cursor;
    };
}
