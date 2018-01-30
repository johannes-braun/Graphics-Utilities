#pragma once
#include <map>
#include <sstream>

namespace jpu
{
    class args
    {
    public:
        void add_mapping(char key, std::string long_key);
        void parse(int arg_count, char** args);

        bool has(std::string key) const;
        bool has(char key) const;

        template<typename T> T get_as(char key, const T& if_empty = T()) const;
        template<typename T> T get_as(const std::string& key, const T& if_empty = T()) const;

        const std::string& executable_name() const;

    private:
        std::string _exec_name;
        std::map<std::string, char> _long_key_mapping;
        std::map<char, std::string> _key_value;
    };

    template <typename T>
    T args::get_as(char key, const T& if_empty) const
    {
        if(has(key))
        {
            std::stringstream str(_key_value.at(key));
            T val;
            str >> val;
            return val;
        }
        return if_empty;
    }

    template <typename T>
    T args::get_as(const std::string& key, const T& if_empty) const
    {
        if(has(key))
        {
            std::stringstream str(_key_value.at(_long_key_mapping.at(key)));
            T val;
            str >> val;
            return val;
        }
        return if_empty;
    }
}
