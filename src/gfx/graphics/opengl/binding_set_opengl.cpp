#include "../../log.hpp"
#include "binding_set_opengl.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {


binding_set_implementation::~binding_set_implementation() {}

void binding_set_implementation::initialize(const binding_layout& layout)
{
    const binding_layout_implementation* impl = static_cast<binding_layout_implementation*>(&*layout.implementation());
    _layout                                   = impl->bindings();
    _types                                    = layout.types();
    _items                                    = impl->items();
}

handle binding_set_implementation::api_handle()
{
	return {};
}

u32 binding_set_implementation::binding(u32 b, u32 arr) const
{
	binding_type bt = _types[b];
	return _layout[static_cast<u32>(bt)][_items[b]].begin + arr;
}

void binding_set_implementation::bind(u32 binding, u32 arr_element, binding_type type, std::any obj, u32 offset, u32 size)
{
    binding_type bt = _types[binding];
    u32 b = _layout[static_cast<u32>(bt)][_items[binding]].begin + arr_element;

	assert(bt == type);

	//dlog << "Binding information: binding: " << binding << ", arr_elem: " << arr_element << " ==> " << [type]{
	//    switch(type)
	//    {
	//    case binding_type::storage_buffer: return "GL_SHADER_STORAGE_BUFFER";
	//    case binding_type::uniform_buffer: return "GL_UNIFORM_BUFFER";
	//    case binding_type::storage_image: return "Image Binding";
	//    case binding_type::sampled_image: return "Texture/Sampler Binding";
	//    default: return "INVALID";
	//    }
	//}() << " #" << b << " + set-offset";

    switch (type)
    {
    case binding_type::storage_buffer:
    {
        mygl::buffer buffer;
        if (obj.type() == typeid(const std::unique_ptr<detail::host_buffer_implementation>*)) {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::host_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
        if (obj.type() == typeid(const std::unique_ptr<detail::device_buffer_implementation>*)) {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::device_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
		_storage_buffers.resize(std::max(u32(_storage_buffers.size()), b + 1));
		_storage_buffers[b] ={ buffer, offset, size };
    }
    break;
    case binding_type::uniform_buffer:
    {
        mygl::buffer buffer;
        if (obj.type() == typeid(const std::unique_ptr<detail::host_buffer_implementation>*)) {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::host_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
        if (obj.type() == typeid(const std::unique_ptr<detail::device_buffer_implementation>*)) {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::device_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
		_uniform_buffers.resize(std::max(u32(_uniform_buffers.size()), b + 1));
		_uniform_buffers[b] = { buffer, offset, size };
    }
    break;
    case binding_type::storage_image:
    {
        auto& iv                            = *std::any_cast<const std::unique_ptr<detail::image_view_implementation>*>(obj);
        auto  img_view                      = std::any_cast<mygl::texture>(iv->api_handle());
		_storage_images.resize(std::max(u32(_storage_images.size()), b + 1));
		_storage_images[b] = img_view;
    }
    break;
    case binding_type::sampled_image:
    {
        auto[ivp, sp] = std::any_cast<
            std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*>>(
            obj);
        auto& iv       = *ivp;
        auto& s        = *sp;
        auto  img_view = std::any_cast<mygl::texture>(iv->api_handle());
        auto  smp      = std::any_cast<mygl::sampler>(s->api_handle());

		_sampled_images.resize(std::max(u32(_sampled_images.size()), b + 1));
		_sampled_images[b] = img_view;
		_sampled_image_samplers.resize(std::max(u32(_sampled_image_samplers.size()), b + 1));
		_sampled_image_samplers[b] = smp;
    }
    break;
    default:;
    }
}

std::function<void()> binding_set_implementation::bind_all(u32& ssb_offset, u32& ub_offset, u32& img_offset, u32& tex_offset)
{
    const auto lbd = [sbo = ssb_offset, ubo = ub_offset, sio = img_offset, texo = tex_offset, sb = _storage_buffers, ub = _uniform_buffers, si = _storage_images, tex = _sampled_images, smp = _sampled_image_samplers]
	{
		for (int i=0; i<sb.size(); ++i)
			sb[i].size != 0 ? glBindBufferRange(GL_SHADER_STORAGE_BUFFER, sbo + i, sb[i].buf, sb[i].offset, sb[i].size) : glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sbo + i, sb[i].buf);
		for(int i=0; i<ub.size(); ++i)
			ub[i].size != 0 ? glBindBufferRange(GL_UNIFORM_BUFFER, ubo + i, ub[i].buf, ub[i].offset, ub[i].size) : glBindBufferBase(GL_UNIFORM_BUFFER, ubo + i, ub[i].buf);
		glBindImageTextures(sio, si.size(), si.data());

		glBindSamplers(texo, smp.size(), smp.data());
		glBindTextures(texo, tex.size(), tex.data());
	};
	ssb_offset += _storage_buffers.size();
	ub_offset += _uniform_buffers.size();
	img_offset += _storage_images.size();
	tex_offset += _sampled_images.size();
	return lbd;
}
std::function<void()> bind_sets(std::initializer_list<binding_set*> sets)
{
	return [=]
	{
		u32 ssb_offset = 0;
		u32 ub_offset = 0;
		u32 img_offset = 0;
		u32 tex_offset = 0;

		for (auto& set : sets)
			static_cast<binding_set_implementation*>(&*set->implementation())->bind_all(ssb_offset, ub_offset, img_offset, tex_offset);
	};
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx