#pragma once

#include "fence.hpp"
#include "graphics_pipeline.hpp"
#include "binding_set.hpp"
#include "framebuffer.hpp"
#include "../geometry.hpp"
#include <optional>

namespace gfx
{
	namespace v2
	{
		enum class commands_type
		{
			graphics,
			compute,
			transfer
		};
		class commands;

		namespace detail
		{
			class commands_implementation
			{
			public:
				static std::unique_ptr<commands_implementation> make();

				virtual void initialize(commands_type type) = 0;

				virtual void reset() = 0;
				virtual void begin() = 0;
				virtual void end() = 0;

				virtual void execute_sync_after(const commands& cmd, fence* f) = 0;
				virtual void execute(fence* f) = 0;

				virtual void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings) = 0;
				virtual void dispatch(u32 x, u32 y, u32 z) = 0;

				virtual void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area) = 0;
				virtual void end_pass() = 0;

				virtual void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings) = 0;
				virtual void draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance) = 0;
			};
		}

		class commands : public impl::implements<detail::commands_implementation>
		{
		public:
			commands(commands_type type) { implementation()->initialize(type); }

			void reset() { implementation()->reset(); }
			void begin() { implementation()->begin(); }
			void end() { implementation()->end(); }

			void execute_sync_after(const commands& cmd, fence* f = nullptr) { implementation()->execute_sync_after(cmd, f); }
			void execute(fence* f = nullptr) { implementation()->execute(f); }

			void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings ={}) { implementation()->bind_pipeline(p, bindings); }
			void dispatch(u32 x, u32 y = 1, u32 z = 1) { implementation()->dispatch(x, y, z); }

			void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area ={})  { implementation()->begin_pass(fbo, render_area); }
			void end_pass()  { implementation()->end_pass(); }

			void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings ={})  { implementation()->bind_pipeline(p, bindings); }
			void draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance)  { implementation()->draw(vertex_count, instance_count, base_vertex, base_instance); }
		};

	}
}