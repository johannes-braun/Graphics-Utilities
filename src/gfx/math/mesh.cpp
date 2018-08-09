#include "geometry.hpp"
#include <execution>

namespace gfx {
inline namespace v1 {
mesh3d mesh3d::extract(submesh3d sm) const
{
    mesh3d m;
    m.indices      = {indices.begin() + sm.base_index, indices.begin() + sm.base_index + sm.index_count};
    m.vertices     = {vertices.begin() + sm.base_vertex, vertices.begin() + sm.base_vertex + sm.vertex_count};
    sm.base_index  = 0;
    sm.base_vertex = 0;
    m.geometries.emplace_back(sm);
    return m;
}

void mesh3d::collapse()
{
#pragma omp parallel for schedule(static)
    for (int vtx = 0; vtx < static_cast<int>(geometries[0].vertex_count); ++vtx) {
        vertices[vtx] = geometries[0].transformation * vertices[vtx];
    }

    for (int i = 1; i < static_cast<int>(geometries.size()); ++i) {
        auto& s = geometries[i];

#pragma omp parallel for schedule(static)
        for (int idx = s.base_index; idx < static_cast<int>(s.base_index + s.index_count); ++idx) {
            indices[idx] += geometries[0].vertex_count;
        }
#pragma omp parallel for schedule(static)
        for (int vtx = s.base_vertex; vtx < static_cast<int>(s.base_vertex + s.vertex_count); ++vtx) {
            vertices[vtx] = s.transformation * vertices[vtx];
        }
        geometries[0].vertex_count += s.vertex_count;
        geometries[0].index_count += s.index_count;
    }
    geometries[0].transformation = transform{};
    geometries.resize(1);
}

bounds3f mesh3d::compute_bounds() const
{
	struct reduction
	{
		bounds3f operator()(const bounds3f& b, const vertex3d& x) const { return b + x.position; }
		bounds3f operator()(const vertex3d& x, const bounds3f& b) const { return b + x.position; }
		bounds3f operator()(const vertex3d& b, const vertex3d& x) const
		{
			bounds3f bounds;
			bounds += b.position;
			return bounds + x.position;
		}
		bounds3f operator()(const bounds3f& b, const bounds3f& x) const { return b + x; }
	};

	return std::reduce(std::execution::par_unseq, vertices.begin(), vertices.end(), bounds3f{}, reduction());
}

mesh3d mesh3d::operator+(const mesh3d& other) const
{
    mesh3d m;
    m.indices.resize(indices.size() + other.indices.size());
    m.vertices.resize(vertices.size() + other.vertices.size());
    m.geometries.resize(geometries.size() + other.geometries.size());

#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(indices.size()); ++i) m.indices[i] = indices[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(other.indices.size()); ++i) m.indices[i + indices.size()] = other.indices[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(vertices.size()); ++i) m.vertices[i] = vertices[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(other.vertices.size()); ++i) m.vertices[i + vertices.size()] = other.vertices[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(geometries.size()); ++i) m.geometries[i] = geometries[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(other.geometries.size()); ++i) {
        auto& g = m.geometries[i + geometries.size()];
        g       = other.geometries[i];
        g.base_index += static_cast<u32>(indices.size());
        g.base_vertex += static_cast<u32>(vertices.size());
    }
    return m;
}

mesh3d& mesh3d::operator+=(const mesh3d& other)
{
    const auto is = indices.size();
    const auto vs = vertices.size();
    const auto gs = geometries.size();
    indices.resize(is + other.indices.size());
    vertices.resize(vs + other.vertices.size());
    geometries.resize(gs + other.geometries.size());

#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(other.indices.size()); ++i) indices[i + is] = other.indices[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(other.vertices.size()); ++i) vertices[i + vs] = other.vertices[i];
#pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(other.geometries.size()); ++i) {
        auto& g = geometries[i + gs];
        g       = other.geometries[i];
        g.base_index += static_cast<u32>(is);
        g.base_vertex += static_cast<u32>(vs);
    }
    return *this;
}
}    // namespace v1
}    // namespace gfx