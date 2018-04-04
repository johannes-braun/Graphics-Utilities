//#pragma once
//#include "ui.hpp"
//#include <io/window.hpp>
//
//namespace game
//{
//    enum ui_button_state
//    {
//        STATE_PRESS,
//        STATE_DOWN,
//        STATE_RELEASE,
//        STATE_UP
//    };
//
//    class ui_button
//    {
//    public:
//        ui_button(const std::wstring& text, const font* fnt, glm::u8vec4 tint ={255, 255, 255, 255}) : _text(text), _font(fnt), _tint(tint){}
//        ui_button(gl_texture_t texture, glm::u8vec4 tint ={ 255, 255, 255, 255 }) : _image(texture), _tint(tint) {}
//        
//        void set_state(ui_button_state state)
//        {
//            _current_state = state;
//        }
//
//        ui_button_state draw_inside(const io::window& win, ui* u, glm::vec2 bmin, glm::vec2 bmax, float alpha = 1.f)
//        {
//            static GLFWcursor* mov_cursor    = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
//
//            int wi, he; glfwGetFramebufferSize(win, &wi, &he);
//            double cx, cy; glfwGetCursorPos(win, &cx, &cy);
//            cy = he - cy;
//
//            if (!_block_click && glm::clamp(glm::vec2(cx, cy), bmin, bmax) == glm::vec2(cx, cy))
//            {
//                _scale ={ 0.9f, 0.9f };
//                u->set_cursor(CUR_HAND);
//
//                if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//                {
//                    _smooth_scale ={ 0.8f, 0.8f };
//                    _scale = _smooth_scale;
//                    if (_current_state != STATE_DOWN && _current_state != STATE_PRESS)
//                        _current_state = STATE_PRESS;
//                    else
//                        _current_state = STATE_DOWN;
//                }
//                else
//                {
//                    if (_current_state != STATE_UP && _current_state != STATE_RELEASE)
//                        _current_state = STATE_RELEASE;
//                    else
//                        _current_state = STATE_UP;
//                }
//            }
//            else
//            {
//                _scale ={ 1.f, 1.f };
//            }
//
//            if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && _current_state == STATE_UP)
//            {
//                _block_click = true;
//            }
//            else
//            {
//                _block_click = false;
//            }
//
//            _smooth_scale = mix(_smooth_scale, _scale, 30.f * win.delta_time());
//            const glm::vec2 size = abs(bmax - bmin);
//            const glm::vec2 scaled_size = size * _smooth_scale;
//
//            const glm::vec2 transf_bmin = bmin - (scaled_size - size) * 0.5f;
//            const glm::vec2 transf_bmax = bmax + (scaled_size - size) * 0.5f;
//
//            if (gl_texture_t::zero != _image)
//            {
//                u->draw_quad(transf_bmin, transf_bmax, { 0, 1 }, { 1, 0 }, _image, { _tint.r, _tint.g, _tint.b, alpha*_tint.a });
//            }
//            else if(_font)
//            {
//                u->draw_text(_text, *_font, 0, transf_bmin, transf_bmax, ui::ALIGN_CENTER, { _tint.r, _tint.g, _tint.b, alpha*_tint.a }, 1);
//            }
//
//            return _current_state;
//        }
//
//    private:
//        ui_button_state _current_state = STATE_UP;
//        bool _block_click = false;
//        const font* _font;
//        std::wstring _text;
//        glm::u8vec4 _tint;
//        gl_texture_t _image;
//        glm::vec2 _smooth_scale{ 1 };
//        glm::vec2 _scale{ 1 };
//    };
//}