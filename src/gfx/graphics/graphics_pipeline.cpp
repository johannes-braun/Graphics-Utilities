#include "graphics_pipeline.hpp"
#include "../context.hpp"
#include "opengl/graphics_pipeline_opengl.hpp"
#include "vulkan/graphics_pipeline_vulkan.hpp"

namespace gfx
{
    namespace v2
    {
    std::unique_ptr<detail::graphics_pipeline_implementation> detail::graphics_pipeline_implementation::make()
    {
        switch(context::current()->options().graphics_api)
        {
		case gapi::opengl: return std::make_unique<opengl::graphics_pipeline_implementation>();
		case gapi::vulkan: return std::make_unique<vulkan::graphics_pipeline_implementation>();
        default: ;
        }
		return nullptr;
    }

	graphics_pipeline::graphics_pipeline(const pipeline_state& state, const renderpass_layout& renderpass, std::initializer_list<v1::shader> shaders)
	{
		std::vector<const v1::shader*> shs;
		for (auto& s : shaders)
			shs.push_back(&s);
		implementation()->initialize(state, renderpass, shs);
	}

	graphics_pipeline::graphics_pipeline(const pipeline_state& state, const renderpass_layout& renderpass, std::initializer_list<const v1::shader* const> shaders)
		: graphics_pipeline(state, renderpass, span<const v1::shader* const>(std::data(shaders), std::size(shaders)))
	{

	}

    graphics_pipeline::graphics_pipeline(const pipeline_state& state, const renderpass_layout& renderpass, span<const v1::shader* const> shaders)
    {
        implementation()->initialize(state, renderpass, shaders);
    }
    }
}