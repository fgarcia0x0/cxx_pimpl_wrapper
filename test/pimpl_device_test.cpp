#include <iostream>
#include <type_traits>
#include <pimpl_device_test.h>

class Device::Impl
{
public:
    Impl() 
        : m_name{""}, 
          m_type{ DeviceType::UNKNOWN }
    {
    }

    Impl(std::string_view name, DeviceType type)
        : m_name{name},
          m_type{type}
    {
        std::clog << "impl constructed at args\n";
    }

    Impl(const Impl& other)
        : m_name{ other.m_name },
          m_type{ other.m_type }
    {
        std::clog << "impl copy constructor\n";
    }

    Impl& operator=(const Impl& other)
    {
        std::clog << "impl copy assignment\n";
        if (this != &other)
        {
            m_name = other.m_name;
            m_type = other.m_type;
        }

        return *this;
    }

    Impl(Impl&& other)
        : m_name{ std::move(other.m_name) },
          m_type{ std::move(other.m_type) }
    {
        std::clog << "impl move constructor\n";
    }

    Impl& operator=(Impl&& other)
    {
        std::clog << "impl move assignment\n";
        m_name = std::move(other.m_name);
        m_type = std::move(other.m_type);
        return *this;
    }

    std::string_view name() const
    {
        return m_name;
    }

    DeviceType type() const
    {
        return m_type;
    }

    ~Impl()
    {
        std::clog << "impl destructed\n";
    }

    friend void swap(Device::Impl& lhs, Device::Impl& rhs)
    {
        std::clog << "impl swapped\n";
        std::swap(lhs.m_name, rhs.m_name);
        std::swap(lhs.m_type, rhs.m_type);
    }

private:
    std::string m_name;
    DeviceType m_type;
};

Device::Device() = default;

Device::Device(std::string_view name, DeviceType type)
    : m_pimpl{ name, type }
{
}

Device::Device(Device&& other) = default;
Device& Device::operator=(Device&& other) = default;

Device::Device(const Device& other) = default;
Device& Device::operator=(const Device& other) = default;

Device::~Device() = default;

void swap(Device& lhs, Device& rhs)
{
    using std::swap;
    swap(lhs.m_pimpl, rhs.m_pimpl);
}

std::string_view Device::name() const
{
    return m_pimpl->name();
}

DeviceType Device::type() const
{
    return m_pimpl->type();
}
