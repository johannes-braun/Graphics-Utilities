#include "host.hpp"
#include "states.hpp"

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
    float col[] = { 1, 0.5f, 0.2f, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

    ImGui::Begin("Luhl");
    if (ImGui::Button("Menu."))
        game::host::set_state(game::loop_menu);
    if (ImGui::Button("Quit."))
        game::host::set_state(game::do_quit);
    ImGui::End();

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

    while (game::host::do_loop()) { }

    return 0;
}