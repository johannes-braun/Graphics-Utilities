#pragma once

namespace vkn
{
    template<typename TCreateInfo, typename TCreatedClass>
    struct ClassInfo
    {
        using info_type = TCreateInfo;
        using target_type = TCreatedClass;

        explicit ClassInfo(TCreateInfo info) : m_info(info) {}

    protected:
        TCreateInfo m_info;
    };
}

namespace
{
    // some helping objects
    struct ParameterOne;
    struct ParameterTwo;

    // Tutorial on how an object should be structurized.
    //
    // Step one: Every object class needs to have a create info struct containing everything 
    // that would usually be a constructor parameter as a member.
    // ++ It should just be named like the class it creates, with a "CreateInfo" appendix.
    struct MyClassCreateInfo
    {
        // The parameter names are the same as the member names.
        // All constructor arguments need a default value.
        // The create info constructor should not be marked as explicit.
        MyClassCreateInfo(ParameterOne* one = nullptr, ParameterTwo* two_and_more = nullptr)
            : one(one), two_and_more(two_and_more) {}

        // The setters should return a reference to the current create info to chain commands.
        // The parameter should always be called "value".
        // The whole function with body should remain in one line.
        MyClassCreateInfo& setOne(ParameterOne* value) { one = value; return *this; }

        // The naming scheme changes underscored member names to camel-case setters.
        MyClassCreateInfo& setTwoAndMore(ParameterTwo* value) { two_and_more = value; return *this; }

        // The members should be public.
        ParameterOne* one;
        ParameterTwo* two_and_more;
    };


    class MyClass : vkn::ClassInfo<MyClassCreateInfo, MyClass>
    {
    public:
        // The class should only have the one constructor passing the create info down to the class info base.
        // It should be marked explicit.
        explicit MyClass(const MyClassCreateInfo& info) : ClassInfo(info)
        {}
    };
}
