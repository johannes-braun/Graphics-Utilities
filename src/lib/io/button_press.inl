#pragma once

namespace io
{
    template <int(*GetKey)(GLFWwindow*, int)>
    button<GetKey>::button(const int key)
        : _key(key)
    {}

    template <int(*GetKey)(GLFWwindow*, int)>
    void button<GetKey>::update(GLFWwindow* window)
    {
        if (GetKey(window, _key) == GLFW_PRESS)
        {
            if (_state == button_state::press || _state == button_state::down)
                _state = button_state::down;
            else
                _state = button_state::press;
        }
        else
        {
            if (_state == button_state::release || _state == button_state::up)
                _state = button_state::up;
            else
                _state = button_state::release;
        }
    }

    template <int(*GetKey)(GLFWwindow*, int)>
    button_state button<GetKey>::state() const
    {
        return _state;
    }

    template <int(* GetKey)(GLFWwindow*, int)>
    void button<GetKey>::set_key(const int key)
    {
        _key = key;
    }
}