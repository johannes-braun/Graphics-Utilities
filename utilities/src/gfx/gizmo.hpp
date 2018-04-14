#pragma once
#include "geometry.hpp"
#include <opengl/pipeline.hpp>
#include <opengl/buffer.hpp>
#include <GLFW/glfw3.h>
#include <gfx/flags.hpp>

namespace gfx
{
    enum class gizmo_state : uint8_t
    {
        hover_none = 0,
        hover_center = 0b111,
        hover_x = 0b001,
        hover_y = 0b010,
        hover_z = 0b100,
        hover_xy = 0b011,
        hover_xz = 0b101,
        hover_yz = 0b110,

        hover_rx = 0b001000,
        hover_ry = 0b010000,
        hover_rz = 0b100000,
    };
    using gizmo_state_flags = flags<uint8_t, gizmo_state>;

    class gizmo
    {
    public:
        struct vertex
        {
            glm::vec3 position;
            glm::u8vec4 color;
        };
        using index = uint16_t;

        constexpr static int circle_resolution = 36;
        static const std::array<vertex, 9 + 6 + 6> non_cube_vertices;
        static const std::array<index, 9 + 6 + 3> non_cube_indices;

        gizmo();

        void reassign_vertices(std::initializer_list<size_t> indices, bool to_default);
        void change_hover_state(gizmo_state_flags new_state);
        void update(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection);
        void render() const;

        gizmo(const gizmo& other) = default;
        gizmo(gizmo&& other) noexcept = default;
        gizmo& operator=(const gizmo& other) = default;
        gizmo& operator=(gizmo&& other) noexcept = default;
        ~gizmo() = default;

        gfx::transform* transform = nullptr;

    private:
        static bool intersect_bounds(glm::vec3 origin, glm::vec3 direction,
                                     glm::vec3 bounds_min, glm::vec3 bounds_max, float max_distance,
                                     float* tmin = nullptr);

        bool _pressed = false;
        bool _first_down = false;
        bool _click_lock = false;
        glm::vec3 _down_offset;
        glm::quat _down_rotation_offset;
        gizmo_state_flags _last_hover_state;
        std::vector<vertex> _vertices;
        std::vector<vertex> _vertices_default;

        gl::pipeline _translate_pipeline;
        gl::buffer<vertex> _vertex_buffer = gl::buffer<vertex>(GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
        gl::buffer<index> _index_buffer = gl::buffer<index>(GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    };
}
