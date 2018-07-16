#pragma once

#include "state_info.hpp"
#include "vertex_input.hpp"
#include <fstream>
#include <gfx/context.hpp>
#include <gfx/log.hpp>
#include <iterator>
#include <memory>

#include <gfx/graphics/shader_include.hpp>

#include <glsp/glsp.hpp>

namespace gfx
{
enum class shader_type
{
    vert,
    frag,
    geom,
    tese,
    tesc,
    comp
};

enum class shader_format
{
    spirv,
    text
};

class shader;

namespace detail
{
template<typename T>
std::shared_ptr<std::decay_t<T>> fwd_create(T&& t)
{
    return std::make_shared<std::decay_t<T>>(std::forward<T&&>(t));
}

class graphics_pipeline_implementation
{
public:
    virtual ~graphics_pipeline_implementation() = default;
    virtual bool bind(vertex_input& input, state_info& info, const std::vector<std::pair<shader_type, std::shared_ptr<shader>>>& shaders,
                      bool shaders_valid)       = 0;
};

std::unique_ptr<graphics_pipeline_implementation> make_graphics_pipeline_implementation();
}    // namespace detail

class graphics_pipeline
{
public:
    graphics_pipeline() : graphics_pipeline(vertex_input {}, state_info {}) {}
    graphics_pipeline(std::shared_ptr<vertex_input> input, state_info&& state)
          : graphics_pipeline(input, detail::fwd_create(std::forward<state_info&&>(state)))
    {}
    graphics_pipeline(vertex_input&& input, state_info&& state)
          : graphics_pipeline(detail::fwd_create(std::forward<vertex_input&&>(input)),
                              detail::fwd_create(std::forward<state_info&&>(state)))
    {}
    graphics_pipeline(vertex_input&& input, std::shared_ptr<state_info> state)
          : graphics_pipeline(detail::fwd_create(std::forward<vertex_input&&>(input)), state)
    {}
    graphics_pipeline(std::shared_ptr<vertex_input> input, std::shared_ptr<state_info> state)
          : _implementation(detail::make_graphics_pipeline_implementation()), _vertex_input(input), _state(state)
    {}

    void attach(shader_type t, shader&& s)
    {
        _shaders.emplace_back(t, std::make_shared<shader>(s));
        _shaders_valid = false;
    }
    void attach(shader_type t, std::shared_ptr<shader> s)
    {
        _shaders.emplace_back(t, std::move(s));
        _shaders_valid = false;
    }

    void bind() { _shaders_valid = _implementation->bind(*_vertex_input, *_state, _shaders, _shaders_valid); }

    const vertex_input& input() const noexcept { return *_vertex_input; }
    const state_info&   state() const noexcept { return *_state; }

private:
    bool                                                         _shaders_valid = false;
    std::shared_ptr<vertex_input>                                _vertex_input  = nullptr;
    std::shared_ptr<state_info>                                  _state         = nullptr;
    std::vector<std::pair<shader_type, std::shared_ptr<shader>>> _shaders;
    std::unique_ptr<detail::graphics_pipeline_implementation>    _implementation;
};

class shader
{
public:
    shader(shader_format fmt, const std::filesystem::path& path) : _format(fmt), _path(path)
    {
        if (context::current()->options().graphics_api != gapi::opengl && fmt == shader_format::text)
        {
            elog << "Invalid configuration: Trying to create an opengl-format shader with a non-opengl context.";
            return;
        }

        bool path_valid = _path.is_absolute();
        if (!path_valid)
            for (auto&& inc : shader_includes::directories)
                if (exists(inc / _path))
                {
                    _path      = inc / _path;
                    path_valid = true;
                    break;
                }
        if (!path_valid)
        {
            elog << "Shader not found: " << path;
            return;
        }

        switch (_format)
        {
        case shader_format::text:
        {
            std::vector<glsp::files::path> includes;
            for (const auto& p : shader_includes::directories) includes.push_back(p.string());
            const auto ppf = glsp::preprocess_file(_path.string(), includes);
            _data.resize(ppf.contents.size() + 1);
            memcpy(_data.data(), ppf.contents.data(), ppf.contents.size() * sizeof(char));
            _data.back() = std::byte(0);
        }
        break;
        case shader_format::spirv:
        {
            std::ifstream in(_path, std::ios::binary | std::ios::in);
            if (in)
            {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);
                _data.resize(size);
                in.read(reinterpret_cast<char*>(_data.data()), size);
            }
        }
        break;
        }
    }

    const std::vector<std::byte>& data() const noexcept { return _data; }
    shader_format                 format() const noexcept { return _format; }

private:
    shader_format          _format;
    std::filesystem::path  _path;
    std::vector<std::byte> _data {};    // source string or binary; empty on failure
};
}    // namespace gfx
