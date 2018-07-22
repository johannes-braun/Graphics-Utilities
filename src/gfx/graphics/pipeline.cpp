#include "opengl/pipeline_opengl.hpp"
#include "pipeline.hpp"

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::graphics_pipeline_implementation> detail::graphics_pipeline_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::graphics_pipeline_implementation>();
    case gapi::vulkan:
    default: break;
    }
    return nullptr;
}

std::unique_ptr<detail::compute_pipeline_implementation> detail::compute_pipeline_implementation::make()
{
	switch (context::current()->options().graphics_api)
	{
	case gapi::opengl: return std::make_unique<opengl::compute_pipeline_implementation>();
	case gapi::vulkan:
	default: break;
	}
	return nullptr;
}

graphics_pipeline::graphics_pipeline() : graphics_pipeline(vertex_input{}, state_info{}) {}

graphics_pipeline::graphics_pipeline(std::shared_ptr<vertex_input> input, state_info&& state)
      : graphics_pipeline(input, detail::fwd_create(std::forward<state_info&&>(state)))
{}

graphics_pipeline::graphics_pipeline(vertex_input&& input, state_info&& state)
      : graphics_pipeline(detail::fwd_create(std::forward<vertex_input&&>(input)), detail::fwd_create(std::forward<state_info&&>(state)))
{}

graphics_pipeline::graphics_pipeline(vertex_input&& input, std::shared_ptr<state_info> state)
      : graphics_pipeline(detail::fwd_create(std::forward<vertex_input&&>(input)), state)
{}

graphics_pipeline::graphics_pipeline(std::shared_ptr<vertex_input> input, std::shared_ptr<state_info> state)
      : _vertex_input(input), _state(state)
{}

void graphics_pipeline::attach(shader_type t, shader&& s)
{
    _shaders.emplace_back(t, std::make_shared<shader>(s));
    _shaders_valid = false;
}

void graphics_pipeline::attach(shader_type t, std::shared_ptr<shader> s)
{
    _shaders.emplace_back(t, std::move(s));
    _shaders_valid = false;
}

void graphics_pipeline::bind()
{
    _shaders_valid = implementation()->bind(*_vertex_input, *_state, _shaders, _shaders_valid);
}

const vertex_input& graphics_pipeline::input() const noexcept
{
    return *_vertex_input;
}

const state_info& graphics_pipeline::state() const noexcept
{
    return *_state;
}

compute_pipeline::compute_pipeline(shader&& input)
    : _shader(std::make_shared<shader>(std::forward<shader>(input)))
{
	implementation()->build(*_shader);
}

compute_pipeline::compute_pipeline(const std::shared_ptr<shader>& input)
	: _shader(input)
{
	implementation()->build(*_shader);
}

shader::shader(shader_format fmt, const std::filesystem::path& path) : _format(fmt), _path(path)
{
    if (context::current()->options().graphics_api != gapi::opengl && fmt == shader_format::text) {
        elog << "Invalid configuration: Trying to create an opengl-format shader with a non-opengl context.";
        return;
    }

    bool path_valid = _path.is_absolute();
    if (!path_valid)
        for (auto&& inc : shader_includes::directories)
            if (exists(inc / _path)) {
                _path      = inc / _path;
                path_valid = true;
                break;
            }
    if (!path_valid) {
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
        if (in) {
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

const std::vector<std::byte>& shader::data() const noexcept
{
    return _data;
}

shader_format shader::format() const noexcept
{
    return _format;
}

}    // namespace v1
}    // namespace gfx
