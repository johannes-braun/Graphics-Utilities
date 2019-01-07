#include "opengl/shader.hpp"
#include <gfx.core/log.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
shader::shader(gsl::span<u32 const> source) : _module(mygl::shader::zero())
{
    load(source);
}

shader::~shader()
{
    if (glIsShader(_module)) glDeleteShader(_module);
}

const mygl::shader& shader::get_module() const noexcept
{
    return _module;
}

void shader::load(gsl::span<u32 const> source)
{
    _module = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &_module, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), gfx::u32(source.size() * sizeof(uint32_t)));
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx