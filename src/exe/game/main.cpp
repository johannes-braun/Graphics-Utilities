#include "host.hpp"
#include "states.hpp"
#include "splash.hpp"
#include <random>
#include "ui.hpp"
#include "ui_window.hpp"
#include <fstream>
#include <numeric>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

bool menu() {
    float col[] = { 0.5f, 0.5f, 0.5f, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);
    glfwSwapInterval(0);
    static game::font fnt("../res/ui/fonts/Poppins-Regular.ttf", 16);
    static game::font title("../res/ui/fonts/Poppins-SemiBold.ttf", 20);
    static std::wstring txt = L"Es gibt im Moment in diese Mannschaft, oh, einige Spieler vergessen ihnen Profi was sie sind. Ich lese nicht sehr viele Zeitungen, aber ich habe gehört viele Situationen. Erstens: wir haben nicht offensiv gespielt. Es gibt keine deutsche Mannschaft spielt offensiv und die Name offensiv wie Bayern. Letzte Spiel hatten wir in Platz drei Spitzen: Elber, Jancka und dann Zickler. Wir müssen nicht vergessen Zickler. Zickler ist eine Spitzen mehr, Mehmet eh mehr Basler. Ist klar diese Wörter, ist möglich verstehen, was ich hab gesagt? Danke. Offensiv, offensiv ist wie machen wir in Platz. Zweitens: ich habe erklärt mit diese zwei Spieler: nach Dortmund brauchen vielleicht Halbzeit Pause. Ich habe auch andere Mannschaften gesehen in Europa nach diese Mittwoch. Ich habe gesehen auch zwei Tage die Training. Ein Trainer ist nicht ein Idiot! Ein Trainer sei sehen was passieren in Platz. In diese Spiel es waren zwei, drei diese Spieler waren schwach wie eine Flasche leer! Haben Sie gesehen Mittwoch, welche Mannschaft hat gespielt Mittwoch? Hat gespielt Mehmet oder gespielt Basler oder hat gespielt Trapattoni? Diese Spieler beklagen mehr als sie spielen! Wissen Sie, warum die Italienmannschaften kaufen nicht diese Spieler? Weil wir haben gesehen viele Male solche Spiel! Haben gesagt sind nicht Spieler für die italienisch Meisters! Strunz! Strunz ist zwei Jahre hier, hat gespielt 10 Spiele, ist immer verletzt! Was erlauben Strunz? Letzte Jahre Meister Geworden mit Hamann, eh, Nerlinger. Diese Spieler waren Spieler! Waren Meister geworden! Ist immer verletzt! Hat gespielt 25 Spiele in diese Mannschaft in diese Verein. Muß respektieren die andere Kollegen! haben viel nette kollegen! Stellen Sie die Kollegen die Frage! Haben keine Mut an Worten, aber ich weiß, was denken über diese Spieler. Mussen zeigen jetzt, ich will, Samstag, diese Spieler müssen zeigen mich, seine Fans, müssen alleine die Spiel gewinnen. Muß allein die Spiel gewinnen! Ich bin müde jetzt Vater diese Spieler, eh der Verteidiger diese Spieler. Ich habe immer die Schuld über diese Spieler. Einer ist Mario, einer andere ist Mehmet! Strunz ich spreche nicht, hat gespielt nur 25 Prozent der Spiel. Ich habe fertig! ...wenn es gab Fragen, ich kann Worte wiederholen...\nEs gibt im Moment in diese Mannschaft, oh, einige Spieler vergessen ihnen Profi was sie sind. Ich lese nicht sehr viele Zeitungen, aber ich habe gehört viele Situationen. Erstens: wir haben nicht offensiv gespielt. Es gibt keine deutsche Mannschaft spielt offensiv und die Name offensiv wie Bayern. Letzte Spiel hatten wir in Platz drei Spitzen: Elber, Jancka und dann Zickler. Wir müssen nicht vergessen Zickler. Zickler ist eine Spitzen mehr, Mehmet eh mehr Basler.";
    static gl::texture close_icon = []() {
        const res::image img = res::load_svg_rasterized("../res/ui/icons/ic_close_white_24px.svg", 1.f);
        gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
        ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
        return ico;
    }();
    static gl::texture minimize_icon = []() {
        const res::image img = res::load_svg_rasterized("../res/ui/icons/ic_remove_white_24px.svg", 1.f);
        gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
        ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
        return ico;
    }();
    static gl::texture maximize_icon = []() {
        const res::image img = res::load_svg_rasterized("../res/ui/icons/ic_fullscreen_white_24px.svg", 1.f);
        gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
        ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
        return ico;
    }();
    static gl::texture window_icon = []() {
        const res::image img = res::load_svg_rasterized("../res/ui/logo.svg", 0.75f);
        gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
        ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
        return ico;
    }();
    static gl::texture image = []() {
        const res::image img = res::load_image("../res/tinted.jpg", res::image_type::u8, res::RGBA);
        gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
        ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
        return ico;
    }();

    static game::ui_layouter layouter;
    static game::ui_window settings(game::host::window, L"My Settings", { 20, 20 }, { 350, 450 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0x00, 0x68, 0x3b, 0xff }, {16, 24, 16, 24});
    static game::ui_window other(game::host::window, L"Other Window", { 0, 120 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other1(game::host::window, L"Other Window", { 100, 150 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other2(game::host::window, L"Other Window", { 200, 170 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other3(game::host::window, L"Other Window", { 300, 190 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other4(game::host::window, L"Other Window", { 400, 220 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other5(game::host::window, L"Other Window", { 400, 250 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other6(game::host::window, L"Other Window", { 500, 270 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });
    static game::ui_window other7(game::host::window, L"Other Window", { 600, 290 }, { 250, 150 }, { 0xe0, 0xe0, 0xe0, 255 }, { 0xb7, 0x1c, 0x1c, 0xff }, { 16, 24, 16, 24 });

    static int a = [&]() {
        settings.set_close_icon(close_icon);
        settings.set_minimize_icon(minimize_icon);
        settings.set_maximize_icon(maximize_icon);
        settings.set_status(L"Status: OK");
        settings.set_icon(window_icon);

        #define setotherthing(x) x.set_close_icon(close_icon);\
        x.set_minimize_icon(minimize_icon);\
        x.set_maximize_icon(maximize_icon);\
        x.set_status(L"Status: Different");\
        x.set_icon(window_icon);

        setotherthing(other);
        setotherthing(other1);
        setotherthing(other2);
        setotherthing(other3);
        setotherthing(other4);
        setotherthing(other5);
        setotherthing(other6);
        setotherthing(other7);
        return 0;
    }();
    static game::ui_button btn(close_icon, { 0x00, 0x68, 0x3b, 255 });
    settings.draw_content(layouter, title, fnt, [&](game::ui_window& window, const glm::vec2& bmin, const glm::vec2& bmax) {
        window.next_no_inset();
        window.draw_image(image, { window.content_size(false).x, 120 });
        window.draw_text(L"General Info", title, game::ui::ALIGN_CENTER, { 0, 0, 0, 255 }, 1);
        window.spacing();
        window.draw_text(L"Delta Time: ", fnt, game::ui::ALIGN_LEFT, { 0, 0, 0, 255 }, 1);
        window.next_same_line();
        window.draw_text(std::to_wstring(game::host::window->delta_time()), fnt, game::ui::ALIGN_RIGHT, { 0x00, 0x68, 0x3b, 255 }, 1);
        window.draw_text(L"Frames per second: ", fnt, game::ui::ALIGN_LEFT, { 0, 0, 0, 255 }, 1);
        window.next_same_line();
        window.draw_text(std::to_wstring(int(1.0 / game::host::window->delta_time())), fnt, game::ui::ALIGN_RIGHT, { 0x00, 0x68, 0x3b, 255 }, 1);
        if (window.draw_button(btn, { 24, 24 }) == game::STATE_RELEASE)
        {
            log_h << "Clicked button.";
        }
        window.spacing();
        window.draw_text(L"Situation", title, game::ui::ALIGN_CENTER, { 0, 0, 0, 255 }, 1);
        window.draw_text(txt, fnt, game::ui::ALIGN_JUSTIFY, {0, 0, 0, 128});

        window.draw_text(L"[Left]\nI am a weird text", fnt, game::ui::ALIGN_LEFT, { 0, 0, 0, 255 });
        window.draw_text(L"[Right]\nNew Line with alpha.", fnt, game::ui::ALIGN_RIGHT, { 0, 0, 0, 128 });
        window.draw_text(L"[Center]\nThis is another kind of pretty long text to test line breaking and vertical offset characteristics. Also it has a different color.", fnt, game::ui::ALIGN_CENTER, { 128, 0, 0, 255 });
        window.draw_text(L"[Justify Center]\nEr hörte leise Schritte hinter sich. Das bedeutete nichts Gutes. Wer würde ihm schon folgen, spät in der Nacht und dazu noch in dieser engen Gasse mitten im übel beleumundeten Hafenviertel? Gerade jetzt, wo er das Ding seines Lebens gedreht hatte und mit der Beute verschwinden wollte! Hatte einer seiner zahllosen Kollegen dieselbe Idee gehabt, ihn beobachtet und abgewartet, um ihn nun um die Früchte seiner Arbeit zu erleichtern?", fnt, game::ui::ALIGN_JUSTIFY | game::ui::ALIGN_CENTER, { 0, 0, 128, 255 });
        window.draw_text(L"[Center]\nFinally, a green one is missing. ÖÄÜß", fnt, game::ui::ALIGN_CENTER, { 0, 128, 0, 255 });
    });

    static const auto empty_window_content = [&](game::ui_window& window, const glm::vec2& bmin, const glm::vec2& bmax) {
        window.draw_text(L"Empty Window", fnt, game::ui::ALIGN_CENTER, { 0, 0, 0, 255 });
    };

    other.draw_content(layouter, title, fnt, empty_window_content);
    other1.draw_content(layouter, title, fnt, empty_window_content);
    other2.draw_content(layouter, title, fnt, empty_window_content);
    other3.draw_content(layouter, title, fnt, empty_window_content);
    other4.draw_content(layouter, title, fnt, empty_window_content);
    other5.draw_content(layouter, title, fnt, empty_window_content);
    other6.draw_content(layouter, title, fnt, empty_window_content);
    other7.draw_content(layouter, title, fnt, empty_window_content);

    game::default_ui().draw();

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


using T = int;
struct custom_deleter { void operator()(T* t) const noexcept { free(t); } };

// Default Constr.: depends
// Copyable: depends
// Immer erste zu bedenkende alternative, weil schnell und sicher.
T                                   stack_value;

// Default Constr.: true
// Copyable: true
// Erstellen mit T* x = new T(...), manuelles löschen mit delete x; erforderlich, sonst MEMORY LEAK!!! und memory leak = böse.
T*                                  raw_pointer;

// - Threadsafe kopierbar
// - Geteilter referenz-zähler zwischen allen kopien
// -> Zähler bei kopie inkrementiert, bei destruktion dekrementiert.
// -> Pointer wird gelöscht, sobald Zähler auf 0.
// - konvertierbar aus temporären std::unique_ptr<T>&&. -> Factory funktionen!
// - Größen- und kopier-overhead vorhanden!
// Eigene deleter nur über zusätzlichen konstruktorparameter möglich.
// kann über dynamic_pointer_cast zwischen polymorphen klassen casten. Auch static_pointer_cast, wenn man sich ganz sicher ist.
std::shared_ptr<T>                  shared_pointer;
std::weak_ptr<T>                    weak_pointer;
// Beispiel shared_ + weak_ptr: baumstruktur...

// - Nicht kopierbar (aber "Movable")
// - Zerstört enthaltenen Ptr wenn unique_ptr zerstört wird.
// -> Normalfall : delete ptr; -> make_unique zum erstellen
// -> Eigene funktion, dann jedoch mehr speicher benötigt, und muss jedes mal wenn konstruiert die funktion als letzten parameter haben. Kein make_unique möglich.
// -> Eigene deleter class mit überschriebenen operator()(T* t). Kein make_unique möglich
// Beispiele für nutzbarkeit von unique_ptr<T[], custom_deleter> zB stb_image oder anderen C-library daten pointern die entsprechend nicht über new sondern malloc erzeugt und somit über free(x) gelöscht werden müssen.
std::unique_ptr<T>                  unique_pointer;
std::unique_ptr<T, void(*)(void*)>  unique_pointer_with_custom_deleter_function{nullptr, &free}; // muss mit deleter funktion initialisiert werden. Allgemein recht doof.
std::unique_ptr<T, custom_deleter>  unique_pointer_with_custom_deleter_class;

// Kann auch anstelle von new T[x] verwendet werden:
size_t x = 5;
std::unique_ptr<T[]>                unique_array_pointer = std::make_unique<T[]>(5);


//Entscheidungskette:
// 1) default konstruierbar, konstruktor-parameter direkt oder (wenn member) in der parent-klasse im konstruktor setzbar?
// 2) oder: move-assignable und nur an der einen stelle gebraucht?
// 3) oder: temporär benötigt innerhalb einer funktion?
//  -> Stack value

// 3.1) Man will das objekt verändern ohne es ständig hin- und her zu kopieren (zB in einem loop: kopier raus, veränder, kopier wieder rein)
// 3.2) oder: man will ein objekt an eine funktion übergeben das größer als 8 bytes ist (int=4bytes, int64_t=8bytes, T* = 8bytes), welches darin aber entweder direkt oder garnicht verändert wird (oder wenn es weg kopiert wird).
// -> (const) referenz (oder std::reference_wrapper wenns kopierbar sein soll)

// 3) Nicht default konstruierbar, nicht kopierbar und im späteren verlauf erst zugewiesen/zuweisbar? (zB globale auf validen opengl kontext angewiesene objekte)
// 4) Factory funktion?
// 5) simple C-datenwrapper?
//  -> Unique ptr

// 6) objekt kann von mehreren objekten gleichzeitig zwischengespeichert werden, und jedem gehört das objekt so ein bisschen.
// 6.5) z.B. framebuffer-texturen oder shader stages in einer pipeline
// -> shared_ptr

// 8) Man speichert sich den pointer temporär zwischen, einem gehört das objekt aber nicht, jedoch möchte man trotzdem wissen, ob es zerstört wurde
// -> weak ptr

// 9) man braucht die eigenschaften eines weak_ptr's, aber auch performance und einem sind speicherzugriffsfehler egal oder man weiß dass der pointer für die dauer der nutzung valide bleibt?
// 10) oder: man will den ptr irgendwo zwischenspeichern und die lebenszeit des pointers ist gemanaged (stack-objekt oder smart-ptr) und sicher länger als die nutzung des zwischengespeicherten objekts, man hat jedoch keine lust
// std::reference_wrapper zu schreiben.
// -> T*

// shared_ptr/weak_ptr gut um flexible baumstrukturen zu bekommen.
struct tree
{
    //  parent muss weak_ptr sein, da es nur einen shared_ptr referenziert, dieser aber nicht wirklich zu ihm gehört.
    // wäre dies ein shared_ptr, zeigt der node auf auf seinen parent und der wiederum auf den node selber. Damit können die pointer niemals automatisch gelöscht werden (müsste manuell gemacht werden).
    std::weak_ptr<tree> parent;
    std::shared_ptr<tree> left;
    std::shared_ptr<tree> right;
};

// Factory funktionen
template<typename T>
std::unique_ptr<T> create_default_object()
{
    return std::make_unique<T>();
}

// custom deleter an stb_image
struct free_deleter { void operator()(void* v) { free(v); } };
auto stb_load_image(const std::string& path, int& w, int&h)
{
    return std::unique_ptr<uint8_t[], free_deleter>(stbi_load(path.c_str(), &w, &h, nullptr, 4));
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