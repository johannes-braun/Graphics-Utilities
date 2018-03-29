#include "host.hpp"
#include "states.hpp"
#include <random>

bool menu() {
    float col[] = { 0, 0, 0, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);
    
    ImGui::Begin("Lel");
    if (ImGui::Button("Run."))
        game::host::set_state(game::loop_ingame);
    if (ImGui::Button("Splash!"))
        game::host::set_state(game::prepare_splash);
    if (ImGui::Button("Quit."))
        game::host::set_state(game::do_quit);
    ImGui::End();

    return true;
}

bool ingame() {
    //static ui u(*game::host::window);
    float col[] = { 1, 0.5f, 0.2f, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

    ImGui::Begin("Luhl");
    if (ImGui::Button("Menu."))
        game::host::set_state(game::loop_menu);
    if (ImGui::Button("Quit."))
        game::host::set_state(game::do_quit);
    ImGui::DragFloat("Prog", &game::progress, 0.1f, 0.f, 1.f);

    ImGui::End();

    int w, h; glfwGetFramebufferSize(*game::host::window, &w, &h);

   /* u.draw_quad({0, 0 }, { w, 24.f }, {64, 64, 64, 255});
    u.draw_quad({0, 0 }, { progress * w, 24.f }, { 255, 255, 255, 255 });

    u.draw();*/
    return true;
}

int main()
{
    if (!game::host::init("../asset/settings.xml"))
        return 1;

    game::host::state_loops[game::loop_menu] = &menu;
    game::host::state_loops[game::loop_ingame] = &ingame;
    game::host::state_loops[game::prepare_splash] = &game::splash;
    game::host::state_loops[game::loop_splash] = &game::run_splash;
    game::host::set_state(game::prepare_splash);

    game::progress = 0.f;
    game::host::do_loop();
    game::host::do_loop();
    game::host::do_loop();
    std::mt19937 gn;
    std::uniform_real_distribution<float> dst(0.f, 1.f);
    while (game::progress != 1.f)
    {
        double d = glfwGetTime();
        while (glfwGetTime() - d < 0.5f)
        {
        }
        game::progress = std::min(game::progress+dst(gn), 1.f);
        game::host::do_loop();
        game::host::do_loop();
    }
    game::host::set_state(game::loop_menu);
    while (game::host::do_loop()) { }

    return 0;
}