#include "font.hpp"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <fstream>
#include <jpu/log.hpp>

namespace gfx::ui
{
    font::font(const files::path& ttf, float size, int atlas_dimension)
        : _size(size), _atlas_dimension(atlas_dimension)
    {
        std::ifstream font_file(ttf, std::ios::binary);
        font_file.seekg(0, std::ios::end);
        size_t len = font_file.tellg();
        font_file.seekg(0, std::ios::beg);
        std::vector<uint8_t> data(len);
        _characters.resize(512);
        font_file.read(reinterpret_cast<char*>(data.data()), len);

        std::vector<uint8_t> tex_data(atlas_dimension * atlas_dimension);

        struct pack_end { void operator()(stbtt_pack_context* c) { stbtt_PackEnd(c); } };
        std::unique_ptr<stbtt_pack_context, pack_end> pctx(new stbtt_pack_context());
        if (!stbtt_PackBegin(pctx.get(), tex_data.data(), atlas_dimension, atlas_dimension, 0, 1, nullptr))
        {
            log_e << "Failed to pack font atlas.";
            return;
        }
        stbtt_PackSetOversampling(pctx.get(), 2, 2);
        if (!stbtt_PackFontRange(pctx.get(), data.data(), 0, size, first_char, 512, _characters.data()))
        {
            log_e << "Failed to pack font atlas.";
            return;
        }

        std::vector<glm::u8vec4> rgba(_atlas_dimension * _atlas_dimension);
        for (int i=0; i<rgba.size(); ++i)
            rgba[i] = glm::u8vec4{ 255, 255, 255, tex_data[i] };
        _atlas = gl::texture(GL_TEXTURE_2D, atlas_dimension, atlas_dimension, GL_RGBA8, 1);
        _atlas.assign(GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }

    glyph font::at(wchar_t c) {
        glyph g;
        stbtt_GetPackedQuad(_characters.data(), _atlas_dimension, _atlas_dimension, c - first_char, &g.offx, &g.offy, reinterpret_cast<stbtt_aligned_quad*>(&g), 1);
        g.y1 = -g.y1;
        g.y0 = -g.y0;
        return g;
    }

    const glyph font::at(wchar_t c) const 
    {
        return const_cast<font*>(this)->at(c);
    }

    glyph font::operator[](wchar_t c)
    { 
        return at(c); 
    }

    const glyph font::operator[](wchar_t c) const 
    {
        return at(c);
    }

    float font::size() const noexcept
    {
        return _size;
    }

    const gl::texture& font::get_atlas() const noexcept
    {
        return _atlas;
    }

    std::wstring font::ellipsize_end(const std::wstring& in, float max_width) const
    {
        max_width -= 3 * at('.').offx;
        if (max_width < 0)
            return L"";
        size_t len = 0;
        for (auto it = in.begin(); it != in.end(); ++it)
        {
            if (*it > first_char)
            {
                const glyph g = at(*it);
                max_width -= g.offx;
                if (max_width < 0)
                {
                    std::wstring result(in.begin(), in.begin() + len);
                    result.insert(result.end(), '.');
                    result.insert(result.end(), '.');
                    result.insert(result.end(), '.');
                    return result;
                }
            }
            ++len;
        }
        return in;
    }

    font::line_info font::line_width(const wchar_t* begin, float max_width) const noexcept
    {
        line_info info;
        info.width = 0;
        info.delim = L'\0';
        info.count = 0;

        line_info at_last_delim = info;

        while (true)
        {
            if (*begin == '\0')
            {
                info.delim = L'\0';
                return info;
            }
            if (*begin == '\n')
            {
                info.delim = L'\n';
                return info;
            }
            if (may_delim_line(begin))
            {
                at_last_delim.delim = *begin;
                at_last_delim.count = info.count;
                at_last_delim.width = info.width;
            }

            const glyph g = at(*begin);
            info.width += g.offx;
            if (info.width > max_width)
                return at_last_delim;

            ++info.count;
            ++begin;
        }
    }
}