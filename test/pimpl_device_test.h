#pragma once

#include <string>
#include <string_view>

#include <pimpl_wrapper.hpp>

enum class DeviceType
{
    CPU,
    RAM,
    GPU,
    UNKNOWN
};

using namespace pimp;
using namespace pimp::policy;

class Device
{
public:
    Device();
    Device(std::string_view name, DeviceType type);

    Device(const Device& other);
    Device& operator=(const Device& other);

    Device(Device&& other);
    Device& operator=(Device&& other);

    std::string_view name() const;
    DeviceType type() const;

    ~Device();

    friend void swap(Device& lhs, Device& rhs);

private:
    class Impl;
    static constexpr size_t kMaxImplSize{ 128u };
private:
    pimpl_wrapper<Impl, static_storage<Impl, kMaxImplSize>> m_pimpl;
};
