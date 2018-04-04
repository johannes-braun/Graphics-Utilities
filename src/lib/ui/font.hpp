#pragma once

#include <experimental/filesystem>
#include <opengl/texture.hpp>
#include "stb_truetype.h"

namespace gfx::ui
{
    namespace files = std::experimental::filesystem;
    struct glyph
    {
        float x0, y0, s0, t0; // top-left
        float x1, y1, s1, t1; // bottom-right
        float offx = 0, offy = 0;
    };
    struct font
    {
    private:
        struct line_info
        {
            float width;
            wchar_t delim;
            int count;
        };

    public:
        constexpr static wchar_t first_char = L' ';
        font(const files::path& ttf, float size, int atlas_dimension = 1024);

        glyph at(wchar_t c);
        const glyph at(wchar_t c) const;
        glyph operator[](wchar_t c);
        const glyph operator[](wchar_t c) const;
        float size() const noexcept;
        const gl::texture& get_atlas() const noexcept;

        line_info line_width(const wchar_t* begin, float max_width) const noexcept;

    private:
        static constexpr bool may_delim_line(const wchar_t* c) noexcept
        {
            return *c==' ' || *c=='-';
        }

        float _size;
        int _atlas_dimension;
        gl::texture _atlas{ GL_TEXTURE_2D };
        std::vector<stbtt_packedchar> _characters;
    };
}