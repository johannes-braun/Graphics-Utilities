#pragma once

namespace jpu
{
    template <typename TIdentifier, typename TPayload>
    TPayload& named_vector<TIdentifier, TPayload>::operator[](const TIdentifier& id)
    {
        if(const auto idx = index_of(id); idx == -1)
            return push(id, TPayload());
        else return _payloads[idx];
    }

    template <typename TIdentifier, typename TPayload>
    const TPayload& named_vector<TIdentifier, TPayload>::operator[](const TIdentifier& id) const
    {
        return get_by_id(id);
    }

    template <typename TIdentifier, typename TPayload>
    void named_vector<TIdentifier, TPayload>::reserve(const size_t count)
    {
        _keys.reserve(count);
        _payloads.reserve(count);
    }

    template <typename TIdentifier, typename TPayload>
    size_t named_vector<TIdentifier, TPayload>::size() const
    {
        return _payloads.size();
    }

    template <typename TIdentifier, typename TPayload>
    TPayload* named_vector<TIdentifier, TPayload>::data()
    {
        return _payloads.data();
    }
    
    template <typename TIdentifier, typename TPayload>
    const TPayload* named_vector<TIdentifier, TPayload>::data() const
    {
        return _payloads.data();
    }

    template <typename TIdentifier, typename TPayload>
    TPayload& named_vector<TIdentifier, TPayload>::get_by_id(const TIdentifier& id)
    {
        return _payloads[index_of(id)];
    }

    template <typename TIdentifier, typename TPayload>
    const TPayload& named_vector<TIdentifier, TPayload>::get_by_id(const TIdentifier& id) const
    {
        return _payloads[index_of(id)];
    }

    template <typename TIdentifier, typename TPayload>
    const TPayload& named_vector<TIdentifier, TPayload>::get_by_index(const int64_t index) const
    {
        return _payloads[index];
    }

    template <typename TIdentifier, typename TPayload>
    const TIdentifier& named_vector<TIdentifier, TPayload>::id_by_index(const int64_t index) const
    {
        return _keys[index];
    }

    template <typename TIdentifier, typename TPayload>
    TPayload& named_vector<TIdentifier, TPayload>::get_by_index(const int64_t index)
    {
        return _payloads[index];
    }

    template <typename TIdentifier, typename TPayload>
    int64_t named_vector<TIdentifier, TPayload>::index_of(const TIdentifier& id) const
    {
        if (const auto it = std::find(_keys.begin(), _keys.end(), id); it != _keys.end())
            return it - _keys.begin();
        return -1;
    }

    template <typename TIdentifier, typename TPayload>
    bool named_vector<TIdentifier, TPayload>::exists(const TIdentifier& id) const
    {
        const auto it = std::find(_keys.begin(), _keys.end(), id);
        return it != _keys.end();
    }

    template <typename TIdentifier, typename TPayload>
    TPayload& named_vector<TIdentifier, TPayload>::push(const TIdentifier& id, const TPayload& payload)
    {
        _keys.push_back(id);
        _payloads.push_back(payload);
        return _payloads.back();
    }

    template <typename TIdentifier, typename TPayload>
    void named_vector<TIdentifier, TPayload>::erase(const TIdentifier& id)
    {
        const auto index = index_of(id);

        // Swap to end
        std::iter_swap(_keys.begin() + index, std::prev(_keys.end()));
        std::iter_swap(_payloads.begin() + index, std::prev(_payloads.end()));

        // Then remove
        _keys.pop_back();
        _payloads.pop_back();
    }

    template <typename TIdentifier, typename TPayload>
    void named_vector<TIdentifier, TPayload>::erase_exact(const TIdentifier& id, const TPayload& payload)
    {
        auto current = _keys.begin();

        while (current != _keys.end() && _payloads[current - _keys.begin()] != payload)
        {
            current = std::find(current, _keys.end(), id);
        }

        if (current == _keys.end())
            return;

        const auto index = current - _keys.begin();

        // Swap to end
        std::iter_swap(_keys.begin() + index, std::prev(_keys.end()));
        std::iter_swap(_payloads.begin() + index, std::prev(_payloads.end()));

        // Then remove
        _keys.pop_back();
        _payloads.pop_back();
    }

    template <typename TIdentifier, typename TPayload>
    void named_vector<TIdentifier, TPayload>::clear()
    {
        _payloads.clear();
        _keys.clear();
    }

    template <typename TIdentifier, typename TPayload>
    size_t named_vector<TIdentifier, TPayload>::capacity() const
    {
        return _payloads.capacity();
    }

    template <typename TIdentifier, typename TPayload>
    const std::vector<TIdentifier>& named_vector<TIdentifier, TPayload>::keys() const
    {
        return _keys;
    }

    template <typename TIdentifier, typename TPayload>
    const std::vector<TPayload>& named_vector<TIdentifier, TPayload>::payloads() const
    {
        return _payloads;
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::iterator named_vector<TIdentifier, TPayload>::begin()
    {
        return _payloads.begin();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::iterator named_vector<TIdentifier, TPayload>::end()
    {
        return _payloads.end();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_iterator named_vector<TIdentifier, TPayload>::begin() const
    {
        return _payloads.begin();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_iterator named_vector<TIdentifier, TPayload>::end() const
    {
        return _payloads.end();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_iterator named_vector<TIdentifier, TPayload>::cbegin() const
    {
        return _payloads.cbegin();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_iterator named_vector<TIdentifier, TPayload>::cend() const
    {
        return _payloads.cend();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::reverse_iterator named_vector<TIdentifier, TPayload>::rbegin()
    {
        return _payloads.rbegin();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::reverse_iterator named_vector<TIdentifier, TPayload>::rend()
    {
        return _payloads.rend();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_reverse_iterator named_vector<TIdentifier, TPayload>::rbegin() const
    {
        return _payloads.rbegin();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_reverse_iterator named_vector<TIdentifier, TPayload>::rend() const
    {
        return _payloads.rend();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_reverse_iterator named_vector<TIdentifier, TPayload>::crbegin() const
    {
        return _payloads.crbegin();
    }

    template <typename TIdentifier, typename TPayload>
    typename std::vector<TPayload>::const_reverse_iterator named_vector<TIdentifier, TPayload>::crend() const
    {
        return _payloads.crend();
    }
}
