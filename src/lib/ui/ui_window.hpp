#include "ui.hpp"
#include <io/window.hpp>
#include "ui_button.hpp"
#include <memory>

#include "window_manager.hpp"

namespace game
{
    constexpr float STACK_WIDTH = 104;
/*
    class ui_window;

    class ui_layouter
    {
    public:

        glm::vec2 win_stack_position{ 0, 0 };

        void notify_minimized(ui_window* win);
        void notify_unminimized(ui_window* win);

        void bring_forward(ui_window* win);
        bool is_at_front(ui_window* win);
        void register_ifnex(ui_window* win);

        std::vector<ui_window*> minimized;
        std::map<ui_window*, int> prio_map;

    private:

    };*/

    class ui_window
    {
    public:
        constexpr static float title_bar_height = 32;
        constexpr static float status_bar_height = 24;

        glm::vec2 position() const noexcept
        {
            return _pos;
        }

        glm::vec2 size() const noexcept
        {
            return _size;
        }

        ui_window(const std::shared_ptr<gfx::ui::window_manager>& manager, const std::wstring& title, glm::vec2 pos, glm::vec2 size, glm::u8vec4 background, glm::u8vec4 title_bar_color, glm::vec4 padding ={ 4, 4, 4, 4 })
            : _title(title), _manager(manager), _pos(pos), _size(size), _background(background), _title_bar_color(title_bar_color), _scroll_offset{ 0 }, _padding(padding)
        {
            _ui = &default_ui();
            _scroll_callback = _manager->get_window().callbacks->scroll_callback.add([&](GLFWwindow* w, double x, double y) {
                double cx, cy;  glfwGetCursorPos(w, &cx, &cy);
                int wi, he;     glfwGetFramebufferSize(w, &wi, &he);
                cy = he - cy;
                if (glm::clamp(glm::vec2(cx, cy), _pos, _pos + _size) == glm::vec2(cx, cy))
                {
                    _scroll_offset = max(glm::vec2(0), glm::min(_scroll_offset - 40 * glm::vec2(x, y), _insert_position - _size + _padding.y + _padding.w + title_bar_height + status_bar_height));
                }
            });
        }

        ~ui_window()
        {
            _manager->get_window().->callbacks->scroll_callback.remove(_scroll_callback);
        }

        struct anim
        {
            anim& to_pos(glm::vec2 pos)
            {
                _w._anim_to_pos = pos;
                return *this;
            }

            anim& to_size(glm::vec2 size)
            {
                _w._anim_to_size = size;
                return *this;
            }

            anim& with_end(std::function<void()> action)
            {
                _w._on_anim_end = action;
                return *this;
            }

            anim(ui_window& w) : _w(w) {}
            ~anim() { _w._run_anim = true; }

        private: ui_window& _w;
        };

        anim animate()
        {
            _on_anim_end = []() {};
            return anim(*this);
        }

        bool cursor_in_frame()
        {
            int wi, he; glfwGetFramebufferSize(_manager->get_window(), &wi, &he);
            double cx, cy; glfwGetCursorPos(_manager->get_window(), &cx, &cy);
            cy = he - cy;
            glm::vec2 bmin = _pos;
            glm::vec2 bmax = _pos + _size;
            return glm::clamp(glm::vec2(cx, cy), bmin, bmax) == glm::vec2(cx, cy);
        }
/*
        bool is_topmost(ui_layouter& lo)
        {
            if (_minimized)
                return true;
            if (!cursor_in_frame())
                return false;
            bool topmost = true;
            for (auto&& other : lo.prio_map)
            {
                if (other.second > frame_priority_layer && other.first->cursor_in_frame())
                {
                    topmost = false;
                    break;
                }
            }
            return topmost;
        }*/

        bool lock_click = false;
        bool _block_actions = false;
        
        void draw_content(const font& title_font, const font& status_bar_font, std::function<void(ui_window& window, const glm::vec2& bmin, const glm::vec2& bmax)> draw_fun)
        {
            if (!_open)
                return;


            int wi, he; glfwGetFramebufferSize(_manager->get_window(), &wi, &he);
            double cx, cy; glfwGetCursorPos(_manager->get_window(), &cx, &cy);
            cy = he - cy;

            _pos = glm::clamp(_pos, glm::vec2(0, 0), glm::vec2(wi, he) - glm::clamp(_size, glm::vec2(STACK_WIDTH, title_bar_height), glm::vec2(wi, he)));
            _size = glm::clamp(_size, glm::vec2(STACK_WIDTH, title_bar_height), glm::vec2(wi, he) - _pos);

            glm::vec2 bmin = _pos;
            glm::vec2 bmax = _pos + _size;

            float handle_width = 16.f;
            lo.register_ifnex(this);
            frame_priority_layer = lo.prio_map[this];
            _ui->push_list(frame_priority_layer, this);
            priority_layer = 0;

            _block_actions = (_manager->front_window_at(cx, cy) != this);
            if (_minimized || _maximized || lock_click || !is_topmost(lo))
            {
                _block_transform = true;
            }

            if (_resize_state != -1)
            {
                if (_resize_state == 0)
                {
                    _ui->set_cursor(CUR_HRESIZE);
                    bmax.x -= (_action_cursor_position.x - cx);
                    _action_cursor_position.x = cx;
                }
                if (_resize_state == 1)
                {
                    _ui->set_cursor(CUR_VRESIZE);
                    bmin.y -= (_action_cursor_position.y - cy);
                    _action_cursor_position.y = cy;
                }
                _smooth_scroll_offset = glm::max(glm::vec2(0), glm::min(_smooth_scroll_offset, _insert_position - _size + _padding.y + _padding.w));
                _scroll_offset = _smooth_scroll_offset;
                if (glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                    _resize_state = -1;
            }
            else if (_move_state != -1)
            {
                _ui->set_cursor(CUR_NORMAL);
                const auto sz = bmax - bmin;
                bmin -= (_action_cursor_position - glm::vec2{ cx, cy });
                bmax = bmin + sz;
                _action_cursor_position ={ cx, cy };
                if (glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                    _move_state = -1;
            }
            else if (!_block_transform && glm::clamp(glm::vec2(cx, cy), bmin + glm::vec2(0, handle_width*0.5f), bmax-glm::vec2(handle_width*0.5f, 0)) != glm::vec2(cx, cy) &&
                glm::clamp(glm::vec2(cx, cy), bmin - glm::vec2(0, handle_width*0.5f), bmax + glm::vec2(handle_width*0.5f, 0)) == glm::vec2(cx, cy))
            {
                if (cx > bmax.x - 4.f)
                {
                    _ui->set_cursor(CUR_HRESIZE);
                    if (glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && (_manager->front_window_at(cx, cy) != this))
                    {
                        lo.bring_forward(this);
                        _action_cursor_position.x = cx;
                        _resize_state = 0;
                    }
                }
                else if (cy < bmin.y + 4.f)
                {
                    _ui->set_cursor(CUR_VRESIZE);
                    if (glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && (_manager->front_window_at(cx, cy) != this))
                    {
                        lo.bring_forward(this);
                        _action_cursor_position.y = cy;
                        _resize_state = 1;
                    }
                }
            }
            else if (!_block_transform && glm::clamp(glm::vec2(cx, cy), glm::vec2(bmin.x, bmax.y - title_bar_height), bmax) == glm::vec2(cx, cy))
            {
                _ui->set_cursor(CUR_NORMAL);
                if (glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    lo.bring_forward(this);
                    _action_cursor_position ={ cx, cy };
                    _move_state = 1;
                }
            }
            
            if (!lock_click && glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                if ((_manager->front_window_at(cx, cy) != this))
                    lo.bring_forward(this);
                lock_click = true;
            }
            else if(glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                lock_click = false;

            if (_resize_state == -1 && _move_state==-1 && glfwGetMouseButton(_manager->get_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                _block_transform = true;
            }
            else
            {
                _block_transform = false;
            }

            if (glm::any(glm::lessThan(bmax, bmin)))
            {
                bmin = bmax;
            }

            _action_cursor_position = glm::clamp(_action_cursor_position, glm::vec2(0, 0), glm::vec2(wi, he));

            _pos = bmin;
            _size = bmax - bmin;
            _pos = glm::clamp(_pos, glm::vec2(0, 0), glm::vec2(wi, he) - glm::clamp(_size, glm::vec2(STACK_WIDTH, title_bar_height), glm::vec2(wi, he)));
            _size = glm::clamp(_size, glm::vec2(STACK_WIDTH, title_bar_height), glm::vec2(wi, he) - _pos);
            bmin = _pos;
            bmax = _pos + _size;

            _insert_position ={ 0,0 };
            _ui->scissor({ 0, 0 }, { wi, he });
            if (!_minimized)
            {
                _ui->draw_quad_shadow(bmin, bmax, { 0,0,0,_alpha * 60 });
                _ui->scissor(bmin, bmax);
                _ui->draw_quad(bmin, bmax, _background);
            }

            if (!_minimized)
            {
                const auto inset = _ui->inset(bmin, bmax, _padding);
                draw_fun(*this, inset.first, inset.second);

                _ui->scissor(bmin, bmax);

                // status bar
                _ui->draw_quad_shadow(bmin, { bmax.x, bmin.y + status_bar_height }, { 0,0,0,64 });
                _ui->scissor(bmin, { bmax.x, std::clamp(bmin.y + status_bar_height, bmin.y, bmax.y) });
                _ui->draw_quad(bmin, { bmax.x, bmin.y + status_bar_height }, { 0xff, 0xff, 0xff, 0xff });
                _ui->draw_text(_status, status_bar_font, 0, { bmin.x+8, bmin.y + 0.5f*(status_bar_height - status_bar_font.size()) }, { bmax.x-8, bmin.y + 0.5f*(status_bar_height + status_bar_font.size()) }, ui::ALIGN_LEFT, { 0, 0, 0, 128 }, 1);
                _ui->scissor(bmin, bmax);

                // title bar shadow
                _ui->draw_quad_shadow({ bmin.x, bmax.y - title_bar_height }, bmax, { 0,0,0,_alpha * 64 });
            }

            // title bar
            _ui->scissor({ bmin.x, std::clamp(bmax.y - title_bar_height, bmin.y, bmax.y) }, bmax);
            _ui->draw_quad({ bmin.x, bmax.y - title_bar_height }, bmax, _title_bar_color);
            _ui->draw_quad({ bmin.x + 8, bmax.y - title_bar_height * 0.5f - 12 }, { bmin.x + 8 + 24, bmax.y - title_bar_height * 0.5f + 12 }, { 0, 1 }, { 1, 0 }, _icon);
            if (!_minimized)
                _ui->draw_text(_title, title_font, 0, { bmin.x + 8 + 24 + 8, bmax.y - 0.5f*(title_bar_height + title_font.size()) }, { bmax.x, bmax.y - 0.5f*(title_bar_height - title_font.size()) }, ui::ALIGN_LEFT, { 255, 255, 255, uint8_t(_alpha * 255) }, 1);

            //title bar actions
            if (_close_button)
            {
                const auto state = _close_button->draw_inside(_manager->get_window(), _ui, { bmax.x - 8 - 24, bmax.y - title_bar_height * 0.5f - 12 }, { bmax.x - 8, bmax.y - title_bar_height * 0.5f + 12 });
                if (state == STATE_RELEASE)
                {
                    if (!_block_actions)
                    {
                        log_i << "Closed.";
                        if (_minimized)
                        {
                            lo.notify_unminimized(this);
                        }
                        animate().to_size({ STACK_WIDTH, title_bar_height }).to_pos(_pos + _size - glm::vec2(STACK_WIDTH, title_bar_height)).with_end([this]() {
                            _open = false;
                        });
                    }
                    _close_button->set_state(STATE_UP);
                }
            }
            if (_maximize_button)
            {
                const auto state = _maximize_button->draw_inside(_manager->get_window(), _ui, { bmax.x - 8 - 24-8-24, bmax.y - title_bar_height * 0.5f - 12 }, { bmax.x - 8-24-8, bmax.y - title_bar_height * 0.5f + 12 });
                if (state == STATE_RELEASE)
                {
                    if (!_block_actions)
                    {
                        if (_minimized || _maximized)
                        {
                            if (_minimized)
                            {
                                lo.win_stack_position.x -= STACK_WIDTH;
                                if (lo.win_stack_position.x < 0)
                                {
                                    lo.win_stack_position.y -= title_bar_height;
                                    lo.win_stack_position.x = ((wi - int(STACK_WIDTH)) / int(STACK_WIDTH)) * STACK_WIDTH;
                                }
                                lo.notify_unminimized(this);
                            }

                            _anim_to_size = _size_before_action;
                            _anim_to_pos = _pos_before_action;
                            _anim_to_alpha = 1.f;
                            _minimized = false;
                            _maximized = false;
                            _run_anim = true;
                            _on_anim_end = []() {};
                        }
                        else
                        {
                            _pos_before_action = _pos;
                            _size_before_action = _size;
                            _anim_to_size ={ wi, he };
                            _anim_to_pos ={ 0, 0 };
                            _run_anim = true;
                            _on_anim_end = [this]() { _maximized = true; };
                        }
                        log_i << "Maximized.";
                    }
                    _maximize_button->set_state(STATE_UP);
                }
            }
            if (!_minimized && _minimize_button)
            {
                const auto state = _minimize_button->draw_inside(_manager->get_window(), _ui, { bmax.x - 8 - 24-8-24-8-24, bmax.y - title_bar_height * 0.5f - 12 }, { bmax.x - 8-24-8-24-8, bmax.y - title_bar_height * 0.5f + 12 }, _alpha);
                if (state == STATE_RELEASE)
                {
                    if (!_block_actions)
                    {
                        log_i << "Minimized.";
                        _pos_before_action = _pos;
                        _size_before_action = _size;
                        _anim_to_alpha = 0.f;
                        _anim_to_size ={ STACK_WIDTH, title_bar_height };
                        _anim_to_pos =lo.win_stack_position;
                        if (lo.win_stack_position.x + _anim_to_size.x + STACK_WIDTH > wi)
                        {
                            lo.win_stack_position ={ 0, lo.win_stack_position.y + title_bar_height };
                        }
                        else
                        {
                            lo.win_stack_position.x += _anim_to_size.x;
                        }
                        _run_anim = true;
                        _on_anim_end = [&, this]() {
                            _minimized = true; lo.notify_minimized(this);
                        };
                    }
                    _minimize_button->set_state(STATE_UP);
                }
            }
            _pos = bmin;
            _size = bmax - bmin;

            if (_run_anim)
            {
                _size = glm::mix(_size, _anim_to_size, 30.f * _window->delta_time());
                _pos = glm::mix(_pos, _anim_to_pos, 30.f * _window->delta_time());
                _alpha = glm::mix<float>(_alpha, _anim_to_alpha, 30.f * _window->delta_time());
                if (glm::distance2(_size, _anim_to_size) < 0.01f && glm::distance2(_pos, _anim_to_pos) < 0.01f)
                {
                    _size = _anim_to_size;
                    _pos = _anim_to_pos;
                    _alpha = _anim_to_alpha;
                    _run_anim = false;
                    if (_on_anim_end) _on_anim_end();
                }
            }

            _pos = glm::clamp(_pos, glm::vec2(0, 0), glm::vec2(wi, he) - glm::clamp(_size, glm::vec2(STACK_WIDTH, title_bar_height), glm::vec2(wi, he)));
            _size = glm::clamp(_size, glm::vec2(STACK_WIDTH, title_bar_height), glm::vec2(wi, he) - _pos);

            _smooth_scroll_offset = glm::mix(_smooth_scroll_offset, _scroll_offset, float(10.f * _window->delta_time()));
            _ui->scissor({ 0, 0 }, { wi, he });
            _ui->pop_list();
        }

        void reset_draw_state()
        {
            _same_line = false;
            _no_inset = false;
        }

        void next_same_line()
        {
            _same_line = true;
        }

        void next_no_inset()
        {
            _no_inset = true;
        }

        void spacing(float height = 10.f)
        {
            _last_insert_position.y += height;
            _insert_position.y += height;
            reset_draw_state();
        }

        void set_status(const std::wstring& status)
        {
            _status = status;
        }

        void set_icon(const gl_texture_t icon)
        {
            _icon = icon;
        }

        void set_close_icon(const gl_texture_t icon)
        {
            _close_button = std::make_unique<ui_button>(icon, glm::u8vec4{ 0xff, 0xff, 0xff, 0xff });
        }

        void set_maximize_icon(const gl_texture_t icon)
        {
            _maximize_button = std::make_unique<ui_button>(icon, glm::u8vec4{ 0xff, 0xff, 0xff, 0xff });
        }

        void set_minimize_icon(const gl_texture_t icon)
        {
            _minimize_button = std::make_unique<ui_button>(icon, glm::u8vec4{ 0xff, 0xff, 0xff, 0xff });
        }

        void draw_text(const std::wstring& text, const font& font, ui::text_align align, glm::u8vec4 color ={ 255, 255, 255, 255 }, int max_lines = std::numeric_limits<int>::max())
        {
            std::pair<glm::vec2, glm::vec2> inset = _ui->inset(_pos, _pos+_size, _padding);
            inset.first.y += status_bar_height;
            inset.second.y -= title_bar_height;

            if (_no_inset)
            {
                inset.first.x = _pos.x;
                inset.second.x = (_pos+_size).x;
                if (_insert_position.y == 0)
                {
                    inset.first.y = _pos.y+status_bar_height;
                    inset.second.y = (_pos+_size).y-title_bar_height;
                }
            }

            if (!_same_line)
            {
                _last_insert_position.x = 0;
                _last_insert_position.y = _insert_position.y;
            }
            else
            {
                _last_insert_position.x = _insert_position.x;
            }
            _insert_position = _last_insert_position;
            _insert_position += _ui->draw_text(text, font, _smooth_scroll_offset.y - _last_insert_position.y, { inset.first.x + _last_insert_position.x, inset.first.y }, inset.second, align, color, max_lines);
            if (align & ui::ALIGN_RIGHT)
                _insert_position.x = inset.second.x;
            reset_draw_state();
        }

        glm::vec2 content_size(bool with_insets = true)
        {
            auto inset = with_insets ? _ui->inset(_pos, _pos+_size, _padding) : std::make_pair(_pos, _pos+_size);
            inset.first.y += status_bar_height;
            inset.second.y -= title_bar_height;
            return inset.second - inset.first;
        }

        void draw_image(gl_texture_t texture, glm::vec2 size, glm::vec2 uv0 ={ 0, 1 }, glm::vec2 uv1={ 1,0 }, glm::u8vec4 tint ={ 0xff, 0xff, 0xff, 0xff })
        {
            std::pair<glm::vec2, glm::vec2> inset = _ui->inset(_pos, _pos+_size, _padding);
            inset.first.y += status_bar_height;
            inset.second.y -= title_bar_height;

            if (_no_inset)
            {
                inset.first.x = _pos.x;
                inset.second.x = (_pos+_size).x;
                if (_insert_position.y == 0)
                {
                    inset.first.y = _pos.y+status_bar_height;
                    inset.second.y = (_pos+_size).y-title_bar_height;
                }
            }

            if (!_same_line)
            {
                _last_insert_position.x = 0;
                _last_insert_position.y = _insert_position.y;
            }
            else
            {
                _last_insert_position.x = _insert_position.x;
            }
            _insert_position = _last_insert_position;
            _insert_position += size;

            int twidth, theight;
            glGetTextureLevelParameteriv(texture, 0, GL_TEXTURE_WIDTH, &twidth);
            glGetTextureLevelParameteriv(texture, 0, GL_TEXTURE_HEIGHT, &theight);

            float texture_aspect = twidth / float(theight);
            float size_aspect = size.x / size.y;

            uv0.x *= size_aspect / texture_aspect;
            uv1.x *= size_aspect / texture_aspect;
            uv0.y /= size_aspect / texture_aspect;
            uv1.y /= size_aspect / texture_aspect;

            uv0.x = std::clamp(uv0.x, 0.f, 1.f);
            uv1.x = std::clamp(uv1.x, 0.f, 1.f);
            uv0.y = std::clamp(uv0.y, 0.f, 1.f);
            uv1.y = std::clamp(uv1.y, 0.f, 1.f);

            const float sx = abs(uv0.x - uv1.x);
            const float sy = abs(uv0.y - uv1.y);
            if (sx < 1)
            {
                uv0.x += (1-sx)*0.5f;
                uv1.x += (1-sx)*0.5f;
            }
            if (sy < 1)
            {
                uv0.y += (1-sy)*0.5f;
                uv1.y += (1-sy)*0.5f;
            }

            glm::vec2 left_top ={ _last_insert_position.x, _last_insert_position.y };
            glm::vec2 right_bottom ={ left_top.x + size.x, left_top.y - size.y };
            glm::vec2 min ={ inset.first.x + left_top.x, inset.second.y - _last_insert_position.y -size.y };
            glm::vec2 max ={ inset.first.x + right_bottom.x, inset.second.y - _last_insert_position.y };
            _ui->draw_quad(min + _smooth_scroll_offset, max + _smooth_scroll_offset, uv0, uv1, texture, tint);
            reset_draw_state();
        }

        ui_button_state draw_button(ui_button& button, glm::vec2 size)
        {
            std::pair<glm::vec2, glm::vec2> inset = _ui->inset(_pos, _pos+_size, _padding);
            inset.first.y += status_bar_height;
            inset.second.y -= title_bar_height;

            if (_no_inset)
            {
                inset.first.x = _pos.x;
                inset.second.x = (_pos+_size).x;
                if (_insert_position.y == 0)
                {
                    inset.first.y = _pos.y+status_bar_height;
                    inset.second.y = (_pos+_size).y-title_bar_height;
                }
            }

            if (!_same_line)
            {
                _last_insert_position.x = 0;
                _last_insert_position.y = _insert_position.y;
            }
            else
            {
                _last_insert_position.x = _insert_position.x;
            }
            _insert_position = _last_insert_position;
            _insert_position += size;
            glm::vec2 left_top ={ _last_insert_position.x, _last_insert_position.y };
            glm::vec2 right_bottom ={ left_top.x + size.x, left_top.y - size.y };
            glm::vec2 min ={ inset.first.x + left_top.x, inset.second.y - _last_insert_position.y -size.y };
            glm::vec2 max ={ inset.first.x + right_bottom.x, inset.second.y - _last_insert_position.y };
            const auto state = button.draw_inside(_manager->get_window(), _ui, min + _smooth_scroll_offset, max + _smooth_scroll_offset);
            reset_draw_state();
            return _block_actions ? STATE_UP : state;
        }

        int frame_priority_layer = 0;
        int priority_layer = 0;

    private:
        std::shared_ptr<gfx::ui::window_manager> _manager;

        bool _minimized = false;
        bool _maximized = false;

        bool _run_anim=false;
        std::function<void()> _on_anim_end;
        glm::vec2 _anim_to_pos;
        glm::vec2 _anim_to_size;

        float _anim_to_alpha = 0;
        float _alpha = 1;

        glm::vec2 _pos_before_action;
        glm::vec2 _size_before_action;

        glm::u8vec4 _title_bar_color;
        bool _open = true;
        bool _block_transform = false;
        std::unique_ptr<ui_button> _close_button;
        std::unique_ptr<ui_button> _minimize_button;
        std::unique_ptr<ui_button> _maximize_button;
        gl_texture_t _icon = gl_texture_t::zero;
        std::wstring _title;
        std::wstring _status = L"OK";
        friend ui_layouter;
        //std::shared_ptr<io::window> _window;
        std::function<void(GLFWwindow* w, double x, double y)>* _scroll_callback;
        ui* _ui;
        glm::vec2 _scroll_offset;
        glm::vec2 _smooth_scroll_offset;
        glm::vec2 _pos;
        glm::vec2 _size;
        glm::vec2 _last_insert_position;
        glm::vec2 _insert_position;
        bool _same_line = false;
        bool _no_inset = false;
        glm::vec4 _padding;
        glm::u8vec4 _background;


        int _resize_state = -1;
        int _move_state = -1;
        glm::vec2 _action_cursor_position;
    };

    inline bool ui_layouter::is_at_front(ui_window* win)
    {
        return prio_map.count(win) != 0 && prio_map[win] == prio_map.size() - 1;
    }

    inline void ui_layouter::register_ifnex(ui_window* win)
    {
        if (prio_map.count(win) == 0)
            prio_map[win] = prio_map.size() -1;
    }

    inline void ui_layouter::bring_forward(ui_window* win)
    {
        if (is_at_front(win) || !win->is_topmost(*this)) return;
        const int last = prio_map[win];
        prio_map[win] = prio_map.size() - 1;
        for (auto&& item : prio_map)
        {
            if (item.first != win && item.second > last)
            {
                item.second = std::max(0, item.second-1);
            }
        }
    }

    inline void ui_layouter::notify_minimized(ui_window* win)
    {
        minimized.push_back(win);
    }

    inline void ui_layouter::notify_unminimized(ui_window* win)
    {
        auto it = std::find(minimized.begin(), minimized.end(), win);
        if (it == minimized.end())
            return;
        it = minimized.erase(it);
        for (; it != minimized.end(); ++it)
        {
            glm::vec2 pos = (*it)->position();
            pos.x -= STACK_WIDTH;
            if (pos.x < -STACK_WIDTH / 2.f)
            {
                int wi, he; glfwGetFramebufferSize(*(*it)->_window, &wi, &he);
                pos.y -= ui_window::title_bar_height;
                pos.x = ((wi - int(STACK_WIDTH)) / int(STACK_WIDTH)) * STACK_WIDTH;
            }

            (*it)->animate().to_pos(pos);
        }
    }
}