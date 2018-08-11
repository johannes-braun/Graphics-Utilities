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
				void bind(u32 binding, u32 arr_element, binding_type type, std::any obj, u32 offset, u32 size) override;
				u32 binding(u32 b, u32 arr) const;

				std::function<void()> bind_all(u32& ssb_offset, u32& ub_offset, u32& img_offset, u32& tex_offset);

				u32 count(binding_type t) const {
					return u32(std::count(_types.begin(), _types.end(), t));
				}

			private:
				std::vector<u32> _items;
				std::vector<std::vector<binding_layout_implementation::range>> _layout;
				std::vector<binding_type> _types;

                struct buffer_binding
                {
					mygl::buffer buf;
					u32 offset;
					u32 size;
                };

				std::vector<buffer_binding> _storage_buffers;
				std::vector<buffer_binding> _uniform_buffers;
				std::vector<mygl::texture> _storage_images;
				std::vector<mygl::texture> _sampled_images;
				std::vector<mygl::sampler> _sampled_image_samplers;
			};

		}
	}
}
