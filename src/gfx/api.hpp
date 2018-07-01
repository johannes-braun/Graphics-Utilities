#pragma once

#include <type_traits>

//#define GFX_EXPOSE_APIS
#if defined(GFX_EXPOSE_APIS)
#include <mygl/mygl.hpp>
#endif

namespace gfx
{
enum class gapi
{
    opengl = 0,
    vulkan
};

template <gapi Api, typename T> struct api_cast_t
{
    using type = void;
    type operator()(const T& object) const { throw; }
};

template <gapi Api, typename T> typename api_cast_t<Api, T>::type api_cast(const T& object) { return api_cast_t<Api, T>()(object); }
}

#if defined(GFX_EXPOSE_APIS)
#define GFX_api_cast_template_type(GAPI, T, RET)                                                                                                    \
    template <typename K> class T;                                                                                                                               \
    template <typename K> struct api_cast_t<GAPI, T<K>>                                                                                           \
    \
{                                                                                                                                     \
        using type = RET;                                                                                                                  \
        type operator()(const T<K>& object) const;                                                                                            \
    \
};
#define GFX_api_cast_type(GAPI, T, RET)                                                                                                    \
    class T;                                                                                                                               \
    template <> struct api_cast_t<GAPI, T>                                                                                           \
    \
{                                                                                                                                     \
        using type = RET;                                                                                                                  \
        type operator()(const T& object) const;                                                                                            \
    \
};
#define GFX_api_cast_template_impl(GAPI, T)                                                                                                         \
    \
template<typename K> typename api_cast_t<GAPI, T<K>>::type                                                                                                  \
            api_cast_t<GAPI, T<K>>::operator()(const T<K>& object) const                                                                         \
    \
{                                                                                                                                     \
        return std::any_cast<typename api_cast_t<GAPI, T<K>>::type>(object._implementation->api_handle());                                    \
    \
}
#define GFX_api_cast_impl(GAPI, T)                                                                                                         \
    \
inline typename api_cast_t<GAPI, T>::type                                                                                                  \
            api_cast_t<GAPI, T>::operator()(const T& object) const                                                                         \
    \
{                                                                                                                                     \
        return std::any_cast<typename api_cast_t<GAPI, T>::type>(object._implementation->api_handle());                                    \
    \
}

#define GFX_api_cast_op(GAPI, T)                                                                                                           \
    friend struct api_cast_t<GAPI, T>;                                                                                                     \
    operator typename api_cast_t<GAPI, T>::type() const { return api_cast<GAPI>(*this); }
#else
#define GFX_api_cast_template_type(GAPI, T, RET)
#define GFX_api_cast_type(GAPI, T, RET)
#define GFX_api_cast_template_impl(GAPI, T)
#define GFX_api_cast_impl(GAPI, T)
#define GFX_api_cast_op(GAPI, T)
#endif