#pragma once

#include <cinttypes>
#include <gfx/gfx.hpp>
#include <random>

class mesh_allocator;

struct mesh
{
    friend class mesh_allocator;

private:
    uint32_t _index_count    = 0;
    uint32_t _vertex_count   = 0;
    uint32_t _bvh_node_count = 0;
    uint32_t _base_index     = 0;
    uint32_t _base_vertex    = 0;
    uint32_t _base_bvh_node  = 0;
};

struct mesh_handle
{
	friend class mesh_allocator;

	mesh_handle() = default;
	mesh_handle(std::nullptr_t) {}
	mesh_handle(mesh* m) : _mesh(m) {}
private:
	mesh* _mesh = nullptr;
};

struct mesh_deleter
{
    mesh_deleter(mesh_allocator& mgr) : _mgr(mgr) {}
    void operator()(mesh* m) const;

private:
    mesh_allocator& _mgr;
};

using unique_mesh = std::unique_ptr<mesh, mesh_deleter>;

class mesh_allocator
{
public:
	struct instance
	{
		alignas(16) uint32_t base_index     = 0;
		alignas(4) uint32_t base_vertex    = 0;
		alignas(4) uint32_t base_bvh_node  = 0;

		alignas(16) glm::mat4 transform;
		alignas(16) glm::u8vec4 color;
		alignas(4)  float roughness;
		alignas(4)  float reflectivity;
	};

    mesh_allocator() : _bvh_generator(gfx::shape::triangle) {}

    gfx::span<mesh_handle> allocate_meshes(const gfx::scene_file& scene)
    {
		const auto prev_size = _meshes.size();
        const auto         base_vertices = gsl::make_span(scene.mesh.vertices);
        const auto         base_indices  = gsl::make_span(scene.mesh.indices);
        for (auto i = 0ull; i < scene.mesh.geometries.size(); ++i)
        {
            allocate_mesh(base_vertices.subspan(scene.mesh.geometries[i].base_vertex, scene.mesh.geometries[i].vertex_count),
                                 base_indices.subspan(scene.mesh.geometries[i].base_index, scene.mesh.geometries[i].index_count));
        }
		mesh_handle* ms = reinterpret_cast<mesh_handle*>(_meshes.data());
		return gsl::make_span(ms, _meshes.size()).subspan(prev_size, scene.mesh.geometries.size());
    }

	mesh_handle allocate_mesh(const gfx::span<const gfx::vertex3d>& vertices, const gfx::span<const gfx::index32>& indices)
    {
        mesh* m = [this] { return &*_meshes.emplace_back(std::make_unique<mesh>()); }();
        assert(m);
        m->_base_index   = static_cast<uint32_t>(_staging_index_buffer.size());
        m->_base_vertex  = static_cast<uint32_t>(_staging_vertex_buffer.size());
        m->_index_count  = static_cast<uint32_t>(indices.size());
        m->_vertex_count = static_cast<uint32_t>(vertices.size());

        const auto prev_index_buffer_size = _staging_index_buffer.size();
        _staging_index_buffer.resize(_staging_index_buffer.size() + indices.size());
        memcpy(_staging_index_buffer.data() + prev_index_buffer_size, indices.data(), indices.size() * sizeof(gfx::index32));
        _bvh_generator.sort(_staging_index_buffer.begin() + prev_index_buffer_size, _staging_index_buffer.end(),
                            [&](const gfx::index32 i) { return vertices[i].position; });
        gfx::buf_copy(_index_buffer, _staging_index_buffer, _staging_index_buffer.size());

        m->_base_bvh_node  = static_cast<uint32_t>(_staging_bvh_buffer.size());
        m->_bvh_node_count = static_cast<uint32_t>(_bvh_generator.nodes().size());

        const auto prev_bvh_buffer_size = _staging_bvh_buffer.size();
        _staging_bvh_buffer.resize(_staging_bvh_buffer.size() + _bvh_generator.nodes().size());
        memcpy(_staging_bvh_buffer.data() + prev_bvh_buffer_size, _bvh_generator.nodes().data(),
               _bvh_generator.nodes().size() * sizeof(gfx::bvh<3>::node));
        gfx::buf_copy(_bvh_buffer, _staging_bvh_buffer, _staging_bvh_buffer.size());

        const auto prev_vertex_buffer_size = _staging_vertex_buffer.size();
        _staging_vertex_buffer.resize(_staging_vertex_buffer.size() + vertices.size());
        memcpy(_staging_vertex_buffer.data() + prev_vertex_buffer_size, vertices.data(), vertices.size() * sizeof(gfx::vertex3d));
        gfx::buf_copy(_vertex_buffer, _staging_vertex_buffer, _staging_vertex_buffer.size());

        return m;
    }
    void free_mesh(const mesh_handle hnd)
    {
		auto* m = hnd._mesh;
        memmove(_staging_vertex_buffer.data() + m->_base_vertex, _staging_vertex_buffer.data() + m->_base_vertex + m->_vertex_count,
                (_staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count) * sizeof(gfx::vertex3d));
        memmove(_staging_index_buffer.data() + m->_base_index, _staging_index_buffer.data() + m->_base_index + m->_index_count,
                (_staging_index_buffer.size() - m->_base_index - m->_index_count) * sizeof(gfx::index32));
        memmove(_staging_bvh_buffer.data() + m->_base_bvh_node, _staging_bvh_buffer.data() + m->_base_bvh_node + m->_bvh_node_count,
                (_staging_bvh_buffer.size() - m->_base_bvh_node - m->_bvh_node_count) * sizeof(gfx::index32));

        gfx::buf_copy(_vertex_buffer, _staging_vertex_buffer, _staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count,
                      m->_base_vertex, m->_base_vertex);
        gfx::buf_copy(_index_buffer, _staging_index_buffer, _staging_index_buffer.size() - m->_base_index - m->_index_count, m->_base_index,
                      m->_base_index);
        gfx::buf_copy(_bvh_buffer, _staging_bvh_buffer, _staging_bvh_buffer.size() - m->_base_bvh_node - m->_bvh_node_count,
                      m->_base_bvh_node, m->_base_bvh_node);

        _staging_vertex_buffer.resize(_staging_vertex_buffer.size() - m->_vertex_count);
        _staging_index_buffer.resize(_staging_index_buffer.size() - m->_index_count);
        _staging_index_buffer.resize(_staging_bvh_buffer.size() - m->_bvh_node_count);

        for (auto it = _meshes.begin(); it != _meshes.end();)
        {
            if (&**it != m)
            {
                if ((*it)->_base_index > m->_base_index) (*it)->_base_index -= m->_index_count;
                if ((*it)->_base_vertex > m->_base_vertex) (*it)->_base_vertex -= m->_vertex_count;
                if ((*it)->_base_bvh_node > m->_base_bvh_node) (*it)->_base_bvh_node -= m->_bvh_node_count;
                ++it;
            }
            else
            {
                const auto d = std::distance(_meshes.begin(), it);
                std::iter_swap(it, std::prev(_meshes.end()));
                _meshes.pop_back();
                it = _meshes.begin() + d;
            }
        }
    }

    unique_mesh allocate_mesh_unique(const gfx::span<const gfx::vertex3d>& vertices, const gfx::span<const gfx::index32>& indices)
    {
        return unique_mesh(allocate_mesh(vertices, indices)._mesh, mesh_deleter(*this));
    }

	const gfx::buffer<gfx::vertex3d>&     vertex_buffer() const noexcept { return _vertex_buffer; }
	const gfx::buffer<gfx::index32>&      index_buffer() const noexcept {return _index_buffer;}
	const gfx::buffer<gfx::bvh<3>::node>& bvh_buffer() const noexcept {return _bvh_buffer;}
	const gfx::hbuffer<instance>&		  instances() const noexcept {return _instances;}
	gfx::span<const mesh_handle> meshes() const noexcept 
	{
		 const mesh_handle* ms = reinterpret_cast<const mesh_handle*>(_meshes.data());
		 return gsl::make_span(ms, _meshes.size());
	}

	void add_instance(const mesh_handle& m, const gfx::transform& transform, glm::vec4 color, float roughness, float reflectivity)
	{		
		instance& i = _instances.emplace_back();
		i.base_bvh_node = m._mesh->_base_bvh_node;
		i.base_index = m._mesh->_base_index;
		i.base_vertex = m._mesh->_base_vertex;
		i.transform = transform;
		i.color = glm::u8vec4(255 * color);
		i.roughness = roughness;
		i.reflectivity = reflectivity;
	}

	void clear_instances_of(const mesh_handle& m)
	{
		for (auto it = _instances.begin(); it != _instances.end();)
		{
			if (it->base_index == m._mesh->_base_index)
			{
				std::iter_swap(it, std::prev(_instances.end()));
				_instances.pop_back();
			}
			else
			{
				++it;
			}
		}
	}

private:
    std::vector<std::unique_ptr<mesh>> _meshes;

    gfx::hbuffer<gfx::vertex3d>     _staging_vertex_buffer;
    gfx::hbuffer<gfx::index32>      _staging_index_buffer;
    gfx::hbuffer<gfx::bvh<3>::node> _staging_bvh_buffer;

    gfx::buffer<gfx::vertex3d>     _vertex_buffer;
    gfx::buffer<gfx::index32>      _index_buffer;
    gfx::buffer<gfx::bvh<3>::node> _bvh_buffer;

	gfx::hbuffer<instance> _instances;

    gfx::bvh<3> _bvh_generator;
};
inline void mesh_deleter::operator()(mesh* m) const
{
    _mgr.free_mesh(m);
}