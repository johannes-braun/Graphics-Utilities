#pragma once

#include "gl.hpp"
#include "shader.hpp"
#include "pipeline.hpp"
#include "buffer.hpp"

#include <cstdint>

namespace gl
{
    template<GLenum Token, class Command>
    struct command
    {
        using u16 = uint16_t;
        using u32 = uint32_t;
        using u64 = uint64_t;

        const uint8_t* as_bytes() const noexcept { return reinterpret_cast<const uint8_t*>(this); }
        constexpr static uint32_t byte_size() noexcept { return sizeof Command; }
    private: 
        u32 _header = glGetCommandHeaderNV(Token, byte_size());
    };

    struct cmd_terminate_sequence : command<GL_TERMINATE_SEQUENCE_COMMAND_NV, cmd_terminate_sequence> {};
    struct cmd_nop : command<GL_NOP_COMMAND_NV, cmd_terminate_sequence> {};
    struct cmd_draw_elements : command<GL_DRAW_ELEMENTS_COMMAND_NV, cmd_draw_elements>
    {
        cmd_draw_elements(const u32 count, const u32 first_index = 0, const u32 base_vertex = 0) noexcept 
            : count(count), first_index(first_index), base_vertex(base_vertex) {}
        u32 count;
        u32 first_index;
        u32 base_vertex;
    };
    struct cmd_draw_arrays : command<GL_DRAW_ARRAYS_COMMAND_NV, cmd_draw_arrays>
    {
        cmd_draw_arrays(const u32 count, const u32 first = 0) noexcept : count(count), first(first) {}
        u32  count;
        u32  first;
    };
    struct cmd_draw_elements_strip : command<GL_DRAW_ELEMENTS_STRIP_COMMAND_NV, cmd_draw_elements_strip>
    {
        cmd_draw_elements_strip(const u32 count, const u32 first_index = 0, const u32 base_vertex = 0) noexcept 
            : count(count), first_index(first_index), base_vertex(base_vertex) {}
        u32 count;
        u32 first_index;
        u32 base_vertex;
    };
    struct cmd_draw_arrays_strip : command<GL_DRAW_ARRAYS_STRIP_COMMAND_NV, cmd_draw_arrays_strip>
    {
        cmd_draw_arrays_strip(const u32 count, const u32 first = 0) noexcept
            : count(count), first(first) {}
        u32  count;
        u32  first;
    };
    struct cmd_draw_elements_instanced : command<GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV, cmd_draw_elements_instanced>
    {
        cmd_draw_elements_instanced(const GLenum p, const u32 count, const u32 first_index = 0, const u32 base_vertex = 0, const u32 instance_count = 1, const u32 base_instance = 0) noexcept
            : mode(static_cast<u32>(p)), count(count), first_index(first_index), base_vertex(base_vertex), instance_count(instance_count), base_instance(base_instance)
        {}
        u32 mode;
        u32 count;
        u32 first_index;
        u32 base_vertex;
        u32 instance_count;
        u32 base_instance;
    };
    struct cmd_draw_arrays_instanced : command<GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV, cmd_draw_arrays_instanced>
    {
        cmd_draw_arrays_instanced(const GLenum p, const u32 count, const u32 first = 0, const u32 instance_count = 1, const u32 base_instance = 0) noexcept
            : mode(static_cast<u32>(p)), count(count), first(first), instance_count(instance_count), base_instance(base_instance)
        {}
        u32 mode;
        u32 count;
        u32 first;
        u32 instance_count;
        u32 base_instance;
    };
    struct cmd_element_address : command<GL_ELEMENT_ADDRESS_COMMAND_NV, cmd_element_address>
    {
        union overlapper64 {
            uint64_t QWORD;
            uint32_t DWORD[2];
        };

        cmd_element_address(const u64 address, const u32 type_size) noexcept 
            : address{ overlapper64{ address }.DWORD[0], overlapper64{ address }.DWORD[1] }, type_size(type_size) {}
        u32 address[2];
        u32 type_size;
    };
    struct cmd_attribute_address : command<GL_ATTRIBUTE_ADDRESS_COMMAND_NV, cmd_attribute_address>
    {
        cmd_attribute_address(const u32 index, const u64 address) noexcept 
            : index(index), address(address) {}
        u32 index;
        u64 address;
    };
    struct cmd_uniform_address : command<GL_UNIFORM_ADDRESS_COMMAND_NV, cmd_uniform_address>
    {
        cmd_uniform_address(const u16 index, const GLenum stage, const u64 address) noexcept
            : index(index), stage_id(glGetStageIndexNV(stage)), address(address)
        {}
        u16 index;
        u16 stage_id;
        u64 address;
    };
    struct cmd_blend_color : command<GL_BLEND_COLOR_COMMAND_NV, cmd_blend_color>
    {
        cmd_blend_color(const glm::vec4 color) noexcept
            : color(color)
        {}
        glm::vec4 color;
    };
    struct cmd_stencil_ref : command<GL_STENCIL_REF_COMMAND_NV, cmd_stencil_ref>
    {
        cmd_stencil_ref(const u32 front, const u32 back) noexcept
            : front(front), back(back)
        {}
        u32 front;
        u32 back;
    };
    struct cmd_line_width : command<GL_LINE_WIDTH_COMMAND_NV, cmd_line_width>
    {
        cmd_line_width(const float line_width) noexcept
            : line_width(line_width)
        {}
        float line_width;
    };
    struct cmd_polygon_offset : command<GL_POLYGON_OFFSET_COMMAND_NV, cmd_polygon_offset>
    {
        cmd_polygon_offset(const float scale, const float bias) noexcept
            : scale(scale), bias(bias)
        {}
        float scale;
        float bias;
    };
    struct cmd_alpha_ref : command<GL_ALPHA_REF_COMMAND_NV, cmd_alpha_ref>
    {
        cmd_alpha_ref(const float alpha_ref) noexcept
            : alpha_ref(alpha_ref)
        {}
        float alpha_ref;
    };
    struct cmd_viewport : command<GL_VIEWPORT_COMMAND_NV, cmd_viewport>
    {
        cmd_viewport(const u32 x, const u32 y, const u32 w, const u32 h) noexcept
            : x(x), y(y), width(w), height(h)
        {}
        u32 x, y;
        u32 width, height;
    };
    struct cmd_scissor : command<GL_SCISSOR_COMMAND_NV, cmd_scissor>
    {
        cmd_scissor(const u32 x, const u32 y, const u32 w, const u32 h) noexcept
            : x(x), y(y), width(w), height(h)
        {}
        u32 x, y;
        u32 width, height;
    };
    enum class front_face : uint32_t { cw = 0, ccw };
    struct cmd_front_face : command<GL_FRONT_FACE_COMMAND_NV, cmd_front_face>
    {
        cmd_front_face(const front_face ff) noexcept
            : front_face(ff)
        {}
        front_face front_face;
    };

    class command_buffer
    {
    public:
        template<typename T, typename = std::void_t<decltype(std::declval<T>().as_bytes()), decltype(T::byte_size())>>
        void push(const T& cmd);
        void start() noexcept;
        void finish() noexcept;

        uint64_t indirect;
        int32_t size;

    private:
        buffer<uint8_t> _command_buffer = buffer<uint8_t>(GL_DYNAMIC_STORAGE_BIT);
    };
}

#include "impl/command_list.inl"
