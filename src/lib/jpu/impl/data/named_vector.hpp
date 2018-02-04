#pragma once

#include <vector>
#include <algorithm>

namespace jpu
{
    template<typename TIdentifier, typename TPayload>
    class named_vector
    {
    public:
        using value_type = TPayload;
        using index_type = TIdentifier;

        TPayload& operator[](const TIdentifier& id);
        const TPayload& operator[](const TIdentifier& id) const;

        void reserve(const size_t count);
        size_t size() const;
        TPayload* data();
        const TPayload* data() const;

        TPayload& get_by_id(const TIdentifier& id);
        const TPayload& get_by_id(const TIdentifier& id) const;

        const TPayload& get_by_index(const int64_t index) const;
        TPayload& get_by_index(const int64_t index);

        int64_t index_of(const TIdentifier& id) const;

        bool exists(const TIdentifier& id) const;

        TPayload& push(const TIdentifier& id, const TPayload& payload);
        template<typename... Args> TPayload& emplace(const TIdentifier& id, Args&&... args) { return push(std::forward<const TIdentifier&>(id), TPayload(std::forward<Args&&>(args)...)); }
        void erase(const TIdentifier& id);
        void erase_exact(const TIdentifier& id, const TPayload& payload);

        size_t capacity() const;
        const std::vector<TIdentifier>& keys() const;
        const std::vector<TPayload>& payloads() const;

        typename std::vector<TPayload>::iterator begin();
        typename std::vector<TPayload>::iterator end();
        typename std::vector<TPayload>::const_iterator begin() const;
        typename std::vector<TPayload>::const_iterator end() const;
        typename std::vector<TPayload>::const_iterator cbegin() const;
        typename std::vector<TPayload>::const_iterator cend() const;
        typename std::vector<TPayload>::reverse_iterator rbegin();
        typename std::vector<TPayload>::reverse_iterator rend();
        typename std::vector<TPayload>::const_reverse_iterator rbegin() const;
        typename std::vector<TPayload>::const_reverse_iterator rend() const;
        typename std::vector<TPayload>::const_reverse_iterator crbegin() const;
        typename std::vector<TPayload>::const_reverse_iterator crend() const;

    private:
        std::vector<TIdentifier> _keys;
        std::vector<TPayload> _payloads;
    };
}

#include "named_vector.inl"