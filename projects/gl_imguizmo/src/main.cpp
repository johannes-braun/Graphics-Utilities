#include <gfx/camera.hpp>
#include <gfx/imgui.hpp>
#include <gfx/window.hpp>

#include <imgui/ImGuizmo.h>

int main()
{
    auto       win = std::make_shared<gfx::window>(gfx::apis::opengl::name, "ImGuizmo", 1280, 720);
    gfx::imgui gui(win);

    gfx::camera            camera;
    gfx::camera_controller controller(win);
    glm::vec3              pos1(0, 0, 0);
    glm::vec3              pos2(1, 0, 0);
    glm::vec3              pos3(1, 1, 0);
    glm::vec3              pos4(0, 1, 0);
    glm::vec2              size(1280, 720);

    gfx::transform transform;

    while(win->update())
    {
        gui.new_frame();
        ImGuizmo::BeginFrame();
        ImGuizmo::SetRect(0, 0, 1280, 720);
        glm::mat4 tfmat = transform;
        ImGuizmo::Manipulate(glm::value_ptr(camera.view()),
                             glm::value_ptr(camera.projection()),
                             ImGuizmo::ROTATE,
                             ImGuizmo::WORLD,
                             glm::value_ptr(tfmat));
        transform = tfmat;

        controller.update(camera);


        ImGui::Begin("touiashd");
        ImGui::End();


        gui.render();
    }
}
