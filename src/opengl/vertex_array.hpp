#pragma once
#include "buffer.hpp"
#include <mygl/mygl.hpp>
#include <unordered_map>

namespace gl
{
class vertex_array
{
public:
    class attribute
    {
    public:
        friend class vertex_array;
        attribute(const attribute& other) = delete;
        attribute(attribute&& other)      = default;
        attribute(mygl::vertex_array vao, int index);

        attribute& enable(bool enabled) noexcept;
        attribute& bind(int binding) noexcept;
        attribute& format(int length, GLenum type, bool normalized, ptrdiff_t offset) noexcept;

    private:
        bool _bound     = false;
        bool _formatted = false;

        mygl::vertex_array _vao{0};
        int                _index      = 0;
        int                _binding    = 0;
        bool               _enabled    = false;
        int                _length     = 0;
        GLenum             _type       = GLenum(0);
        bool               _normalized = false;
        ptrdiff_t          _offset     = 0;
    };

    vertex_array();
    ~vertex_array();

    vertex_array(const vertex_array& other);
    vertex_array(vertex_array&& other) noexcept;
    vertex_array& operator=(const vertex_array& other);
    vertex_array& operator=(vertex_array&& other) noexcept;

    attribute&                 attrib(int index);
    template <typename T> void vertex_buffer(int binding, const buffer<T>& buffer, ptrdiff_t offset = 0, size_t stride = sizeof(T));
    void                       vertex_buffer(int binding, const mygl::buffer& buffer, ptrdiff_t offset, size_t stride);
    template <typename T> void element_buffer(const buffer<T>& buffer);
    void                       element_buffer(const mygl::buffer& buffer);

    void bind() const noexcept;

    operator mygl::vertex_array() const noexcept;

    void draw(GLenum mode, int vertices, int first_vertex = 0) const noexcept;
    void draw_instanced(GLenum mode, int vertices, int instances, int first_vertex = 0, int first_instance = 0) const noexcept;
    void draw(GLenum mode, int elements, GLenum element_type, ptrdiff_t first_element = 0, int first_vertex = 0) const noexcept;
    void draw_instanced(GLenum mode, int elements, GLenum element_type, int instances, ptrdiff_t first_element = 0, int first_vertex = 0, int first_instance = 0) const noexcept;

    struct multi_array_command
    {
        int count;
        int first = 0;
    };
    void multi_draw(GLenum mode, const std::vector<multi_array_command>& commands) const;

    struct multi_element_command
    {
        int count;
        ptrdiff_t first_element = 0;
        int first_vertex = 0;
    };
    void multi_draw(GLenum mode, GLenum element_type, const std::vector<multi_element_command>& commands) const;

private:
    struct vbo
    {
        mygl::buffer buffer;
        ptrdiff_t    offset;
        size_t       stride;
    };

    mygl::vertex_array                 _id;
    std::unordered_map<int, attribute> _attributes;
    std::unordered_map<int, vbo>       _vertex_buffers;
    mygl::buffer                       _element_buffer = mygl::buffer::zero;
};
}
#include "impl/vertex_array.inl"
