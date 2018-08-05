#pragma once

#include "../binding_set.hpp"
#include "binding_layout_opengl.hpp"
#include <mygl/mygl.hpp>

namespace gfx
{
	inline namespace v1
	{
		namespace opengl
		{
			std::function<void()> bind_sets(std::initializer_list<binding_set*> sets);

			class binding_set_implementation : public detail::binding_set_implementation
			{
			public:
				~binding_set_implementation();
				void initialize(const binding_layout& layout) override;
				handle api_handle() override;
				void bind(u32 binding, u32 arr_element, binding_type type, std::any obj) override;

				std::function<void()> bind_all(u32& ssb_offset, u32& ub_offset, u32& img_offset, u32& tex_offset);

			private:
				std::vector<u32> _items;
				std::vector<std::vector<binding_layout_implementation::range>> _layout;
				std::vector<binding_type> _types;

				std::vector<mygl::buffer> _storage_buffers;
				std::vector<mygl::buffer> _uniform_buffers;
				std::vector<mygl::texture> _storage_images;
				std::vector<mygl::texture> _sampled_images;
				std::vector<mygl::sampler> _sampled_image_samplers;
			};

		}
	}
}
