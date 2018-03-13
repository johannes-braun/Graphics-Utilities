#include "../query.hpp"

namespace gl
{
    query::query(GLenum type) noexcept
        : _type(type)
    {
        glCreateQueries(type, 1, &_id);
    }

    query::query(const query& other) noexcept
        : query(other._type)
    {
    }

    query::query(query&& other) noexcept
        : _id(other._id), _type(other._type)
    {
        other._id = gl_query_t::zero;
        other._type = GL_ZERO;
    }

    query& query::operator=(const query& other) noexcept
    {
        if (_id != gl_query_t::zero)
            glDeleteQueries(1, &_id);
        _type = other._type;
        glCreateQueries(_type, 1, &_id);
        return *this;
    }

    query& query::operator=(query&& other) noexcept
    {
        _id = other._id;
        _type = other._type;
        other._id = gl_query_t::zero;
        other._type = GL_ZERO;
        return *this;
    }

    query::~query() noexcept
    {
        if (_id != gl_query_t::zero)
            glDeleteQueries(1, &_id);
    }

    void query::start(uint32_t index) const noexcept
    {
        glBeginQueryIndexed(_type, index, _id);
    }

    void query::finish(uint32_t index) const noexcept
    {
        glEndQueryIndexed(_type, index);
    }

    template<>
    int query::get<int>(GLenum param) const noexcept
    {
        int i;
        glGetQueryObjectiv(_id, param, &i);
        return i;
    }

    template<>
    uint32_t query::get<uint32_t>(GLenum param) const noexcept
    {
        uint32_t i;
        glGetQueryObjectuiv(_id, param, &i);
        return i;
    }

    template<>
    int64_t query::get<int64_t>(GLenum param) const noexcept
    {
        int64_t i;
        glGetQueryObjecti64v(_id, param, &i);
        return i;
    }

    template<>
    uint64_t query::get<uint64_t>(GLenum param) const noexcept
    {
        uint64_t i;
        glGetQueryObjectui64v(_id, param, &i);
        return i;
    }
}
