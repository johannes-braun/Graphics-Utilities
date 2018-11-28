#pragma once

#include "input.hpp"
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <glm/vec2.hpp>

namespace gfx
{
    inline namespace v1
    {
        class qt_input_system : public QObject, public input_system
        {
        public:
            ~qt_input_system();

            qt_input_system(QWidget* parent);

            qt_input_system(const qt_input_system&) = delete;
            qt_input_system& operator=(const qt_input_system&) = delete;
            qt_input_system(qt_input_system&&) = default;
            qt_input_system& operator=(qt_input_system&&) = default;

            bool key_down(key code) const override;
            bool button_down(button code) const override;
            void set_cursor_state(cursor_state state) override;
            glm::vec2 cursor_position() const override;
            cursor_state get_cursor_state() const noexcept override;

            glm::vec2 cursor_delta() const override;
            void post_update() override;

        protected:
            void add_mouse_button_callback(Qt::MouseButton btn, std::function<void(QMouseEvent* event)> callback);
            bool eventFilter(QObject* obj, QEvent* event) override;

            mouse_movement _mm;
            QTimer* _timer;
            cursor_state _last_cursor_state = cursor_state::free;
            std::atomic<cursor_state> _cursor_state = cursor_state::free;
            std::unordered_map<Qt::Key, bool>                                                         _keys_down;
            std::unordered_map<Qt::MouseButton, bool>                                                 _mouse_buttons_down;
            std::unordered_map<Qt::MouseButton, std::vector<std::function<void(QMouseEvent* event)>>> _mouse_button_callbacks;
            QWidget*                                                                                  _parent;

            bool       _moved = true;
            glm::dvec2 _position_last_update{};

            void set_state();
        };
    }
}
