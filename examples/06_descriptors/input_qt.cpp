#include "input_qt.hpp"
#include <QEvent>
#include <QMouseEvent>

namespace gfx {
inline namespace v1 {
qt_input_system::~qt_input_system()
{
    if (_parent) _parent->removeEventFilter(this);
}

qt_input_system::qt_input_system(QWidget* parent) : _parent(parent)
{
    parent->installEventFilter(this);
}

bool qt_input_system::key_down(key code) const
{
    if (const auto it = _keys_down.find(Qt::Key(code)); it != _keys_down.end()) return it->second;
    return false;
}

bool qt_input_system::button_down(button code) const
{
    if (const auto it = _mouse_buttons_down.find(Qt::MouseButton(code)); it != _mouse_buttons_down.end()) return it->second;
    return false;
}

void qt_input_system::set_cursor_state(cursor_state state)
{
    _cursor_state = state;
}

cursor_state qt_input_system::get_cursor_state() const noexcept
{
    return _cursor_state;
}

void qt_input_system::post_update()
{
    _moved = false;
    input_system::post_update();
}

void qt_input_system::add_mouse_button_callback(Qt::MouseButton btn, std::function<void(QMouseEvent* event)> callback)
{
    _mouse_button_callbacks[btn].push_back(callback);
}

bool qt_input_system::eventFilter(QObject* obj, QEvent* event)
{
    set_state();
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* key                  = static_cast<QKeyEvent*>(event);
        _keys_down[Qt::Key(key->key())] = event->type() == QEvent::KeyPress;
        return true;
    }
    else if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mm                   = static_cast<QMouseEvent*>(event);
        _mouse_buttons_down[mm->button()] = event->type() == QEvent::MouseButtonPress;
        if (const auto it = _mouse_button_callbacks.find(mm->button()); it != _mouse_button_callbacks.end())
            for (auto& c : it->second) c(mm);
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!_moved)
        {
            QCursor::setPos(_parent->mapToGlobal(QPoint{_parent->width() / 2, _parent->height() / 2}));
            _position_last_update = {double(QCursor::pos().x()), double(QCursor::pos().y())};
            _moved                = true;
        }
        return true;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}

void qt_input_system::set_state()
{
    if (_last_cursor_state != _cursor_state)
    {
        _last_cursor_state = _cursor_state;
        if (_cursor_state == cursor_state::free)
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
}
glm::vec2 qt_input_system::cursor_position() const
{
    return {QCursor::pos().x(), QCursor::pos().y()};
}
}    // namespace v1
}    // namespace gfx
