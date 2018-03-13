#pragma once

namespace gl
{
    template<typename T, typename>
    void command_buffer::push(const T& cmd)
    {
        _command_buffer.insert(_command_buffer.end(), cmd.as_bytes(), cmd.as_bytes() + cmd.byte_size());
    }
}