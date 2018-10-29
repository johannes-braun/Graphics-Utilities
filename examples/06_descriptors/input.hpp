#pragma once

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <gfx.ecs/ecs.hpp>

namespace gfx {
inline namespace v1 {
struct grabbed_cursor_component : gfx::ecs::component<grabbed_cursor_component>
{
    glm::dvec2 last_position;
    glm::dvec2 current_position{NAN, NAN};
    glm::dvec2 delta;
};

class key_event_filter : public QObject, public gfx::ecs::system
{
public:
    key_event_filter(QWidget* parent) : _parent(parent)
    {
        add_component_type<gfx::grabbed_cursor_component>(gfx::ecs::component_flag::optional);

        add_mouse_button_callback(Qt::MouseButton::RightButton, [&](QMouseEvent* ev) {
            if (ev->type() == QEvent::MouseButtonPress)
            {
                if (_parent->hasMouseTracking())
                {
                    _parent->releaseMouse();
                    _parent->releaseKeyboard();
                    _parent->setMouseTracking(false);
                    _parent->setCursor(QCursor(Qt::ArrowCursor));
                }
                else
                {
                    _parent->grabMouse();
                    _parent->grabKeyboard();
                    _parent->setMouseTracking(true);
                    _parent->setCursor(QCursor(Qt::BlankCursor));
                    QCursor cur = _parent->cursor();
                    cur.setPos(_parent->mapToGlobal(QPoint{_parent->width() / 2, _parent->height() / 2}));
                    _parent->setCursor(cur);
                    _position_last_update = {double(cur.pos().x()), double(cur.pos().y())};
                }
            }
        });
    }

    bool key_down(Qt::Key key) const
    {
        if (const auto it = _keys_down.find(key); it != _keys_down.end()) return it->second;
        return false;
    }

    void add_mouse_button_callback(Qt::MouseButton btn, std::function<void(QMouseEvent* event)> callback)
    {
        _mouse_button_callbacks[btn].push_back(callback);
    }

    void post_update() { 
		_moved = false;
		_position_last_update = { double(QCursor::pos().x()), double(QCursor::pos().y()) };
	}

    void update(double delta, gfx::ecs::component_base** components) const
    {
        if (_parent->hasMouseTracking())
        {
            gfx::grabbed_cursor_component* gcur = components[0]->as_ptr<gfx::grabbed_cursor_component>();

            if (gcur)
            {
                gcur->last_position    = _position_last_update;
                gcur->current_position = {double(QCursor::pos().x()), double(QCursor::pos().y())};
                gcur->delta            = gcur->current_position - gcur->last_position;
            }
        }
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* key                  = static_cast<QKeyEvent*>(event);
            _keys_down[Qt::Key(key->key())] = event->type() == QEvent::KeyPress;
            return true;
        }
        else if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* mm = static_cast<QMouseEvent*>(event);
            if (const auto it = _mouse_button_callbacks.find(mm->button()); it != _mouse_button_callbacks.end())
                for (auto& c : it->second) c(mm);
        }
        else if (event->type() == QEvent::MouseMove)
        {
            if (!_moved)
            {
                QCursor::setPos(_parent->mapToGlobal(QPoint{_parent->width() / 2, _parent->height() / 2}));
                _position_last_update = {double(QCursor::pos().x()), double(QCursor::pos().y())};
                _moved                = true;
            }
        }
        else
        {
            return QObject::eventFilter(obj, event);
        }
    }

    std::unordered_map<Qt::Key, bool>                                                         _keys_down;
    std::unordered_map<Qt::MouseButton, bool>                                                 _mouse_buttons_down;
    std::unordered_map<Qt::MouseButton, std::vector<std::function<void(QMouseEvent* event)>>> _mouse_button_callbacks;
    QWidget*                                                                                  _parent;

    bool       _moved = true;
    glm::dvec2 _position_last_update;
};

}    // namespace v1
}    // namespace gfx