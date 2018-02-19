#include "query.hpp"

namespace gl
{
    query::query(const GLenum type)
        : _type(type)
    {
        glCreateQueries(_type, 1, &_id);
    }

    query::~query()
    {
        glDeleteQueries(1, &_id);
    }

    void query::begin() const
    {
        glBeginQuery(_type, _id);
    }

    void query::end() const
    {
        glEndQuery(_type);
    }

    int query::get_int(const GLenum param) const
    {
        int i;
        glGetQueryObjectiv(_id, param, &i);
        return i;
    }

    uint32_t query::get_uint(const GLenum param) const
    {
        uint32_t i;
        glGetQueryObjectuiv(_id, param, &i);
        return i;
    }

    int64_t query::get_int64(const GLenum param) const
    {
        int64_t i;
        glGetQueryObjecti64v(_id, param, &i);
        return i;
    }

    uint64_t query::get_uint64(const GLenum param) const
    {
        uint64_t i;
        glGetQueryObjectui64v(_id, param, &i);
        return i;
    }
}
