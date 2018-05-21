#include "../vertex_array.hpp"

namespace gl
{
vertex_array::attribute::attribute(mygl::vertex_array vao, int index)
        : _vao(vao)
        , _index(index)
{
}

vertex_array::attribute& vertex_array::attribute::enable(bool enabled) noexcept
{
    _enabled = enabled;
    _enabled ? glEnableVertexArrayAttrib(_vao, _index) : glDisableVertexArrayAttrib(_vao, _index);
    return *this;
}

vertex_array::attribute& vertex_array::attribute::bind(int binding) noexcept
{
    glVertexArrayAttribBinding(_vao, _index, _binding = binding);
    _bound = true;
    return *this;
}

vertex_array::attribute& vertex_array::attribute::format(int length, GLenum type, bool normalized,
                                                         ptrdiff_t offset) noexcept
{
    _length     = length;
    _type       = type;
    _normalized = normalized;
    _offset     = offset;
    glVertexArrayAttribFormat(
            _vao, _index, _length, type, normalized, static_cast<unsigned>(offset));
    _formatted = true;
    return *this;
}

vertex_array::vertex_array() { glCreateVertexArrays(1, &_id); }

vertex_array::~vertex_array()
{
    if(_id != mygl::vertex_array::zero)
        glDeleteVertexArrays(1, &_id);
}

vertex_array::vertex_array(const vertex_array& other)
        : vertex_array()
{
    for(const auto& attr : other._attributes)
    {
        auto&& att = attrib(attr.second._index);
        att.enable(attr.second._enabled);
        if(attr.second._formatted)
            att.format(attr.second._length,
                       attr.second._type,
                       attr.second._normalized,
                       attr.second._offset);
        if(attr.second._bound)
            att.bind(attr.second._binding);
    }

    for(const auto& vbop : other._vertex_buffers)
    {
        vertex_buffer(vbop.first, vbop.second.buffer, vbop.second.offset, vbop.second.stride);
    }
    if(other._element_buffer != mygl::buffer::zero)
        element_buffer(other._element_buffer);
}

vertex_array::vertex_array(vertex_array&& other) noexcept
        : _id(other._id)
        , _attributes(std::move(other._attributes))
        , _vertex_buffers(std::move(other._vertex_buffers))
        , _element_buffer(other._element_buffer)
{
    other._id = mygl::vertex_array::zero;
}

vertex_array& vertex_array::operator=(const vertex_array& other)
{
    if(_id != mygl::vertex_array::zero)
        glDeleteVertexArrays(1, &_id);
    glCreateVertexArrays(1, &_id);
    for(const auto& attr : other._attributes)
    {
        auto&& att = attrib(attr.second._index);
        att.enable(attr.second._enabled);
        if(attr.second._formatted)
            att.format(attr.second._length,
                       attr.second._type,
                       attr.second._normalized,
                       attr.second._offset);
        if(attr.second._bound)
            att.bind(attr.second._binding);
    }
    for(const auto& vbop : other._vertex_buffers)
    {
        vertex_buffer(vbop.first, vbop.second.buffer, vbop.second.offset, vbop.second.stride);
    }
    if(other._element_buffer != mygl::buffer::zero)
        element_buffer(other._element_buffer);
    return *this;
}

vertex_array& vertex_array::operator=(vertex_array&& other) noexcept
{
    if(_id != mygl::vertex_array::zero)
        glDeleteVertexArrays(1, &_id);
    _id             = other._id;
    _attributes     = std::move(other._attributes);
    _vertex_buffers = std::move(other._vertex_buffers);
    _element_buffer = other._element_buffer;
    other._id       = mygl::vertex_array::zero;
    return *this;
}

vertex_array::attribute& vertex_array::attrib(int index)
{
    if(const auto it = _attributes.find(index); it != _attributes.end())
        return it->second;

    _attributes.emplace(index, attribute(_id, index));
    return _attributes.at(index);
}

void vertex_array::vertex_buffer(int binding, const mygl::buffer& buffer, ptrdiff_t offset,
                                 size_t stride)
{
    _vertex_buffers[binding] = vbo{buffer, offset, stride};
    glVertexArrayVertexBuffer(_id, binding, buffer, offset, static_cast<int>(stride));
}

void vertex_array::element_buffer(const mygl::buffer& buffer)
{
    _element_buffer = buffer;
    glVertexArrayElementBuffer(_id, buffer);
}

void vertex_array::bind() const noexcept { glBindVertexArray(_id); }

vertex_array::operator mygl::vertex_array() const noexcept { return _id; }
}
