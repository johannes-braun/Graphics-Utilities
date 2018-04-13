#pragma once

#include "imgui_orient.h"
#include <string>

namespace ImGui
{
    inline void Title(const std::string& title, const int font = 0)
    {
        PushFont(ImGui::GetIO().Fonts->Fonts[font]);
        Text("%s", title.c_str());
        PopFont();
    }
}
