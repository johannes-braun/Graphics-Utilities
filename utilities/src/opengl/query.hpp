#pragma once

#include <mygl/mygl.hpp>
#include <type_traits>

namespace gl
{
    class query
    {
    public:
        query(GLenum type) noexcept;
        query(const query& other) noexcept;
        query(query&& other) noexcept;
        query& operator=(const query& other) noexcept;
        query& operator=(query&& other) noexcept;
        ~query() noexcept;

        void start(uint32_t index = 0) const noexcept;
        void finish(uint32_t index = 0) const noexcept;

        template<typename T>
        std::enable_if_t<std::disjunction_v <
            std::is_same<T, int32_t>,
            std::is_same<T, uint32_t>,
            std::is_same<T, int64_t>,
            std::is_same<T, uint64_t>>, T> get(GLenum param = GL_QUERY_RESULT) const noexcept;

    private:
        GLenum _type;
        mygl::query _id;
    };
}
