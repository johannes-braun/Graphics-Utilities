#include "host.hpp"
#include "states.hpp"
#include "splash.hpp"
#include <random>
#include "ui.hpp"
#include <fstream>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

bool menu() {
    float col[] = { 0, 0, 0, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

    static game::ui ui;
    static std::vector<stbtt_packedchar> characters;
    static game::font fnt("../res/ui/fonts/Poppins-Regular.ttf", 16);

    static std::wstring txt = L"Dies ist ein Typoblindtext. An ihm kann man sehen, ob alle Buchstaben da sind und wie sie aussehen. Manchmal benutzt man Worte wie Hamburgefonts, Rafgenduks oder Handgloves, um Schriften zu testen. Manchmal Sätze, die alle Buchstaben des Alphabets enthalten - man nennt diese Sätze »Pangrams«. Sehr bekannt ist dieser: The quick brown fox jumps over the lazy old dog. Oft werden in Typoblindtexte auch fremdsprachige Satzteile eingebaut (AVAIL® and Wefox™ are testing aussi la Kerning), um die Wirkung in anderen Sprachen zu testen. In Lateinisch sieht zum Beispiel fast jede Schrift gut aus. Quod erat demonstrandum.";

    ui.draw_quad({ 100, 100 }, { 100 + 500.f - 200.f*sin(glfwGetTime()), 300 }, glm::u8vec4(128, 64, 0, 255));
    ui.draw_text(txt, &fnt, { 100, 100 }, { 100 + 500.f - 200.f*sin(glfwGetTime()), 300 });
    glEnable(GL_SCISSOR_TEST);
    glScissor(100, 100, 500.f - 200.f*sin(glfwGetTime()), 200);
    ui.draw();

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
    ImGui::DragFloat("Prog", &game::progress, 0.1f, 0.f, 1.f);

    ImGui::End();

    int w, h; glfwGetFramebufferSize(*game::host::window, &w, &h);

    return true;
}

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

    //std::vector<uint32_t> vecy(2311);
    //std::generate(vecy.begin(), vecy.end(), []() { return uint8_t(rand()); });
    //std::basic_string<uint8_t> vec(reinterpret_cast<uint8_t*>(vecy.data()), reinterpret_cast<uint8_t*>(vecy.data()+vecy.size()));
    //auto encoded = huffman::encode(vec);
    //auto decoded = huffman::decode(encoded);
    //int enc = encoded.length();
    //int dec = decoded.length();

   /* bool eq = std::equal(vec.begin(), vec.end(), decoded.begin(), decoded.end());

    std::vector<uint32_t> vecx(reinterpret_cast<uint32_t*>(decoded.data()), reinterpret_cast<uint32_t*>(decoded.data()) + decoded.size() / sizeof(uint32_t));

*/
    if (!game::host::init("../asset/settings.xml"))
        return 1;

     game::host::state_loops[game::loop_menu] = &menu;
     game::host::state_loops[game::loop_ingame] = &ingame;
     game::host::set_state(game::loop_menu);

     while (game::host::do_loop()) { }

    return 0;
}