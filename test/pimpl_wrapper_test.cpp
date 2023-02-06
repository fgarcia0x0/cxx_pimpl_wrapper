#include <iostream>

#include <pimpl_wrapper.hpp>
#include <pimpl_device_test.h>

struct person
{
    person(const std::string& name, uint8_t age)
        : m_name{ name }, m_age(age)
    {}
    std::string m_name;
    uint8_t m_age;
};

template <typename StoragePolicy, typename Predicate>
static void build_person(Predicate&& pred);

int main(int argc, char** argv)
{
    [argc, argv](){}();

    Device dev0("cpu#0", DeviceType::CPU);
    Device dev1(dev0);
    Device dev2("cpu#1", DeviceType::CPU);

    dev1 = dev2;
    using std::swap;
    swap(dev1, dev2);

    Device t1 = dev0;
    dev2 = std::move(t1);

    if (dev1.type() == DeviceType::CPU)
        std::cout << "[CPU]" << " Dev Name: " << dev1.name() << '\n';

    using person_policy = pimp::policy::static_storage<person, sizeof(person), alignof(person)>;
    pimp::pimpl_wrapper<person, person_policy> pimpl{ "Felipe Garcia", 21u };
    std::cout << pimpl->m_name << '\n';
    std::cout << static_cast<std::uint32_t>(pimpl->m_age) << '\n';

    using person_dyn_policy = pimp::policy::dynamic_storage<person>;
    using person_stc_policy = pimp::policy::static_storage<person, sizeof(person), alignof(person)>;
    
    auto person_print = [](const person& person) {
        std::cout << person.m_name << '\n';
        std::cout << static_cast<std::uint32_t>(person.m_age) << '\n'; 
    };

    build_person<person_dyn_policy>(person_print);
    build_person<person_stc_policy>(person_print);

    return {};
}

template <typename StoragePolicy, typename Action>
static void build_person(Action&& action)
{
    using person_policy = StoragePolicy;
    pimp::pimpl_wrapper<person, person_policy> pimpl{ "John Arthur", 44u };
    action(*pimpl);
}