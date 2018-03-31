#include "host.hpp"
#include "states.hpp"
#include "splash.hpp"
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
#include <glsp/glsp.hpp>
#include <glsp/definition.hpp>
#include <opengl/context.hpp>
#include <glshader/huffman.hpp>

int main()
{
   /* gl::context contikz(gl::native_handle::null, { { GL_CONTEXT_FLAGS_ARB, GL_CONTEXT_DEBUG_BIT_ARB } });

    glsp::state preproc_state;
    preproc_state.add_definition("my_def(a, b, c) (-a + b * c)"_gdef);
    preproc_state.add_definition({ "MY_VAL", 2 });
    preproc_state.add_definition({ "NO_VAL" });
    preproc_state.add_definition({ "AddMul",{ { "a", "b", "c" }, "a + b * c" } });
    preproc_state.add_include_dir("../shaders/");
    auto file = preproc_state.preprocess_file("my_file.glsl");

    auto def = "my_def(a, b, c) (-a + b * c)"_gdef;
    glsp::definition def_my_val("MY_VAL", 2);
    glsp::definition def_no_val("NO_VAL");
    glsp::definition def_add_mul("AddMul", { {"a", "b", "c"}, "a + b * c" });
    glsp::preprocess_file("file", {}, { def_my_val, def_no_val, def_add_mul });
    */

    std::vector<uint32_t> vecy(2311);
    std::generate(vecy.begin(), vecy.end(), []() { return uint8_t(rand()); });
    std::basic_string<uint8_t> vec(reinterpret_cast<uint8_t*>(vecy.data()), reinterpret_cast<uint8_t*>(vecy.data()+vecy.size()));
    auto encoded = huffman::encode(vec);
    auto decoded = huffman::decode(encoded);
    int enc = encoded.length();
    int dec = decoded.length();

    bool eq = std::equal(vec.begin(), vec.end(), decoded.begin(), decoded.end());

    std::vector<uint32_t> vecx(reinterpret_cast<uint32_t*>(decoded.data()), reinterpret_cast<uint32_t*>(decoded.data()) + decoded.size() / sizeof(uint32_t));


    if (!game::host::init("../asset/settings.xml"))
        return 1;

    return 0;

    game::splash splash(game::host::window);

    double d = glfwGetTime();
    while (glfwGetTime() - d < 0.5f)
    {
    }
    splash.set_progress(0.3f);
    d = glfwGetTime();
    while (glfwGetTime() - d < 0.8f)
    {
    }
    splash.set_progress(0.8f);
    d = glfwGetTime();
    while (glfwGetTime() - d < 0.4f)
    {
    }
    splash.set_progress(1.f);
    /* game::host::state_loops[game::loop_menu] = &menu;
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
     while (game::host::do_loop()) { }*/

    return 0;
}