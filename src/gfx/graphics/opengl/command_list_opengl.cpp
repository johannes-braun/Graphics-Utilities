#include "command_list_opengl.hpp"
#include "graphics_pipeline_opengl.hpp"
#include "framebuffer_opengl.hpp"
#include "fence_opengl.hpp"
#include "binding_set_opengl.hpp"

namespace gfx
{
	namespace v2
	{
		namespace opengl
		{
			void commands_implementation::initialize(commands_type type)
			{
				
			}
			void commands_implementation::reset()
			{
				_has_state = false;
				_queue.clear();
				_curr_pipeline = nullptr;
			}
			void commands_implementation::begin()
			{
			}
			void commands_implementation::end()
			{
			}
			void commands_implementation::execute_sync_after(const commands & cmd, fence * f)
			{
				execute(f);
			}
			void commands_implementation::execute(fence * f)
			{
				for (auto& q : _queue) {
					q();
				}
				_curr_pipeline = nullptr;

				glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero);
				v1::opengl::push_fence(f);
				glFlush();
			}
			void commands_implementation::bind_pipeline(const compute_pipeline & p, std::initializer_list<binding_set*> bindings)
			{
				_has_state     = false;
				_curr_pipeline = nullptr;
				_queue.emplace_back([hnd = handle_cast<mygl::pipeline>(p)] { glBindProgramPipeline(hnd); });
			}
			void commands_implementation::dispatch(u32 x, u32 y, u32 z)
			{
				_queue.emplace_back([=] { glDispatchCompute(x, y, z); });
			}
			void commands_implementation::begin_pass(const framebuffer & fbo, std::optional<rect2f> render_area)
			{
				_curr_framebuffer = &fbo;
				_queue.emplace_back([fbo = _curr_framebuffer] {
					static_cast<v1::opengl::framebuffer_implementation*>(&*fbo->implementation())->begin();

					for (int i = 0; i < fbo->color_clear_values().size(); ++i) {
						if (const auto cv = fbo->color_clear_values()[i]) {
							glClearNamedFramebufferfv(handle_cast<mygl::framebuffer>(*fbo), GL_COLOR, i, glm::value_ptr(std::get<glm::vec4>(*cv)));
						}
					}
					if (const auto dv = fbo->depth_clear_value()) {
						const auto& dvv = std::get<depth_stencil>(*dv);
						glClearNamedFramebufferfi(handle_cast<mygl::framebuffer>(*fbo), GL_DEPTH_STENCIL, 0, dvv.depth, dvv.stencil);
					}
				});
			}
			void commands_implementation::end_pass()
			{
				_queue.emplace_back([fbo = _curr_framebuffer] { 
					static_cast<v1::opengl::framebuffer_implementation*>(&*fbo->implementation())->end();
				});
			}
			void commands_implementation::bind_pipeline(const graphics_pipeline & p, std::initializer_list<binding_set*> bindings)
			{
				_has_state     = true;
				_curr_pipeline = &p;
				_queue.emplace_back([&] { static_cast<graphics_pipeline_implementation*>(&*p.implementation())->apply_all(); });

				if (bindings.size() > 0)
				{
					u32 ssb = 0;
					u32 ub  = 0;
					u32 img = 0;
					u32 tex = 0;
					for (auto* b : bindings)
					{
						_queue.emplace_back(static_cast<v1::opengl::binding_set_implementation*>(&*b->implementation())->bind_all(ssb, ub, img, tex));
					}
				}
			}
			void commands_implementation::draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance)
			{
				_queue.emplace_back([ =, draw_mode = current_draw_mode() ] {
					glDrawArraysInstancedBaseInstance(draw_mode, static_cast<int>(base_vertex), static_cast<int>(vertex_count),
					static_cast<int>(instance_count), static_cast<uint32_t>(base_instance));
					});
			}

			GLenum commands_implementation::current_draw_mode() const
			{
				if (!_curr_pipeline) return GLenum(0);
				switch (static_cast<graphics_pipeline_implementation*>(&*_curr_pipeline->implementation())->primitive_topology())
				{
				case topology::point_list: return GL_POINTS;
				case topology::line_list: return GL_LINES;
				case topology::line_strip: return GL_LINE_STRIP;
				case topology::triangle_list: return GL_TRIANGLES;
				case topology::triangle_strip: return GL_TRIANGLE_STRIP;
				case topology::triangle_fan: return GL_TRIANGLE_FAN;
				case topology::line_list_adj: return GL_LINES_ADJACENCY;
				case topology::line_strip_adj: return GL_LINE_STRIP_ADJACENCY;
				case topology::triangle_list_adj: return GL_TRIANGLES_ADJACENCY;
				case topology::triangle_strip_adj: return GL_TRIANGLE_STRIP_ADJACENCY;
				case topology::patch_list: return GL_PATCHES;
				default: return GLenum(0);
				}
			}
		}
	}
}