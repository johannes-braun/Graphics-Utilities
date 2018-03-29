#pragma once

#include <io/window.hpp>
#include <pugixml.hpp>

#include <filesystem>

namespace game
{
    namespace files = std::experimental::filesystem;
}

namespace game::host
{
    extern std::unique_ptr<io::window> window;
    extern std::map<int, std::function<bool()>> state_loops;

    bool init(const files::path& settings);
    void set_state(int state);
    bool do_loop();
}