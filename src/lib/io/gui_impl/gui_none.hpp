#pragma once
#include "imgui/imgui.h"

namespace io::impl
{
    class gui_none
    {
    public:
        ImTextureID build_font_atlas() { return nullptr;  }
        void pre_render(ImDrawData* draw_data) {}
        void render(const ImDrawCmd& cmd, int index_offset, int vertex_offset) const {}
        void post_render() {}
    };
}
