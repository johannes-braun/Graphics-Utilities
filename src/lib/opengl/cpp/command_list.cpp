#include "../command_list.hpp"

namespace gl
{
    void command_buffer::start() noexcept
    {
        _command_buffer.clear();
    }

    void command_buffer::finish() noexcept
    {
        indirect = _command_buffer.handle();
        this->size = static_cast<uint32_t>(_command_buffer.size());
    }
}