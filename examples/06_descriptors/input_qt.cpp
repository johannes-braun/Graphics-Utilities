#include "input_qt.hpp"
#include <QEvent>
#include <QMouseEvent>

namespace gfx {
inline namespace v1 {

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
    std::atomic_bool _block_move;
};

qt_input_system::~qt_input_system()
{
    _timer->stop();
    if (_parent) _parent->removeEventFilter(this);
    delete _timer;
}

qt_input_system::qt_input_system(QWidget* parent) : _parent(parent)
{
    parent->installEventFilter(this);
    _timer = new QTimer;
    _mm = std::make_unique<mouse_movement>();
    QObject::connect(_timer, &QTimer::timeout, [&]
    {
        if (_cursor_state == cursor_state::captured)
        {
            const auto center = _parent->mapToGlobal(QPoint{ _parent->width() / 2, _parent->height() / 2 });
            auto x = QCursor::pos() - center;
            _mm->notify_moved({ x.x(), x.y() });
            QCursor::setPos(center);
        }
    });
    _timer->start();
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
    _mm->reset();
    _mm->notify_not_moved();
}

cursor_state qt_input_system::get_cursor_state() const noexcept
{
    return _cursor_state;
}

glm::vec2 qt_input_system::cursor_delta() const
{
    return _mm->moved;
}

void qt_input_system::post_update()
{
    _moved = false;
    _mm->reset();
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
            for (auto& c : it->second) 
                c(mm);

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
        }
    }
}
glm::vec2 qt_input_system::cursor_position() const
{
    return {QCursor::pos().x(), QCursor::pos().y()};
}
}    // namespace v1
}    // namespace gfx
