#pragma once

#include <mutex>

namespace ImGui
{
struct tab_state
{
    struct info
    {
        std::string str;
        bool        init = false;
    };

    const std::string*                    current;
    std::unordered_map<std::string, info> tab_states;
};

inline tab_state& _state()
{
    static tab_state s;
    return s;
}

auto BeginTabbed(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0)
{
    _state().tab_states[name];
    const auto it    = _state().tab_states.find(name);
    _state().current = &it->first;

    return ImGui::Begin(name, p_open, flags | ImGuiWindowFlags_MenuBar);
}

auto Tab(const char* name)
{
    const auto it = _state().tab_states.find(*_state().current);
    assert(it != _state().tab_states.end());

	if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu(name, it->second.str != name)) {
            it->second.str = name;
            ImGui::CloseCurrentPopup();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (!it->second.init) {
        it->second.str  = name;
        it->second.init = true;
        return true;
    }

    return it->second.str == name;
}
}