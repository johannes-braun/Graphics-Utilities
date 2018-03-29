#include "host.hpp"

namespace game::host
{
    std::unique_ptr<io::window> window;
    std::map<int, std::function<bool()>> state_loops;

    int _current_state = 0;

    struct window_settings
    {
        std::string title;
        int width, height;
        bool fullscreen;
        int msaa_samples;
    };

    window_settings load_settings(const files::path& settings)
    {
        pugi::xml_document sfile; sfile.load_file(settings.c_str());

        const pugi::xml_node settings_root_node = sfile.child("settings");
        const pugi::xml_node window_node = settings_root_node.child("window");
        const pugi::xml_node window_size_node = window_node.child("size");
        const pugi::xml_node window_msaa_node = window_node.child("multisample");

        window_settings result;
        result.title = window_node.attribute("title").as_string();
        result.width = window_size_node.attribute("width").as_int(800);
        result.height = window_size_node.attribute("height").as_int(600);
        result.fullscreen = window_size_node.attribute("fullscreen").as_bool(false);
        result.msaa_samples = window_msaa_node.attribute("count").as_int(1);

        return result;
    }

    bool init(const files::path& settings)
    {
        const window_settings ws = load_settings(settings);
        glfwWindowHint(GLFW_SAMPLES, ws.msaa_samples);
#if not defined(NDEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
        window = std::make_unique<io::window>(io::api::opengl, ws.width, ws.height, ws.title);

        return true;
    }

    void set_state(int state)
    {
        _current_state = state;
    }

    bool do_loop()
    {
        bool updated = window->update();
        if (updated && state_loops.count(_current_state) != 0)
            return state_loops.at(_current_state)();
        return false;
    }
}