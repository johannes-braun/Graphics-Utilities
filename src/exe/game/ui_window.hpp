#include "ui.hpp"
#include <io/window.hpp>
#include "ui_button.hpp"
#include <memory>

namespace game
{
    constexpr float STACK_WIDTH = 104;

    class ui_window;

    class ui_layouter
    {
    public:

        glm::vec2 win_stack_position{ 0, 0 };

        void notify_minimized(ui_window* win);
        void notify_unminimized(ui_window* win);
        std::vector<ui_window*> minimized;
    private:

    };

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

        ui_window(std::shared_ptr<io::window> win, const std::wstring& title, glm::vec2 pos, glm::vec2 size, glm::u8vec4 background, glm::u8vec4 title_bar_color, glm::vec4 padding = {4, 4, 4, 4})
            : _title(title), _window(win), _pos(pos), _size(size), _background(background), _title_bar_color(title_bar_color), _scroll_offset{0}, _padding(padding)
        {
            _ui = &default_ui();
            _scroll_callback = _window->callbacks->scroll_callback.add([&](GLFWwindow* w, double x, double y) {
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
            if(_window)
                _window->callbacks->scroll_callback.remove(_scroll_callback);
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

        void draw_content(ui_layouter& lo, const font& title_font, const font& status_bar_font, std::function<void(ui_window& window, const glm::vec2& bmin, const glm::vec2& bmax)> draw_fun)
        {         
            if (!_open)
                return;

            glm::vec2 bmin = _pos;
            glm::vec2 bmax = _pos + _size;

            int wi, he; glfwGetFramebufferSize(*_window, &wi, &he);
            double cx, cy; glfwGetCursorPos(*_window, &cx, &cy);
            cy = he - cy;

            float handle_width = 16.f;

            if (_minimized)
                _block_transform = true;

            if (_resize_state != -1)
            {
                if (_resize_state == 0)
                {
                    _ui->set_cursor(CUR_HRESIZE);
                    // X
                    bmax.x -= (_action_cursor_position.x - cx);
                    _action_cursor_position.x = cx;
                }
                if (_resize_state == 1)
                {
                    _ui->set_cursor(CUR_VRESIZE);
                    //y
                    bmin.y -= (_action_cursor_position.y - cy);
                    _action_cursor_position.y = cy;
                }
                _smooth_scroll_offset = glm::max(glm::vec2(0), glm::min(_smooth_scroll_offset, _insert_position - _size + _padding.y + _padding.w));
                _scroll_offset = _smooth_scroll_offset;
                if (glfwGetMouseButton(*_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                    _resize_state = -1;
            }
            else if (_move_state != -1)
            {
                _ui->set_cursor(CUR_NORMAL);
                const auto sz = bmax - bmin;
                bmin -= (_action_cursor_position - glm::vec2{ cx, cy });
                bmax = bmin + sz;
                _action_cursor_position ={ cx, cy };
                if (glfwGetMouseButton(*_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                    _move_state = -1;
            }
            else if (!_block_transform && glm::clamp(glm::vec2(cx, cy), bmin + glm::vec2(0, handle_width*0.5f), bmax-glm::vec2(handle_width*0.5f, 0)) != glm::vec2(cx, cy) &&
                glm::clamp(glm::vec2(cx, cy), bmin - glm::vec2(0, handle_width*0.5f), bmax + glm::vec2(handle_width*0.5f, 0)) == glm::vec2(cx, cy))
            {
                if (cx > bmax.x - 4.f)
                {
                    _ui->set_cursor(CUR_HRESIZE);
                    if (glfwGetMouseButton(*_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                    {
                        _action_cursor_position.x = cx;
                        _resize_state = 0;
                    }
                }
                else if (cy < bmin.y + 4.f)
                {
                    _ui->set_cursor(CUR_VRESIZE);
                    if (glfwGetMouseButton(*_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                    {
                        _action_cursor_position.y = cy;
                        _resize_state = 1;
                    }
                }
            }
            else if (!_block_transform && glm::clamp(glm::vec2(cx, cy), glm::vec2(bmin.x, bmax.y - title_bar_height), bmax) == glm::vec2(cx, cy))
            {
                _ui->set_cursor(CUR_NORMAL);
                if (glfwGetMouseButton(*_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    _action_cursor_position ={ cx, cy };
                    _move_state = 1;
                }
            }

            if (_resize_state == -1 && _move_state==-1 && glfwGetMouseButton(*_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
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
                return;
            }

            _pos = bmin;
            _size = bmax - bmin;

            _insert_position = { 0,0 };
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
            if(!_minimized)
                _ui->draw_text(_title, title_font, 0, { bmin.x + 8 + 24 + 8, bmax.y - 0.5f*(title_bar_height + title_font.size()) }, { bmax.x, bmax.y - 0.5f*(title_bar_height - title_font.size()) }, ui::ALIGN_LEFT, {255, 255, 255, uint8_t(_alpha * 255)}, 1);

            //title bar actions
            if (_close_button)
            { 
                const auto state = _close_button->draw_inside(*_window, _ui, { bmax.x - 8 - 24, bmax.y - title_bar_height * 0.5f - 12 }, { bmax.x - 8, bmax.y - title_bar_height * 0.5f + 12 });
                if (state == STATE_RELEASE)
                {
                    log_i << "Closed.";
                    if (_minimized)
                    {
                        lo.notify_unminimized(this);
                    }
                    animate().to_size({ STACK_WIDTH, title_bar_height }).to_pos(_pos + _size - glm::vec2(STACK_WIDTH, title_bar_height)).with_end([this]() {
                        _open = false;
                    });
                    _close_button->set_state(STATE_UP);
                }
            }
            if (_maximize_button)
            {
                const auto state = _maximize_button->draw_inside(*_window, _ui, { bmax.x - 8 - 24-8-24, bmax.y - title_bar_height * 0.5f - 12 }, { bmax.x - 8-24-8, bmax.y - title_bar_height * 0.5f + 12 });
                if (state == STATE_RELEASE)
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
                    _maximize_button->set_state(STATE_UP);
                }
            }
            if (!_minimized && _minimize_button)
            {
                const auto state = _minimize_button->draw_inside(*_window, _ui, { bmax.x - 8 - 24-8-24-8-24, bmax.y - title_bar_height * 0.5f - 12 }, { bmax.x - 8-24-8-24-8, bmax.y - title_bar_height * 0.5f + 12 }, _alpha);
                if (state == STATE_RELEASE)
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

            _smooth_scroll_offset = glm::mix(_smooth_scroll_offset, _scroll_offset, float(10.f * _window->delta_time()));
            _ui->scissor({ 0, 0 }, { wi, he});

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
            if(align & ui::ALIGN_RIGHT)
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
            const auto state = button.draw_inside(*_window, _ui, min + _smooth_scroll_offset, max + _smooth_scroll_offset);
            reset_draw_state();
            return state;
        }

    private:
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
        std::shared_ptr<io::window> _window;
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

    void ui_layouter::notify_minimized(ui_window* win)
    {
        minimized.push_back(win);
    }

    void ui_layouter::notify_unminimized(ui_window* win)
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