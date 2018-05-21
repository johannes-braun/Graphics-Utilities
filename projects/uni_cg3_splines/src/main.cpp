#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/spline.hpp>
#include <opengl/framebuffer.hpp>
#include <numeric>
#include <random>

int main()
{
    gfx::window_hints hints;
    hints[GLFW_SAMPLES] = 8;
    auto window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "[CG3] Splines", 1280, 720, hints);
    gfx::imgui gui(window);

    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });

    gfx::bspline spline;
    spline.add(gfx::vertex2d{ glm::vec2(50, 50), glm::u8vec4(255, 128, 40, 255) });
    spline.add(gfx::vertex2d{ glm::vec2(200, 200), glm::u8vec4(0, 66, 72, 255) });
    spline.add(gfx::vertex2d{ glm::vec2(50, 400), glm::u8vec4(128, 128, 40, 255) });
    spline.add(gfx::vertex2d{ glm::vec2(600, 500), glm::u8vec4(255, 0, 3, 255) });
    spline.add(gfx::vertex2d{ glm::vec2(800, 200), glm::u8vec4(67, 22, 221, 255) });
    spline.add(gfx::vertex2d{ glm::vec2(300, 300), glm::u8vec4(142, 34, 115, 255) });

    gl::buffer<gfx::vertex2d> point_buffer(GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<gfx::vertex2d> cpoint_buffer(GL_DYNAMIC_STORAGE_BIT);

    gl::buffer<glm::mat4> globals_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    globals_buffer[0] = glm::mat4(1.f);
    globals_buffer[0][0][0] = 2.0f / 1280;
    globals_buffer[0][1][1] = -2.0f / 720;
    globals_buffer[0][2] = glm::vec4{ 0.0f, 0.0f, -1.0f, 0.0f };
    globals_buffer[0][3] = glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f };
    globals_buffer.synchronize();

    gl::pipeline spline_pipeline;
    spline_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("spline.vert");
    spline_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("spline.frag");

    std::default_random_engine eng;
    std::uniform_real_distribution<float> dst(0.f, 1.f);

    while (window->update())
    {
        gui.new_frame();
        glDisable(GL_CULL_FACE);
        gl::framebuffer::zero().clear(0, { 1, 0.95f, 1, 1 });
        spline_pipeline.bind();
        spline_pipeline.bind_uniform_buffer(0, globals_buffer);

        double mx, my;
        glfwGetCursorPos(*window, &mx, &my);

        static int dragged = -1;
        static glm::vec2 offset = glm::vec2(0);

        int highlighted = -1;
        bool found = false;
        if (dragged == -1)
        {
            for (int i=0; i<spline.points().size(); ++i)
            {
                spline.highlight(i, false);
                if (!found && glm::distance2(spline[i].position, glm::vec2(mx, my)) < 100.f)
                {
                    highlighted = i;
                    spline.highlight(i, true);
                    offset = glm::vec2(mx, my);
                    found = true;
                }
            }
        }

        static bool mmpress = false;
        if(!mmpress && glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            if(found)
            {
                spline.remove(highlighted);
            }
            else
            {
                struct nn
                {
                    int idx = 0;
                    float dist = std::numeric_limits<float>::max();
                };
                nn cnn;
                for (int i=0; i<spline.points().size(); ++i)
                {
                    const int inext = (i+1) % spline.points().size();
                    const glm::vec2 dir = spline[inext].position - spline[i].position;
                    const glm::vec2 norm = normalize(glm::vec2(dir.y, -dir.x));
                    const glm::vec2 ndir = normalize(glm::vec2(mx, my) - spline[i].position);

                    const bool i1 = dot(dir, ndir) > 0;
                    const bool i2 = dot(-dir, normalize(glm::vec2(mx, my) - spline[inext].position)) > 0;
                    
                    const float d = abs(dot(norm, ndir));
                    if (i1 && i2 && d < cnn.dist)
                    {
                        cnn.idx = inext;
                        cnn.dist = d;
                    }
                }
                spline.add(gfx::vertex2d{ glm::vec2(mx, my), glm::u8vec4(255*dst(eng), 255*dst(eng), 255*dst(eng), 255) }, cnn.idx);
            }
            mmpress = true;
        }
        else if (glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
        {
            mmpress = false;
        }

        if (found && glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            dragged = highlighted;
        else if (glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            dragged = -1;

        if(dragged != -1)
        {
            spline[dragged].position += glm::vec2(mx, my) - offset;
            offset = glm::vec2(mx, my);
        }

        static int order = 3;
        static int res = 32;
        static int type = 0;
        ImGui::Begin("Settings");
        ImGui::DragInt("Order (k)", &order, 0.1f, 1, static_cast<int>(spline.points().size()));
        ImGui::DragInt("Resolution", &res, 0.1f, 16, 128);
        ImGui::Combo("Type", &type, [](void*, int i, const char** text)
        {
            switch(i)
            {
            case 0: *text = "Periodic"; return true;
            case 1: *text = "Open"; return true;
            default: return false;
            }
        }, nullptr, 2);
        ImGui::End();

        std::vector<gfx::vertex2d> sp = spline.build(gfx::bspline::type(type), order, res);
        point_buffer.clear();
        point_buffer.insert(point_buffer.end(), sp.begin(), sp.end());
        point_buffer.insert(point_buffer.end(), spline.points().begin(), spline.points().end());
                
        if (order > 1)
        {
            glLineWidth(10.f);
            spline_pipeline.bind_attribute(0, 0, point_buffer, 2, GL_FLOAT, offsetof(gfx::vertex2d, position));
            spline_pipeline.bind_attribute(1, 0, point_buffer, 4, GL_UNSIGNED_BYTE, true, offsetof(gfx::vertex2d, color));
            spline_pipeline.draw(GL_LINE_STRIP, sp.size());
            glPointSize(10.f);
            spline_pipeline.draw(GL_POINTS, sp.size());
        }

        spline_pipeline.bind_attribute(0, 0, point_buffer, 2, GL_FLOAT, offsetof(gfx::vertex2d, position));
        spline_pipeline.bind_attribute(1, 0, point_buffer, 4, GL_UNSIGNED_BYTE, true, offsetof(gfx::vertex2d, color));
        glLineWidth(2.f);
        spline_pipeline.draw(gfx::bspline::type(type) == gfx::bspline::type::open ? GL_LINE_STRIP : GL_LINE_LOOP, spline.points().size(), sp.size());
        glPointSize(20.f);
        spline_pipeline.draw(GL_POINTS, spline.points().size(), sp.size());
        gui.render();

    }
}