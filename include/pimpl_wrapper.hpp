#pragma once

#include <memory>
#include <cstdint>
#include <iterator>
#include <new>
#include <cstddef>

namespace pimp::policy
{
    static constexpr inline auto kMaxAlignSize = alignof(std::max_align_t);

    #define PIMPL_STATIC_CHECK() \
        static_assert(sizeof(T) <= Size, "The size provided is not enough to fit T"); \
        static_assert(alignof(T) <= Alignment,                            \
                      "The given alignment must be greater than or equal to the alignment of T")

    template <typename T, size_t Size, size_t Alignment = kMaxAlignSize>
    class static_storage
    {
    public:
        using value_type = std::decay_t<T>;

        static_storage()
        {
            PIMPL_STATIC_CHECK();
            (void) std::construct_at(reinterpret_cast<value_type *>(m_storage));
        }

        template <typename... Args>
        static_storage(Args&&... args)
        {
            PIMPL_STATIC_CHECK();
            (void) std::construct_at(reinterpret_cast<value_type *>(m_storage),
                                     std::forward<Args>(args)...);
        }

        static_storage(const static_storage& other)
        {
            PIMPL_STATIC_CHECK();
            (void) std::construct_at(reinterpret_cast<value_type *>(m_storage), *other);
        }

        static_storage(static_storage&& other)
        {
            PIMPL_STATIC_CHECK();
            (void) std::construct_at(reinterpret_cast<value_type *>(m_storage), 
                                     std::move(*other));
        }

        static_storage& operator=(const static_storage& other) 
        { 
            PIMPL_STATIC_CHECK();
            if (this != &other)
                **this = *other;
            return *this; 
        }

        static_storage& operator=(static_storage&& other) 
        { 
            PIMPL_STATIC_CHECK();
            **this = std::move(*other); 
            return *this; 
        }

        value_type& operator*()
        { 
            PIMPL_STATIC_CHECK();
            return *std::launder(reinterpret_cast<value_type *>(m_storage));
        }

        const value_type& operator*() const
        { 
            PIMPL_STATIC_CHECK();
            return *std::launder(reinterpret_cast<const value_type *>(m_storage));
        }
        
        value_type* operator->()
        {
            PIMPL_STATIC_CHECK();
            return std::addressof(**this); 
        }

        const value_type* operator->() const
        {
            PIMPL_STATIC_CHECK();
            return std::addressof(**this); 
        }

        ~static_storage()
        {
            PIMPL_STATIC_CHECK();
            std::destroy_at(std::launder(reinterpret_cast<value_type *>(m_storage)));
        }

    private:
        alignas(Alignment) std::byte m_storage[Size];
    };

    template <typename T>
    class dynamic_storage
    {
    public:
        using value_type = std::decay_t<T>;

        dynamic_storage()
            : m_storage{ std::make_unique<value_type>() }
        {
        }

        template <typename... Args>
        dynamic_storage(Args&&... args)
            : m_storage{ std::make_unique<value_type>(std::forward<Args>(args)...) }
        {
        }

        dynamic_storage(const dynamic_storage& other)
            : m_storage{ std::make_unique<value_type>(*other) }
        {
        }

        dynamic_storage& operator=(const dynamic_storage& other)
        {
            if (this != &other)
                **this = *other;
            return *this;
        }

        dynamic_storage(dynamic_storage&& other)
            : m_storage{ std::move(other.m_storage) }
        {
        }

        dynamic_storage& operator=(dynamic_storage&& other)
        {
            // create new empty storage
            auto ptr = new value_type{};
            if (!ptr)
                throw std::bad_alloc{};

            m_storage.reset(std::move(ptr));
            **this = std::move(*other);
            return *this;
        }

        value_type& operator*() 
        { 
            return *m_storage;
        }

        const value_type& operator*() const
        { 
            return *m_storage;
        }
        
        value_type* operator->()
        {
            return std::addressof(**this); 
        }

        const value_type* operator->() const
        {
            return std::addressof(**this); 
        }

    private:
        std::unique_ptr<value_type> m_storage;
    };

    #undef PIMPL_STATIC_CHECK
} // namespace pimp::policy

namespace pimp
{
    template <typename T, 
              typename StoragePolicy = policy::dynamic_storage<T>>
    class pimpl_wrapper 
    {
    public:
        using value_type = std::decay_t<T>;
        using pointer = value_type*;
        using storage_policy = StoragePolicy;

        static_assert(std::is_same_v<value_type, typename storage_policy::value_type>,
                      "The policy storage value_type must be the same type of pimpl_wrapper value_type");

        pimpl_wrapper() = default;

        template <typename... Args>
        pimpl_wrapper(Args&&... args)
            : m_storage{ std::forward<Args>(args)... }
        {
        }

        pimpl_wrapper(const pimpl_wrapper& other) = default;
        pimpl_wrapper& operator=(const pimpl_wrapper& other) = default;

        pimpl_wrapper(pimpl_wrapper&& other) = default;
        pimpl_wrapper& operator=(pimpl_wrapper&& other) = default;

        ~pimpl_wrapper() = default;

        value_type& operator*() { return *m_storage; }
        const value_type& operator*() const { return *m_storage; }
        
        value_type* operator->() { return m_storage.operator->(); }
        const value_type* operator->() const { return m_storage.operator->(); }

        friend inline void swap(pimpl_wrapper& lhs, pimpl_wrapper& rhs)
        {
            using std::swap;
            swap(*lhs, *rhs);
        }

    private:
        storage_policy m_storage;
    };
} // namespace pimp
