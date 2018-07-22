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

namespace gfx {
inline namespace v1 {
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

namespace detail {
template<typename T>
std::shared_ptr<std::decay_t<T>> fwd_create(T&& t)
{
    return std::make_shared<std::decay_t<T>>(std::forward<T&&>(t));
}

class graphics_pipeline_implementation
{
public:
    static std::unique_ptr<graphics_pipeline_implementation> make();

    virtual ~graphics_pipeline_implementation() = default;
    virtual bool bind(vertex_input& input, state_info& info, const std::vector<std::pair<shader_type, std::shared_ptr<shader>>>& shaders,
                      bool shaders_valid)       = 0;

    virtual std::any api_handle() = 0;
};

class compute_pipeline_implementation
{
public:
	static std::unique_ptr<compute_pipeline_implementation> make();

	virtual ~compute_pipeline_implementation() = default;
	virtual bool build(shader& shaders)       = 0;

	virtual std::any api_handle() = 0;
};
}    // namespace detail

class graphics_pipeline : public impl::implements<detail::graphics_pipeline_implementation>
{
public:
    graphics_pipeline();
    graphics_pipeline(std::shared_ptr<vertex_input> input, state_info&& state);
    graphics_pipeline(vertex_input&& input, state_info&& state);
    graphics_pipeline(vertex_input&& input, std::shared_ptr<state_info> state);
    graphics_pipeline(std::shared_ptr<vertex_input> input, std::shared_ptr<state_info> state);

    void attach(shader_type t, shader&& s);
    void attach(shader_type t, std::shared_ptr<shader> s);

    void bind();

    const vertex_input& input() const noexcept;
    const state_info&   state() const noexcept;

private:
    bool                                                         _shaders_valid = false;
    std::shared_ptr<vertex_input>                                _vertex_input  = nullptr;
    std::shared_ptr<state_info>                                  _state         = nullptr;
    std::vector<std::pair<shader_type, std::shared_ptr<shader>>> _shaders;
};

class compute_pipeline : public impl::implements<detail::compute_pipeline_implementation>
{
public:
    compute_pipeline(shader&& input);
	compute_pipeline(const std::shared_ptr<shader>& input);

private:
	std::shared_ptr<shader> _shader;
};

class shader
{
public:
    shader(shader_format fmt, const std::filesystem::path& path);

    const std::vector<std::byte>& data() const noexcept;
    shader_format                 format() const noexcept;

private:
    shader_format          _format;
    std::filesystem::path  _path;
    std::vector<std::byte> _data{};    // source string or binary; empty on failure
};
}    // namespace v1
}    // namespace gfx
