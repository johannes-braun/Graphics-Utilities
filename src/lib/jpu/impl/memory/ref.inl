#pragma once

namespace jpu
{
    inline int ref_count::inc_ref()
    {
        return _references.fetch_add(1) + 1;
    }

    inline int ref_count::dec_ref()
    {
        const auto last = _references.fetch_sub(1);
        if (last <= 1)
            delete this;

        return last - 1;
    }

    inline int ref_count::references() const
    {
        return _references.load();
    }

    inline void ref_count::eliminate_references()
    {
        while (references() > 1)
            dec_ref();
    }

    template <typename T>
    ref_ptr<T>::ref_ptr(nullptr_t)
        : _object(nullptr)
    {}

    template <typename T>
    ref_ptr<T>::ref_ptr()
        : ref_ptr(nullptr)
    {}

    template <typename T>
    ref_ptr<T>::ref_ptr(const ref_ptr& other)
        : _object(other.get())
    {
        if (_object) _object->inc_ref();
    }

    template <typename T>
    ref_ptr<T>::ref_ptr(ref_ptr&& other) noexcept
        : _object(other)
    {
        other._object = nullptr;
    }

    template <typename T>
    ref_ptr<T>& ref_ptr<T>::operator=(const ref_ptr& other)
    {
        if (_object)
            _object->dec_ref();
        _object = other._object;
        if (_object) _object->inc_ref();
        return *this;
    }

    template <typename T>
    ref_ptr<T>& ref_ptr<T>::operator=(ref_ptr&& other) noexcept
    {
        if (_object)
            _object->dec_ref();
        _object = other._object;
        other._object = nullptr;
        return *this;
    }

    template <typename T>
    ref_ptr<T>::~ref_ptr()
    {
        if (_object)
            _object->dec_ref();
    }

    template <typename T>
    ref_ptr<T>::ref_ptr(T* value)
    {
        _object = value;
    }

    template <typename T>
    ref_ptr<T>& ref_ptr<T>::operator=(T* other) noexcept
    {
        reset(other);
        return *this;
    }

    template <typename T>
    ref_ptr<T>::operator T*() const
    {
        return _object;
    }

    template <typename T>
    T* ref_ptr<T>::get() const
    {
        return _object;
    }

    template <typename T>
    ref_ptr<T>::operator bool() const
    {
        return _object == nullptr;
    }

    template <typename T>
    void ref_ptr<T>::reset()
    {
        if (_object)
        {
            _object->dec_ref();
            _object = nullptr;
        }
    }

    template <typename T>
    void ref_ptr<T>::reset(nullptr_t)
    {
        reset();
    }

    template <typename T>
    void ref_ptr<T>::reset(T* other)
    {
        if (_object)
            _object->dec_ref();
        _object = other;
        _object->inc_ref();
    }

    template<typename T, typename... Args>
    ref_ptr<T> make_ref(Args&&... args)
    {
        return new T(std::forward<Args&&>(args)...);
    }


    template <typename T>
    remove_add_refcount_t<T>& ref_ptr<T>::operator*() const
    {
        return *_object;
    }

    template <typename T>
    T* ref_ptr<T>::operator->() const
    {
        return _object;
    }
}