#pragma once
#include <memory>

namespace vk
{
template <class T, class... Params> std::shared_ptr<T> create(Params&&... args)
{
    return std::make_shared<T, Params...>(std::forward<Params>(args)...);
}
}
