#pragma once

#include <gfx/gfx.hpp>
#include <optional>
#include <string>
#include <vector>

class prototype_manager;
struct mesh;

struct prototype
{
    friend class prototype_manager;
	constexpr static size_t max_submeshes = 8;
    struct submesh
    {
		mesh* base;
		gfx::transform relative_transform;
    };

    std::string name;
	std::array<std::optional<submesh>, max_submeshes> meshes{std::nullopt};

private:
    uint32_t _first_draw_command = 0;
    uint32_t _draw_command_count = 0;
};

struct mesh
{
    friend class prototype_manager;

private:
    uint32_t _index_count  = 0;
    uint32_t _vertex_count = 0;
    uint32_t _base_index   = 0;
    uint32_t _base_vertex  = 0;
};

struct prototype_deleter
{
	prototype_deleter(prototype_manager& mgr) : _mgr(mgr) {}
	void operator()(prototype* p) const;

private:
	prototype_manager& _mgr;
};
struct mesh_deleter
{
	mesh_deleter(prototype_manager& mgr) : _mgr(mgr) {}
	void operator()(mesh* m) const;

private:
	prototype_manager& _mgr;
};

using unique_prototype = std::unique_ptr<prototype, prototype_deleter>;
using unique_mesh = std::unique_ptr<mesh, mesh_deleter>;

class prototype_manager
{
public:
	prototype* allocate_prototype(std::string name, std::initializer_list<mesh*> meshes)
	{
		return allocate_prototype(std::move(name), gsl::make_span(std::data(meshes), std::size(meshes)));
	}
    prototype* allocate_prototype(std::string name, const gfx::span<mesh* const>& meshes)
    {
		auto& proto = *_prototypes.emplace(name, std::make_unique<prototype>()).first->second;
		int i = 0;
		for (mesh* const m : meshes)
			proto.meshes[i++] ={ m, gfx::transform{} };
		proto.name = std::move(name);
        return &proto;
    }
    void free_prototype(const prototype* proto)
    {
		assert(proto);
		_prototypes.erase(proto->name);
    }

	unique_prototype allocate_prototype_unique(std::string name, const gfx::span<mesh*>& meshes)
    {
		return unique_prototype(allocate_prototype(name, meshes), prototype_deleter(*this));
    }

    mesh* allocate_mesh(const gfx::span<gfx::vertex3d>& vertices, const gfx::span<gfx::index32>& indices)
    {
        mesh* m = [this] { return &*_meshes.emplace_back(std::make_unique<mesh>()); }();
        assert(m);
        m->_base_index   = static_cast<uint32_t>(_index_buffer.size());
        m->_base_vertex  = static_cast<uint32_t>(_vertex_buffer.size());
        m->_index_count  = static_cast<uint32_t>(indices.size());
        m->_vertex_count = static_cast<uint32_t>(vertices.size());

        _staging_vertex_buffer.resize(_staging_vertex_buffer.size() + vertices.size());
        _staging_index_buffer.resize(_staging_index_buffer.size() + indices.size());
        memcpy(_staging_vertex_buffer.data(), vertices.data(), vertices.size() * sizeof(gfx::vertex3d));
        memcpy(_staging_index_buffer.data(), indices.data(), indices.size() * sizeof(gfx::index32));

        gfx::buf_copy(_vertex_buffer, _staging_vertex_buffer, _staging_vertex_buffer.size());
        gfx::buf_copy(_index_buffer, _staging_index_buffer, _staging_index_buffer.size());

        return m;
    }
    void free_mesh(const mesh* m)
    {
        memmove(_staging_vertex_buffer.data() + m->_base_vertex, _staging_vertex_buffer.data() + m->_base_vertex + m->_vertex_count,
                (_staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count) * sizeof(gfx::vertex3d));
        memmove(_staging_index_buffer.data() + m->_base_index, _staging_index_buffer.data() + m->_base_index + m->_index_count,
                (_staging_index_buffer.size() - m->_base_index - m->_index_count) * sizeof(gfx::index32));

        gfx::buf_copy(_vertex_buffer, _staging_vertex_buffer, _staging_vertex_buffer.size() - m->_base_vertex - m->_vertex_count,
                      m->_base_vertex, m->_base_vertex);
        gfx::buf_copy(_index_buffer, _staging_index_buffer, _staging_index_buffer.size() - m->_base_index - m->_index_count, m->_base_index,
                      m->_base_index);

		_staging_vertex_buffer.resize(_staging_vertex_buffer.size() - m->_vertex_count);
		_staging_index_buffer.resize(_staging_index_buffer.size() - m->_index_count);

        for (auto it = _meshes.begin(); it != _meshes.end();)
        {
            if (&**it != m)
            {
                if ((*it)->_base_index > m->_base_index) (*it)->_base_index -= m->_index_count;
                if ((*it)->_base_vertex > m->_base_vertex) (*it)->_base_vertex -= m->_vertex_count;
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

	unique_mesh allocate_mesh_unique(const gfx::span<gfx::vertex3d>& vertices, const gfx::span<gfx::index32>& indices)
	{
		return unique_mesh(allocate_mesh(vertices, indices), mesh_deleter(*this));
	}

	prototype* prototype_by_name(const std::string& name)
    {
		return &*_prototypes.at(name);
    }

    template<typename Fun, typename = decltype(std::declval<Fun>()(std::declval<prototype*>()))>
    void for_each_prototype(Fun&& f)
	{
		for (auto& p : _prototypes)
			f(&*p.second);
	}

    void clear_commands()
    {
		_current_instance = (_current_instance+1) % _draw_commands.size();
        _draw_commands[_current_instance].clear();
    }

    void draw(prototype* p, const gfx::transform& t)
    {
		for (const auto& m : p->meshes)
		{
			if (!m) break;
			draw_command& c = _draw_commands[_current_instance].emplace_back();
			c.indirect.base_index = m->base->_base_index;
			c.indirect.base_vertex = m->base->_base_vertex;
			c.indirect.index_count = m->base->_index_count;
			c.indirect.base_instance = 0;
			c.indirect.instance_count = 1;
			c.transform = t * m->relative_transform;
		}
    }

    void render(gfx::commands& cmd)
    {
		cmd.bind_vertex_buffer(_vertex_buffer, 0);
		cmd.bind_index_buffer(_index_buffer, gfx::index_type::uint32);
		cmd.draw_indirect_indexed(_draw_commands[_current_instance]);
    }

private:
    struct draw_command
    {
        alignas(16) gfx::drawcmd_indexed indirect;
        alignas(16) glm::mat4 transform;
    };

    std::unordered_map<std::string, std::unique_ptr<prototype>> _prototypes;
    std::vector<std::unique_ptr<mesh>>      _meshes;

    gfx::hbuffer<gfx::vertex3d> _staging_vertex_buffer;
    gfx::hbuffer<gfx::index32>  _staging_index_buffer;
    gfx::buffer<gfx::vertex3d>  _vertex_buffer;
    gfx::buffer<gfx::index32>   _index_buffer;

	ptrdiff_t _current_instance = 0;
    std::array<gfx::hbuffer<draw_command>, 3> _draw_commands;
};

inline void prototype_deleter::operator()(prototype* p) const
{
	_mgr.free_prototype(p);
}

inline void mesh_deleter::operator()(mesh* m) const
{
	_mgr.free_mesh(m);
}
