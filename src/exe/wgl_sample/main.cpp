
#include <cstdint>
#include "graphics_host.hpp"
#include <window/window.hpp>
#include <window/monitor.hpp>
#include <opengl/context.hpp>
#include <mygl/gl.hpp>
#include <jpu/log.hpp>

#include <window/imgui.hpp>

int main()
{
    auto window = std::make_shared<wf::window>(L"My Window", 1280, 720);
    auto wintwo = std::make_shared<wf::window>(L"My other Window", 300, 400);

    window->key_callbacks.emplace_back([&window](int k, bool down, bool alt) {
        if (down && k == 'V' && wf::is_key_down(wf::key_control))
            std::wcout << wf::get_clipboard_text();

        if(down && k == wf::mouse_right)
            window->set_cursor_mode(window->get_cursor_mode() == wf::cursor_mode::captured ? wf::cursor_mode::none : wf::cursor_mode::captured);

        if (down && k == wf::mouse_middle)
            window->is_fullscreen() ? window->fullscreen_monitor(nullptr) : window->fullscreen_monitor(window->get_monitor());
    });
    window->resize_callbacks.emplace_back([](int x, int y) {
        log_i << "Resized to " << x << ", " << y;  
    });  

    gl::context context(window->get_native_handle(), { { GL_CONTEXT_FLAGS_ARB, GL_CONTEXT_DEBUG_BIT_ARB } });
    context.make_current();
    context.set_pixel_format({ {GL_SAMPLE_BUFFERS_ARB, true}, {GL_SAMPLES_ARB, 8} });
    context.set_swap_interval(0);
    auto gui = std::make_shared<gfx::imgui>(window);

    gl::context other_ctx(wintwo->get_native_handle(), {}, &context);
    other_ctx.make_current();
    auto other_gui = std::make_shared<gfx::imgui>(wintwo);

    wf::set_clipboard_text(L"Autschi");

    while (window || wintwo)
    {
        if (window && window->poll())
        {
            context.make_current();
            gui->begin();
            float col[]{ 0.3f, 0.55f, 1.f, 1.f };
            glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

            ImGui::Begin("Bla");
            static float f;
            ImGui::DragFloat("Blubas", &f);
            ImGui::End();

            gui->draw();
            context.swap_buffers();
        }
        else window.reset();
        if (wintwo && wintwo->poll())
        {
            other_ctx.make_current();
            other_gui->begin();
            float col[]{ 0.8f, 0.45f, 0.2f, 1.f };
            glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

            ImGui::Begin("You!");
            static float f;
            ImGui::DragFloat("Exceletat", &f);
            ImGui::End();

            other_gui->draw();
            other_ctx.swap_buffers();
        }
        else wintwo.reset();
    }

    return 0;
}