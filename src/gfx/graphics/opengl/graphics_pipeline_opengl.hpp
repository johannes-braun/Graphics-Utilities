#pragma once

#include "../graphics_pipeline.hpp"
#include <mygl/mygl.hpp>
#include "../general/handle.hpp"

namespace gfx
{
    namespace v2
    {
        namespace opengl
        {
			struct shader_module
			{
				movable_handle<mygl::shader_program> s;
				~shader_module() { glDeleteProgram(s); }
			};

			class graphics_pipeline_implementation : public detail::graphics_pipeline_implementation
			{
			public:
				~graphics_pipeline_implementation();

			    void initialize(const pipeline_state& state, const renderpass_layout& renderpass, span<const v1::shader* const> shaders) override;
			    handle api_handle() override;
                
			private:
				topology _prim_topology = topology::triangle_list;
				movable_handle<mygl::vertex_array> _vao; 
				std::unordered_map<uint32_t, size_t> _binding_strides;
				std::vector<std::function<void()>> _apply_state;
				movable_handle<mygl::pipeline> _pipeline;
				std::vector<shader_module>     _shds;
			};
        }
    }
}