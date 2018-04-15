#include "window_manager.hpp"
#include <cassert>
#include <algorithm>
#include <gfx/file.hpp>

namespace gfx::ui
{
    window_manager::window_manager(const std::shared_ptr<gfx::window>& base)
        : _ui_draw_list(base), _base_window(base)
    {
        _base_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("ui/vs.vert");
        _base_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("ui/fs.frag");
    }

    void window_manager::bring_to_front(const std::wstring& name)
    {
        if (_windows.count(name) == 0)
            return;

        const int previous_layer = _windows[name].layer;
        for (auto&& pair : _windows)
        {
            if (pair.first == name)
                pair.second.layer = _windows.size() - 1;
            else
                pair.second.layer = std::max(pair.second.layer - 1, 0);
        }
    }

    window_info& window_manager::info(const std::wstring& name)
    {
        assert(_windows.count(name) != 0);
        return _windows[name];
    }

    window* window_manager::front_window_at(float x, float y)
    {
        int front_layer = -1;
        window* front_window = nullptr;
        for (const auto& pair : _windows)
        {
            if (pair.second.layer > front_layer && pair.second.window->get_rect().contains({ x, y }))
            {
                front_window = pair.second.window;
                front_layer = pair.second.layer;
            }
        }
        return front_window;
    }

    const std::wstring* window_manager::front_window_name_at(float x, float y)
    {
        int front_layer = -1;
        std::pair<const std::wstring, window_info>* front_window = nullptr;
        for (auto&& pair : _windows)
        {
            if (pair.second.layer > front_layer && pair.second.window->get_extended_rect().contains({ x, y }))
            {
                front_window = &pair;
                front_layer = pair.second.layer;
            }
        }
        return &(front_window->first);
    }

    window* window_manager::make_window(const std::wstring& name, rect initial_rect)
    {
        if (_windows.count(name) != 0)
            return _windows.at(name).window;

        window_info info;
        info.window = _window_storage.emplace_back(std::make_unique<window>(_base_window, this, name, initial_rect)).get();
        info.layer = _windows.size();
        _windows.emplace(name, info);
        return info.window;
    }

    window* window_manager::get_window(const std::wstring& name) const noexcept
    {
        return _windows.count(name) != 0 ? _windows.at(name).window : nullptr;
    }

    gfx::window& window_manager::get_base_window()
    {
        return *_base_window;
    }

    glm::vec2 window_manager::get_screen_size() const noexcept
    {
        int win_width, win_height; glfwGetFramebufferSize(*_base_window, &win_width, &win_height);
        return { win_width, win_height };
    }

    rect window_manager::get_maximal_rect() const noexcept
    {
        rect r;
        r.min ={ 0, 44 };
        r.max = get_screen_size();
        return r;
    }

    mouse_state window_manager::get_mouse_left_state() const noexcept
    {
        return _mouse_left_state;
    }

    glm::vec2 window_manager::get_cursor_position() const noexcept
    {
        double cursor_x, cursor_y; glfwGetCursorPos(*_base_window, &cursor_x, &cursor_y);
        int win_width, win_height; glfwGetFramebufferSize(*_base_window, &win_width, &win_height);
        cursor_y = win_height - cursor_y;
        return { cursor_x, cursor_y };
    }

    glm::vec2 window_manager::minimize_position() const noexcept
    {
        return _minimize_position;
    }

    void window_manager::notify_minimize(window* w)
    {
        _minimized.push_back(w);
        _minimize_position += glm::vec2(104+8+24, 0);
    }

    void window_manager::notify_unminimize(window* w)
    {
        auto it = std::find(_minimized.begin(), _minimized.end(), w);
        if (it != _minimized.end())
        {
            it = _minimized.erase(it);
            _minimize_position -= glm::vec2(104+8+24, 0);
        }

        for (; it < _minimized.end(); ++it)
        {
            (*it)->animate<glm::vec2>().to_position((**it).get_rect().min - glm::vec2(104+8+24, 0));
        }
    }

    void window_manager::render()
    {
        static std::map<uint32_t, GLFWcursor*> curs = []() {
            std::map<uint32_t, GLFWcursor*> mp;
            mp[0] = nullptr;
            mp[GLFW_HAND_CURSOR] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
            mp[GLFW_HRESIZE_CURSOR] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
            mp[GLFW_VRESIZE_CURSOR] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
            return mp;
        }();
        glm::vec2 cursor = get_cursor_position();
        glm::vec2 scrsize = get_screen_size();

        switch (_mouse_left_state)
        {
        case MOUSE_UP:
            if (glfwGetMouseButton(*_base_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                _mouse_left_state = MOUSE_PRESS;
            break;
        case MOUSE_PRESS:
            _mouse_left_state = MOUSE_DOWN;
            break;
        case MOUSE_DOWN:
            if (glfwGetMouseButton(*_base_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                _mouse_left_state = MOUSE_RELEASE;
            break;
        case MOUSE_RELEASE:
            _mouse_left_state = MOUSE_UP;
            _action_window = nullptr;
            break;
        }

        const auto* front_window_name = front_window_name_at(cursor.x, cursor.y);
        window* focussed = front_window_name ? get_window(*front_window_name) : nullptr;

        bool change_cursor = false;
        if (focussed)
        {
            if (focussed->grip_top().contains(cursor) || focussed->grip_bottom().contains(cursor))
            {
                glfwSetCursor(*_base_window, curs[GLFW_VRESIZE_CURSOR]);
                change_cursor = true;
            }
            else if (focussed->grip_left().contains(cursor) || focussed->grip_right().contains(cursor))
            {
                glfwSetCursor(*_base_window, curs[GLFW_HRESIZE_CURSOR]);
                change_cursor = true;
            }
        }
        if (!change_cursor)
            glfwSetCursor(*_base_window, curs[0]);

        if (_mouse_left_state == MOUSE_PRESS)
        {
            if (front_window_name != nullptr)
            {
                bring_to_front(*front_window_name);

                if (focussed->state() == WIN_NORMAL)
                {
                    if (focussed->grip_top().contains(cursor))
                    {
                        _action_window = focussed;
                        _action =[&, this, action_pos = cursor]() mutable {
                            _action_window->resize_by({ 0, cursor.y - action_pos.y });
                            action_pos = cursor;
                        };
                    }
                    else if (focussed->grip_right().contains(cursor))
                    {
                        _action_window = focussed;
                        _action =[&, this, action_pos = cursor]() mutable {
                            _action_window->resize_by({ cursor.x - action_pos.x, 0 });
                            action_pos = cursor;
                        };
                    }
                    else if (focussed->grip_bottom().contains(cursor))
                    {
                        _action_window = focussed;
                        _action =[&, this, action_pos = cursor]() mutable {
                            _action_window->move_by({ 0, cursor.y-action_pos.y });
                            _action_window->resize_by({ 0, action_pos.y-cursor.y });
                            action_pos = cursor;
                        };
                    }
                    else if (focussed->grip_left().contains(cursor))
                    {
                        _action_window = focussed;
                        _action =[&, this, action_pos = cursor]() mutable {
                            _action_window->move_by({ cursor.x-action_pos.x, 0 });
                            _action_window->resize_by({ action_pos.x-cursor.x, 0 });
                            action_pos = cursor;
                        };
                    }
                    else if (focussed->title_area().contains(cursor))
                    {
                        _action_window = focussed;
                        _action = [&, this, action_pos = cursor]() mutable {
                            _action_window->move_by(cursor - action_pos);
                            action_pos = cursor;
                        };
                    }
                }
            }
        }

        if (_action_window && _action)
            _action();

        static gl::texture window_icon = []() {
            const gfx::image_file img("../res/ui/logo.svg", 1.f);
            gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
            ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
            return ico;
        }();

        _base_pipeline.bind();
        _ui_draw_list.push_rounded_quad({ -8, -8 }, { 51, 52 }, { 0, 0 }, { 1, 1 }, { 0, 0, 0, 80 }, { 0,0,0,0 }, 12, CORNER_ALL);
        _ui_draw_list.push_rounded_quad({ -8, -8 }, { 48, 48 }, { 0, 0 }, { 1, 1 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, 8, CORNER_ALL);
        _ui_draw_list.push_quad({ 8, 8 }, { 40, 40 }, { 0, 1 }, { 1, 0 }, { 255, 255, 255, 255 }, {}, GRADIENT_NONE, window_icon);
        
        struct comparator { bool operator()(const std::pair<int, window*>& lhs, const std::pair<int, window*>& rhs) { return lhs.first > rhs.first; }; };
        std::priority_queue<std::pair<int, window*>, std::vector<std::pair<int, window*>>, comparator> queue;

        for (auto&& data : _windows)
        {
            // Only a pointer check, because a wstring check would be unnecessarily slow.
            data.second.window->set_contains_cursor(front_window_name && front_window_name == &data.first);
            queue.emplace(data.second.layer, data.second.window);
        }

        _base_pipeline.bind();
        _ui_draw_list.draw(_base_pipeline);
        while (!queue.empty())
        {
            queue.top().second->list().draw(_base_pipeline);
            queue.pop();
        }
    }
}