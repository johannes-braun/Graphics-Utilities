#include "mesh.hpp"

mesh_allocator::mesh_allocator(gfx::device & device)
	: _device(&device)
	, _staging_vertex_buffer(*_device)
	, _staging_index_buffer(*_device)
	, _staging_bvh_buffer(*_device)
	, _bvh_generator(gfx::shape::triangle)
{}

const gfx::buffer<gfx::index32>& mesh_allocator::index_buffer() const noexcept { return *_index_buffer; }

const gfx::buffer<gfx::vertex3d>& mesh_allocator::vertex_buffer() const noexcept { return *_vertex_buffer; }

unique_mesh mesh_allocator::allocate_mesh_unique(const gsl::span<const gfx::vertex3d>& vertices, const gsl::span<const gfx::index32>& indices)
{
	return unique_mesh(allocate_mesh(vertices, indices)._mesh, mesh_deleter(*this));
}

void mesh_allocator::free_mesh(const mesh_handle hnd)
{
	auto* m = hnd._mesh;
	memmove(_staging_vertex_buffer.data() + m->_base_vertex, _staging_vertex_buffer.data() + m->_base_vertex + m->_vertex_count,
		(_staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count) * sizeof(gfx::vertex3d));
	memmove(_staging_index_buffer.data() + m->_base_index, _staging_index_buffer.data() + m->_base_index + m->_index_count,
		(_staging_index_buffer.size() - m->_base_index - m->_index_count) * sizeof(gfx::index32));
	memmove(_staging_bvh_buffer.data() + m->_base_bvh_node, _staging_bvh_buffer.data() + m->_base_bvh_node + m->_bvh_node_count,
		(_staging_bvh_buffer.size() - m->_base_bvh_node - m->_bvh_node_count) * sizeof(gfx::index32));

	_index_buffer = gfx::buffer<gfx::index32>(*_device, _staging_index_buffer);
	_bvh_buffer = gfx::buffer<gfx::bvh<3>::node>(*_device, _staging_bvh_buffer);
	_vertex_buffer = gfx::buffer<gfx::vertex3d>(*_device, _staging_vertex_buffer);

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

mesh_handle mesh_allocator::allocate_mesh(const gsl::span<const gfx::vertex3d>& vertices, const gsl::span<const gfx::index32>& indices)
{
	mesh* m = &*_meshes.emplace_back(std::make_unique<mesh>());
	assert(m);
	m->_base_index = static_cast<uint32_t>(_staging_index_buffer.size());
	m->_base_vertex = static_cast<uint32_t>(_staging_vertex_buffer.size());
	m->_index_count = static_cast<uint32_t>(indices.size());
	m->_vertex_count = static_cast<uint32_t>(vertices.size());

	_staging_index_buffer.resize(_staging_index_buffer.size() + indices.size());
	memcpy(_staging_index_buffer.data() + m->_base_index, indices.data(), indices.size() * sizeof(gfx::index32));
	_bvh_generator.sort(_staging_index_buffer.begin() + m->_base_index, _staging_index_buffer.end(),
		[&](const gfx::index32 i) { return vertices[i].position; });

	m->_base_bvh_node = static_cast<uint32_t>(_staging_bvh_buffer.size());
	m->_bvh_node_count = static_cast<uint32_t>(_bvh_generator.nodes().size());

	_staging_bvh_buffer.resize(_staging_bvh_buffer.size() + _bvh_generator.nodes().size());
	memcpy(_staging_bvh_buffer.data() + m->_base_bvh_node, _bvh_generator.nodes().data(),
		_bvh_generator.nodes().size() * sizeof(gfx::bvh<3>::node));

	_staging_vertex_buffer.resize(_staging_vertex_buffer.size() + vertices.size());
	memcpy(_staging_vertex_buffer.data() + m->_base_vertex, vertices.data(), vertices.size() * sizeof(gfx::vertex3d));

	_index_buffer = gfx::buffer<gfx::index32>(*_device, _staging_index_buffer);
	_bvh_buffer = gfx::buffer<gfx::bvh<3>::node>(*_device, _staging_bvh_buffer);
	_vertex_buffer = gfx::buffer<gfx::vertex3d>(*_device, _staging_vertex_buffer);

	return m;
}

gsl::span<mesh_handle> mesh_allocator::allocate_meshes(const gfx::scene_file & scene)
{
	const auto prev_size = _meshes.size();
	const auto base_vertices = gsl::make_span(scene.mesh.vertices);
	const auto base_indices = gsl::make_span(scene.mesh.indices);
	for (const auto& geometry : scene.mesh.geometries) {
		allocate_mesh(base_vertices.subspan(geometry.base_vertex, geometry.vertex_count),
			base_indices.subspan(geometry.base_index, geometry.index_count));
	}
    auto* ms = reinterpret_cast<mesh_handle*>(_meshes.data());
	return gsl::make_span(ms, _meshes.size()).subspan(prev_size, scene.mesh.geometries.size());
}

const gfx::buffer<gfx::bvh<3>::node>& mesh_allocator::bvh_buffer() const noexcept { return *_bvh_buffer; }

gsl::span<const mesh_handle> mesh_allocator::meshes() const noexcept
{
	const auto* ms = reinterpret_cast<const mesh_handle*>(_meshes.data());
	return gsl::make_span(ms, _meshes.size());
}

void mesh_deleter::operator()(mesh* m) const
{
	_mgr.free_mesh(m);
}