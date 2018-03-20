#pragma once

namespace gl
{

    template<typename T>
    buffer<T>::buffer(const buffer& other) noexcept
        : buffer(other._usage)
    {
        operator=(std::forward<const buffer&>(other));
    }

    template<typename T>
    buffer<T>::buffer(buffer&& other) noexcept
    {
        operator=(std::forward<buffer&&>(other));
    }

    template<typename T>
    buffer<T>& buffer<T>::operator=(const buffer& other) noexcept
    {
        _usage = other._usage;
        _size = other._size;
        _reserved_size = _size;
        _data_size = other._data_size;
        _data_offset = other._data_offset;
        _data_full_size = other._data_full_size;
        _data_access = other._data_access;
        _usage = other._usage;

        if (other._id == gl_buffer_t::zero)
        {
            glDeleteBuffers(1, &_id);
            _id = gl_buffer_t::zero;
            return *this;
        }

        const size_t last_size = _size;
        const size_t last_data_size = _data_size;
        const size_t last_data_offset = _data_offset;
        const bool was_data_full_size = _data_full_size;
        const GLbitfield map_access = _data_access;
        if (was_data_full_size && _size != 0)
            unmap();

        const bool was_mapped = is_mapped();

        if (_id != gl_buffer_t::zero)
        {
            if (was_mapped)
                unmap();
            glDeleteBuffers(1, &_id);
        }
        glCreateBuffers(1, _id);
        if ((_usage & GL_DYNAMIC_STORAGE_BIT) == GL_DYNAMIC_STORAGE_BIT)
        {
            glNamedBufferStorage(_id, _reserved_size * sizeof(T), nullptr, _usage);
            glCopyNamedBufferSubData(other, _id, 0, 0, other.size() * sizeof(T));
        }
        else
        {
            std::vector<T> temp(_size);
            glGetNamedBufferSubData(other, 0, _size * sizeof(T), temp.data());
            glNamedBufferStorage(_id, _reserved_size * sizeof(T), nullptr, _usage);
            glNamedBufferSubData(other, _id, 0, 0, other.size() * sizeof(T), temp.data());
        }
        init_handle();

        if (was_data_full_size)
            map(map_access);
        else if (was_mapped)
            map(_data_offset, _data_size, map_access);
        return *this;
    }

    template<typename T>
    buffer<T>& buffer<T>::operator=(buffer&& other) noexcept
    {
        _id = other._id;
        _data = other._data;
        _cached = std::move(other._cached);
        _cached_index = other._cached_index;
        _usage = other._usage;
        _size = other._size;
        _reserved_size = other._reserved_size;
        _data_size = other._data_size;
        _data_offset = other._data_offset;
        _data_full_size = other._data_full_size;
        _data_access = other._data_access;
        _handle = other._handle;

        other._id = gl_buffer_t::zero;
        other._data = nullptr;
        for (auto&& c : _cached) {
            c.first = -1; c.second = nullptr;
        }
        other._cached_index = 0;
        other._usage = GL_ZERO;
        other._size = 0;
        other._reserved_size = 0;
        other._data_size = 0;
        other._data_offset = 0;
        other._data_full_size = false;
        other._data_access = GL_ZERO;
        other._handle = 0ui64;

        return *this;
    }

    template<typename T>
    buffer<T>::buffer(GLbitfield usage) noexcept
        : _usage(usage)
    {
        glCreateBuffers(1, &_id);
    }

    template<typename T>
    buffer<T>::~buffer() noexcept
    {
        if(_id != gl_buffer_t::zero)
            glDeleteBuffers(1, &_id);
    }

    template<typename T>
    buffer<T>::buffer(size_t count, GLbitfield usage)
        : buffer(count, T(), usage)
    {
    }

    template<typename T>
    buffer<T>::buffer(size_t count, const T& init, GLbitfield usage)
        : buffer(usage)
    {
        resize(count, init, usage);
    }

    template<typename T>
    buffer<T>::buffer(std::initializer_list<T> list, GLbitfield usage)
        : buffer(list.begin(), list.end(), usage)
    {
    }

    template<typename T>
    template<typename It, typename>
    buffer<T>::buffer(It begin, It end, GLbitfield usage)
        : buffer(usage)
    {
        _size = std::distance(begin, end);
        if (_size > 0)
        {
            if constexpr(iterator_has_difference_v<It>)
            {
                const T* data = &*begin;
                if (_size > 1 && (std::distance(&*begin, &*std::prev(end)) != _size-1))
                {
                    std::vector<T> temp(begin, end);
                    glNamedBufferStorage(_id, _size * sizeof(T), temp.data(), _usage);
                }
                else
                {
                    glNamedBufferStorage(_id, _size * sizeof(T), data, _usage);
                }
            }
            else
            {
                std::vector<T> temp(begin, end);
                glNamedBufferStorage(_id, _size * sizeof(T), temp.data(), _usage);
            }
        }
        _reserved_size = _size;
        init_handle();
    }

    template<typename T>
    void buffer<T>::push_back(T && value)
    {
        const size_t last_size = _size;
        const bool was_data_full_size = _data_full_size;
        const GLbitfield map_access = _data_access;
        if (was_data_full_size && _size != 0)
            unmap();
        if (_size + 1 >= _reserved_size)
        {
            reserve(static_cast<size_t>(std::max(1.0, static_cast<double>(_reserved_size)) * compute_size_increase()));
        }
        glNamedBufferSubData(_id, last_size * sizeof(T), sizeof(T), &value);
        ++_size;
        if (was_data_full_size)
            map(map_access);
        glFinish();
    }

    template<typename T>
    void buffer<T>::push_back(const T& value)
    {
        const size_t last_size = _size;
        const bool was_data_full_size = _data_full_size;
        const GLbitfield map_access = _data_access;
        if (was_data_full_size && _size != 0)
            unmap();
        if (_size + 1 >= _reserved_size)
        {
            reserve(static_cast<size_t>(std::max(1.0, static_cast<double>(_reserved_size)) * compute_size_increase()));
        }
        glNamedBufferSubData(_id, last_size * sizeof(T), sizeof(T), &value);
        ++_size;
        if(was_data_full_size)
            map(map_access);
        glFinish();
    }

    template<typename T>
    template<typename ...As>
    void buffer<T>::emplace_back(As && ...value)
    {
        push_back(std::move(T(std::forward<As&&>(value)...)));
    }

    template<typename T>
    template<typename It, typename>
    inline void buffer<T>::assign(iterator start, It begin, It end)
    {
        const bool was_data_full_size = _data_full_size;
        const GLbitfield map_access = _data_access;
        if (was_data_full_size && _size != 0)
            unmap();
        if constexpr(iterator_has_difference_v<It>)
        {
            const ptrdiff_t size = end - begin;
            if (size > ptrdiff_t(_size) - start._offset)
                throw std::out_of_range("Range out of bounds.");
            const T* data = &*begin;

            if (size > 0)
            {
                glNamedBufferSubData(_id, start._offset * sizeof(T), size * sizeof(T), data);
            }
        }
        else
        {
            std::vector<T> temp(begin, end);
            const ptrdiff_t size = temp.size();
            if (size > ptrdiff_t(_size) - start._offset)
                throw std::out_of_range("Range out of bounds.");

            if (size > 0)
            {
                glNamedBufferSubData(_id, start._offset * sizeof(T), size * sizeof(T), temp.data());
            }
        }
        if (was_data_full_size)
            map(map_access);
    }

    template<typename T>
    template<typename It, typename>
    inline typename buffer<T>::iterator buffer<T>::insert(iterator at, It begin, It end)
    {
        iterator retval;
        const bool was_data_full_size = _data_full_size;
        const GLbitfield map_access = _data_access;
        auto end_it = this->end();
        if (was_data_full_size && _size != 0)
            unmap();

        const auto dist = std::distance(begin, end);

        if (dist > 0)
        {
            if constexpr(iterator_has_difference_v<It>)
            {
                const size_t new_size = _size + dist;
                size_t reserved = _reserved_size;
                if (new_size > reserved)
                {
                    while (new_size > reserved)
                        reserved = static_cast<size_t>(std::max(1.0, static_cast<double>(reserved)) * compute_size_increase());
                    reserve(reserved);
                }
                glFinish();

                _size = new_size;
                const size_t insert_position = at._offset;
                end_it._size += dist;
                at._size += dist;
                if (end_it > at + dist)
                    retval = std::rotate(at, at + dist, end_it);
                else
                    retval = at;

                if (_size > 1 && (std::distance(&*begin, &*std::prev(end)) != _size - 1))
                {
                    std::vector<T> temp(begin, end);
                    glNamedBufferSubData(_id, insert_position * sizeof(T), dist * sizeof(T), temp.data());
                }
                else
                {
                    const T* data = &*begin;
                    glNamedBufferSubData(_id, insert_position * sizeof(T), dist * sizeof(T), data);
                }
            }
            else
            {
                const size_t new_size = _size + dist;
                size_t reserved = _reserved_size;
                if (new_size > reserved)
                {
                    while (new_size > reserved)
                        reserved = static_cast<size_t>(std::max(1.0, static_cast<double>(reserved)) * compute_size_increase());
                    reserve(reserved);
                }
                _size = new_size;
                const size_t insert_position = at._offset;
                end_it._size += dist;
                at._size += dist;
                if (end_it > at + dist)
                    retval = std::rotate(at, at + dist, end_it);
                else
                    retval = at;
                std::vector<T> temp(begin, end);
                glNamedBufferSubData(_id, insert_position * sizeof(T), dist * sizeof(T), temp.data());
            }
        }
      
        if (was_data_full_size)
            map(map_access);

        return retval;
    }

    template<typename T>
    inline typename buffer<T>::iterator buffer<T>::erase(iterator it)
    {
        const ptrdiff_t diff = it - begin();
        --_size;
        if (diff < ptrdiff_t(_data_offset))
            _data_offset--;
        else if (diff < ptrdiff_t(_data_offset + _data_size))
            --_data_size;
        iterator i; i._parent = this; i._size = _size; i._offset = diff;
        return i;
    }

    template<typename T>
    double buffer<T>::compute_size_increase() const noexcept
    {
        return 1.0 + (10.0 / std::max(1.0, std::pow(_size, 0.35)));
    }

    template<typename T>
    void buffer<T>::shrink_to_fit()
    {
        const bool was_data_full_size = _data_full_size;
        const GLbitfield map_access = _data_access;
        if (was_data_full_size && _size != 0)
            unmap();
        resize(_size, _usage);
        if (was_data_full_size)
            map(map_access);
    }

    template<typename T>
    inline void buffer<T>::bind(GLenum target, uint32_t binding) const noexcept
    {
        bind(target, binding, 0, _size);
    }

    template<typename T>
    inline void buffer<T>::bind(GLenum target, uint32_t binding, ptrdiff_t offset, size_t size) const noexcept
    {
        glBindBufferRange(target, binding, _id, offset * sizeof(T), size * sizeof(T));
    }

    template<typename T>
    inline bool buffer<T>::empty() const noexcept
    {
        return _size == 0;
    }

    template<typename T>
    inline size_t buffer<T>::size() const noexcept
    {
        return _size;
    }

    template<typename T>
    inline size_t buffer<T>::capacity() const noexcept
    {
        return _reserved_size;
    }

    template<typename T>
    void buffer<T>::clear() noexcept
    {
        std::vector<uint8_t> temp(_size * sizeof(T), 0);
        glNamedBufferSubData(_id, 0, _size * sizeof(T), temp.data());
        _size = 0;
    }

    template<typename T>
    void buffer<T>::clear(const T& value) noexcept
    {
        std::vector<T> temp(_size, value);
        glNamedBufferSubData(_id, 0, _size * sizeof(T), temp.data());
        _size = 0;
    }

    template<typename T>
    void buffer<T>::resize(size_t size, GLbitfield usage)
    {
        resize(size, T(), usage);
    }

    template<typename T>
    void buffer<T>::resize(size_t size, const T & init, GLbitfield usage)
    {
        if (usage != keep_usage) _usage = usage;
        reserve(size);
        if (_size < size)
        {
            const ptrdiff_t diff = size - _size;
            std::vector<T> additional_data(diff, init);
            glNamedBufferSubData(_id, _size * sizeof(T), diff * sizeof(T), additional_data.data());
        }
        _size = size;
    }

    template<typename T>
    void buffer<T>::reserve(size_t size)
    {
        if ((_usage & GL_DYNAMIC_STORAGE_BIT) != GL_DYNAMIC_STORAGE_BIT)
            throw std::invalid_argument("Cannot reserve or resize a buffer without GL_DYNAMIC_STORAGE_BIT set.");
        if (size == _reserved_size)
            return;
        const bool was_mapped = is_mapped();
        size_t map_size = _data_size;
        const ptrdiff_t map_offset = _data_offset;
        const GLbitfield map_access = _data_access;
        bool data_full_size = _data_full_size;
        if (was_mapped && _data_size != 0)
            unmap();

        std::vector<T> data;
        data.reserve(_size);
        glGetNamedBufferSubData(_id, 0, _size * sizeof(T), data.data());
        glFinish();
        glDeleteBuffers(1, &_id);
        glCreateBuffers(1, &_id);
        glNamedBufferStorage(_id, size * sizeof(T), nullptr, _usage);
        glNamedBufferSubData(_id, 0, std::min(_size, size) * sizeof(T), data.data());
        glFinish();
        if (was_mapped)
        {
            const ptrdiff_t max_size = static_cast<ptrdiff_t>(size) - map_offset;
            if (_size > size)
            {
                map_size = static_cast<size_t>(std::max(std::min(static_cast<ptrdiff_t>(map_size), max_size), 0i64));
            }
            if (!data_full_size && map_size > 0)
                map(map_offset, map_size, map_access);
        }
        _reserved_size = size;
        _size = std::min(_size, size);
        init_handle();
    }

    template<typename T>
    void buffer<T>::init_handle() noexcept
    {
        if (!glIsNamedBufferResidentNV(_id))
            glMakeNamedBufferResidentNV(_id, GL_READ_WRITE);
        glGetNamedBufferParameterui64vNV(_id, GL_BUFFER_GPU_ADDRESS_NV, &_handle);
    }

    template<typename T>
    void buffer<T>::map(GLbitfield access)
    {
        _data_full_size = true;
        map(_size, access);
    }

    template<typename T>
    void buffer<T>::map(size_t count, GLbitfield access)
    {
        map(0, count, access);
    }

    template<typename T>
    bool buffer<T>::is_mapped() const noexcept
    {
        return (_data_full_size && _size != 0) || _data != nullptr;
    }

    template<typename T>
    void buffer<T>::map(size_t begin, size_t count, GLbitfield access)
    {
        const bool full = _data_full_size;
        if (!(_data_full_size && _size == 0))
        {
            if (begin + count > _size)
                throw std::out_of_range("Range out of range.");
            if (!is_mapped()) unmap();
        }
        _data_full_size = full;
        _data_size = count;
        _data_offset = begin;
        _data_access = access;
        for (auto&& c : _cached) {
            c.first = -1; c.second = nullptr;
        }
        if (count != 0)
            _data = static_cast<T*>(glMapNamedBufferRange(_id, begin * sizeof(T), count * sizeof(T), access));
        else
            _data = nullptr;
    }

    template<typename T>
    void buffer<T>::flush_element(size_t index)
    {
        flush_elements(index, 1);
    }

    template<typename T>
    void buffer<T>::flush_elements(size_t begin, size_t count)
    {
        flush_bytes(begin * sizeof(T), count * sizeof(T));
    }

    template<typename T>
    void buffer<T>::flush_bytes(size_t offset, size_t size)
    {
        if (offset + size > _data_size * sizeof(T)) throw std::out_of_range("Range out of bounds.");
        synchronize();
        glFlushMappedNamedBufferRange(_id, offset, size);
    }

    template<typename T>
    void buffer<T>::flush()
    {
        flush_elements(_data_offset, _data_size);
    }

    template<typename T>
    void buffer<T>::unmap() noexcept
    {
        synchronize();
        _data_full_size = false;
        _data = nullptr;
        _data_size = 0;
        _data_offset = 0;
        _data_access = GL_ZERO;
        glUnmapNamedBuffer(_id);
    }

    template<typename T>
    void buffer<T>::synchronize() const noexcept
    {
        for (int i = 0; i < _cached.size(); ++i)
        {
            if ((_usage & GL_DYNAMIC_STORAGE_BIT) == GL_DYNAMIC_STORAGE_BIT && _cached[i].second)
                glNamedBufferSubData(_id, _cached[i].first * sizeof(T), sizeof(T), _cached[i].second.get());
        }
    }

    template<typename T>
    inline uint64_t buffer<T>::handle() const noexcept
    {
        return _handle;
    }

    template<typename T>
    T & buffer<T>::at(size_t index)
    {
        if ((_data && index >= _data_offset + _data_size) || index >= _size)
            throw std::out_of_range("Index out of bounds.");
        if (!_data)
        {
            int idx = -1;
            for (int i = 0; i < _cached.size(); ++i)
            {
                if (_cached[i].first == index)
                {
                    idx = i;
                    break;
                }
            }

            if (idx == -1)
            {
                if ((_usage & GL_DYNAMIC_STORAGE_BIT) == GL_DYNAMIC_STORAGE_BIT && _cached[_cached_index].second)
                    glNamedBufferSubData(_id, _cached[_cached_index].first * sizeof(T), sizeof(T), _cached[_cached_index].second.get());
                if (!_cached[_cached_index].second)
                    _cached[_cached_index].second = std::unique_ptr<T>(static_cast<T*>(malloc(sizeof(T))));
                glGetNamedBufferSubData(_id, index * sizeof(T), sizeof(T), _cached[_cached_index].second.get());
            }
            else
            {
                _cached_index = idx;
            }

            _cached[_cached_index].first = index;
            int last_idx = _cached_index;
            _cached_index = (_cached_index + 1) % _cached.size();
            return *_cached[last_idx].second;
        }
        return _data[index];
    }

    template<typename T>
    const T & buffer<T>::at(size_t index) const
    {
        return const_cast<buffer*>(this)->at(index);
    }

    template<typename T>
    T & buffer<T>::operator[](size_t index)
    {
        return at(index);
    }

    template<typename T>
    const T & buffer<T>::operator[](size_t index) const
    {
        return at(index);
    }

    template<typename T>
    T* buffer<T>::data() noexcept
    {
        return _data;
    }

    template<typename T>
    const T* buffer<T>::data() const noexcept
    {
        return _data;
    }

    template<typename T>
    buffer<T>::operator gl_buffer_t() const noexcept
    {
        return _id;
    }

    template<typename T>
    typename buffer<T>::bounded_buffer_data buffer<T>::iterate() noexcept
    {
        if (_data)
            return bounded_buffer_data(this, _data_size);
        else
            return bounded_buffer_data(this, _size);
    }

    template<typename T>
    typename buffer<T>::iterator buffer<T>::begin() noexcept { return iterate().begin(); }

    template<typename T>
    typename buffer<T>::iterator buffer<T>::end() noexcept { return iterate().end(); }

    template<typename T>
    typename buffer<T>::const_iterator buffer<T>::begin() const noexcept { return iterate().cbegin(); }

    template<typename T>
    typename buffer<T>::const_iterator buffer<T>::end() const noexcept { return iterate().cend(); }

    template<typename T>
    typename buffer<T>::const_iterator buffer<T>::cbegin() const noexcept { return iterate().cbegin(); }

    template<typename T>
    typename buffer<T>::const_iterator buffer<T>::cend() const noexcept { return iterate().cend(); }

    template<typename T>
    typename buffer<T>::reverse_iterator buffer<T>::rbegin() noexcept { return iterate().rbegin(); }

    template<typename T>
    typename buffer<T>::reverse_iterator buffer<T>::rend() noexcept { return iterate().rend(); }

    template<typename T>
    typename buffer<T>::const_reverse_iterator buffer<T>::rbegin() const noexcept { return iterate().crbegin(); }

    template<typename T>
    typename buffer<T>::const_reverse_iterator buffer<T>::rend() const noexcept { return iterate().crend(); }

    template<typename T>
    typename buffer<T>::const_reverse_iterator buffer<T>::crbegin() const noexcept { return iterate().crbegin(); }

    template<typename T>
    typename buffer<T>::const_reverse_iterator buffer<T>::crend() const noexcept { return iterate().crend(); }
}