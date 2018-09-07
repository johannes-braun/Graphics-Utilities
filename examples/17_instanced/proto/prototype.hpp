#pragma once

#include <gfx/gfx.hpp>
#include <optional>
#include <string>
#include <vector>

class prototype_manager;

struct prototype
{
    friend class prototype_manager;

    std::string name;

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

class prototype_manager
{
public:
    prototype* allocate_prototype() { return &*_prototypes.emplace_back(std::make_unique<prototype>()); }

    void free_prototype(const prototype* proto)
    {
        for (auto& p : _prototypes)
            if (p && &*p == proto)
            {
                std::exchange(p, _prototypes.back());
                _prototypes.pop_back();
                break;
            }
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

        for (auto& e : _meshes)
        {
            if (&*e != m)
            {
                if (e->_base_index > m->_base_index) e->_base_index -= m->_index_count;
                if (e->_base_vertex > m->_base_vertex) e->_base_vertex -= m->_vertex_count;
            }
        }
    }

private:
    std::vector<std::unique_ptr<prototype>> _prototypes;
    std::vector<std::unique_ptr<mesh>>      _meshes;

    gfx::hbuffer<gfx::vertex3d> _staging_vertex_buffer;
    gfx::hbuffer<gfx::index32>  _staging_index_buffer;
    gfx::buffer<gfx::vertex3d>  _vertex_buffer;
    gfx::buffer<gfx::index32>   _index_buffer;

    gfx::hbuffer<gfx::drawcmd_indexed> _draw_commands;
};