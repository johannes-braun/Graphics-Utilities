#pragma once
#define GFX_EXPOSE_APIS
#include "state_info.hpp"
#include <gfx/graphics/pipeline.hpp>
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
inline GLbitfield stage_bitfield(GLenum st) noexcept
{
    switch (st)
    {
    case GL_VERTEX_SHADER: return GL_VERTEX_SHADER_BIT;
    case GL_FRAGMENT_SHADER: return GL_FRAGMENT_SHADER_BIT;
    case GL_GEOMETRY_SHADER: return GL_GEOMETRY_SHADER_BIT;
    case GL_TESS_CONTROL_SHADER: return GL_TESS_CONTROL_SHADER_BIT;
    case GL_TESS_EVALUATION_SHADER: return GL_TESS_EVALUATION_SHADER_BIT;
    case GL_COMPUTE_SHADER: return GL_COMPUTE_SHADER_BIT;
    default: return GL_ZERO;
    }
}

struct shd
{
	mygl::shader_program s;
	~shd()
	{
		if (glIsProgram(s)) glDeleteProgram(s);
	}
};

inline auto make_shader(shader_type t, shader& s, mygl::pipeline handle) {
	const auto ty = [&]() {
		switch (t)
		{
		case shader_type::vert: return GL_VERTEX_SHADER;
		case shader_type::frag: return GL_FRAGMENT_SHADER;
		case shader_type::comp: return GL_COMPUTE_SHADER;
		case shader_type::geom: return GL_GEOMETRY_SHADER;
		case shader_type::tesc: return GL_TESS_CONTROL_SHADER;
		case shader_type::tese: return GL_TESS_EVALUATION_SHADER;
		}
		return GLenum(0);
	}();
	if (s.format() == shader_format::text) {
		const char* src[2] = {reinterpret_cast<const char*>(s.data().data()), "\n"};
		shd         temp{glCreateShaderProgramv(ty, 2, src)};

		int success = 0;
		glGetProgramiv(temp.s, GL_LINK_STATUS, &success);
		if (!success) {
			int log_length;
			glGetProgramiv(temp.s, GL_INFO_LOG_LENGTH, &log_length);
			std::string log(log_length, ' ');
			glGetProgramInfoLog(temp.s, log_length, &log_length, log.data());
			glDeleteProgram(temp.s);
			elog("shader") << log;
			return shd{mygl::shader_program::zero};
		}

		glUseProgramStages(handle, stage_bitfield(ty), temp.s);
		return temp;
	}
	else if (s.format() == shader_format::spirv)
	{
		mygl::shader cs = glCreateShader(ty);
		glShaderBinary(1, &cs, GL_SHADER_BINARY_FORMAT_SPIR_V, s.data().data(), static_cast<int>(s.data().size()));
		glSpecializeShader(cs, "main", 0, nullptr, nullptr);
		shd prog{glCreateProgram()};
		glProgramParameteri(prog.s, GL_PROGRAM_SEPARABLE, true);
		glAttachShader(prog.s, cs);
		glLinkProgram(prog.s);
		glDetachShader(prog.s, cs);
		glDeleteShader(cs);
		int success = 0;
		glGetProgramiv(prog.s, GL_LINK_STATUS, &success);
		if (!success) {
			int log_length;
			glGetProgramiv(prog.s, GL_INFO_LOG_LENGTH, &log_length);
			std::string log(log_length, ' ');
			glGetProgramInfoLog(prog.s, log_length, &log_length, log.data());
			glDeleteProgram(prog.s);
			elog("shader") << log;
			return shd{mygl::shader_program::zero};
		}
		glUseProgramStages(handle, stage_bitfield(ty), prog.s);
		return prog;
	}
	return shd{mygl::shader_program::zero};
};

bool validate(mygl::pipeline handle)
{
	glValidateProgramPipeline(handle);

	int success = 0;
	glGetProgramPipelineiv(handle, GL_VALIDATE_STATUS, &success);
	if (!success) {
		int log_length;
		glGetProgramPipelineiv(handle, GL_INFO_LOG_LENGTH, &log_length);
		std::string log(log_length, ' ');
		glGetProgramPipelineInfoLog(handle, log_length, &log_length, log.data());
		elog("shader") << log;
		return false;
	}
	return true;
}

class graphics_pipeline_implementation : public detail::graphics_pipeline_implementation
{
public:
    ~graphics_pipeline_implementation()
    {
        if (glIsProgramPipeline(_handle)) glDeleteProgramPipelines(1, &_handle);
    }

    bool bind(vertex_input& input, state_info& info, const std::vector<std::pair<shader_type, std::shared_ptr<shader>>>& shaders,
              bool shaders_valid) override
    {
        if (_handle == mygl::pipeline::zero) {
            glCreateProgramPipelines(1, &_handle);
        }

        if (!shaders_valid) {
            ilog << "Recompiling shaders...";

            shds.resize(shaders.size());
            for (int i = 0; i < shds.size(); ++i)
                if ((shds[i] = make_shader(shaders[i].first, *(shaders[i].second), _handle)).s == mygl::shader_program::zero) return false;

			if (!validate(_handle)) return false;
        }

        glBindVertexArray(handle_cast<mygl::vertex_array>(input));
        apply(info);
        glBindProgramPipeline(_handle);
        return true;
    }

    std::any api_handle() override
	{
		return _handle;
	}

private:
    std::vector<shd> shds;
    mygl::pipeline   _handle = mygl::pipeline::zero;
};


class compute_pipeline_implementation : public detail::compute_pipeline_implementation
{
public:
    bool build(shader& shaders) override
    {
		if (_handle == mygl::pipeline::zero) {
			glCreateProgramPipelines(1, &_handle);
		}
		_s = make_shader(shader_type::comp, shaders, _handle);
		if (!validate(_handle)) return false;
		return true;
    }
    std::any api_handle() override
    {
		return _handle;
    }

private:
	shd _s = {mygl::shader_program::zero};
	mygl::pipeline   _handle = mygl::pipeline::zero;
};

}    // namespace opengl
}    // namespace v1
}    // namespace gfx