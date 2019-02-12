#pragma once

#include <gfx.ecs/ecs.hpp>
#include "keys.hpp"
#include <glm/glm.hpp>
#include <mutex>
#include <atomic>

namespace gfx {
inline namespace v1 {
struct grabbed_cursor_component : gfx::ecs::component<grabbed_cursor_component>
{
    glm::dvec2 last_position;
    glm::dvec2 current_position{NAN, NAN};
    glm::dvec2 delta;
};

enum class cursor_state
{
    free,
    captured
};

struct mouse_movement
{
    void notify_moved(glm::vec2 c)
    {
        if (_block_move.exchange(false))
            return;
        std::unique_lock l(_mtx);
        moved += c;
    }

    void notify_not_moved()
    {
        _block_move = true;
    }

    void reset()
    {
        std::unique_lock l(_mtx);
        moved = { 0, 0 };
    }

    glm::vec2 moved{ 0 };

private:
    std::mutex _mtx;
    std::atomic_bool _block_move = false;
};

class input_system : public ecs::system
{
public:
    virtual ~input_system() = default;

    input_system()
    {
        add_component_type<grabbed_cursor_component>();
    }

    virtual bool key_down(key code) const = 0;
    virtual bool button_down(button code) const = 0;
    virtual glm::vec2 cursor_position() const = 0;
    virtual glm::vec2 cursor_delta() const = 0;
    virtual void set_cursor_state(cursor_state state) = 0;
    virtual cursor_state get_cursor_state() const noexcept = 0;

    void post_update() override
    {
        _position_last_update = { cursor_position() };
    }

    void update(duration_type delta, gfx::ecs::component_base** components) const override
    {
        auto& gcur = components[0]->as<gfx::grabbed_cursor_component>();
        if (get_cursor_state() == cursor_state::captured)
        {
            gcur.last_position = _position_last_update;
            gcur.current_position = cursor_position();
            gcur.delta = cursor_delta();
        }
        else
        {
            gcur.last_position = _position_last_update;
            gcur.current_position = _position_last_update;
            gcur.delta = gcur.current_position - gcur.last_position;
        }
    }

private:
    glm::dvec2 _position_last_update{0, 0};
};
}    // namespace v1
}    // namespace gfx