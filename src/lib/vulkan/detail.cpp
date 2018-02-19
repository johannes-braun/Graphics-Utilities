
#include "detail.hpp"
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace vkn::detail
{
	const std::vector<const char*>& instance_extensions()
	{
		static auto extension_loader = [extensions = []() {
			uint32_t count;
			const auto ext = glfwGetRequiredInstanceExtensions(&count);
			return std::vector<const char*>(ext, ext + count);
		}(), properties = vk::enumerateInstanceExtensionProperties()]() mutable {
			return [&extensions, &properties]() {
				for (auto&& prop : properties) extensions.push_back(prop.extensionName);
				return extensions;
			}();
		};
		static std::vector<const char*> loaded_extensions = extension_loader();
		return loaded_extensions;
	}

	const std::vector<const char*>& instance_layers()
	{
		static auto layer_loader = [active_layers = std::vector<const char*>()]() mutable {
			auto layers = vk::enumerateInstanceLayerProperties();
			for (auto&& layer : detail::layers)
			{
				if (std::find_if(layers.begin(), layers.end(), [&layer](const vk::LayerProperties& props) {
					return std::string_view(layer) == props.layerName;
				}) != layers.end())
				{
					active_layers.push_back(layer);
				}
			}
			return active_layers;
		};
		static auto layers = layer_loader();
		return layers;
	}
}
