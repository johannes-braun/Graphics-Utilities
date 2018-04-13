#pragma once
#include <atomic>

namespace jpu
{
    class ref_count
    {
    public:
        constexpr static int invalid_reference_count = -6500;

        ref_count() = default;
        ref_count(const ref_count& other) = delete;
        ref_count(ref_count&& other) noexcept {
            _references.store(other._references);
            other._references.store(invalid_reference_count);
        }
        ref_count& operator=(const ref_count& other) = delete;
        ref_count& operator=(ref_count&& other) noexcept {
            _references.store(other._references);
            other._references.store(invalid_reference_count);
        }
        virtual ~ref_count() = default;

        int inc_ref();
        int dec_ref();
        int references() const;
        void eliminate_references();

    private:
        std::atomic_int32_t _references{1};
    };

    template<typename T>
    class ref_ptr
    {
    public:
        static_assert(std::is_base_of_v<ref_count, T>, "Ref Pointer base class needs to derive from ref_count.");

        ref_ptr(std::nullptr_t);
        ref_ptr();
        ref_ptr(const ref_ptr& other);
        ref_ptr(ref_ptr&& other) noexcept;
        ref_ptr& operator=(const ref_ptr& other);
        ref_ptr& operator=(ref_ptr&& other) noexcept;
        ~ref_ptr();
        
        ref_ptr(T* value);
        ref_ptr& operator=(T* other) noexcept;
        operator T*() const;

        T& operator*() const;
        T* operator->() const;

        T* get() const;

        operator bool() const;

        void reset();
        void reset(std::nullptr_t);
        void reset(T* other);

    private:
        T* _object;
    };
    
    template<typename T, typename... Args>
    ref_ptr<T> make_ref(Args&&... args);
} // namespace jpu

#include "ref.inl"