#include "args.hpp"
#include <cassert>

namespace jpu
{
    void args::add_mapping(char key, std::string long_key)
    {
        _long_key_mapping[long_key] = key;
    }

    void args::parse(int arg_count, char** args)
    {
        assert(arg_count >= 1 && ((arg_count - 1) & 1 == 0) && args && "Invalid arguments.");
        _exec_name = args[0];

        for (int i = 1; i < arg_count; i += 2)
        {
            if (args[i][0] != '-') throw std::invalid_argument("Expected key. Got " + std::string(args[i]));

            if (args[i][1] == '-')
            {
                // long key
                const std::string exp(args[i] + 2);
                if(_long_key_mapping.count(exp) == 0)
                    continue;
                _key_value[_long_key_mapping.at(exp)] = args[i + 1];
            }
            else
            {
                _key_value[args[i][1]] = args[i + 1];
            }
        }
    }

    bool args::has(std::string key) const
    {
        return _long_key_mapping.count(key) != 0 && _key_value.count(_long_key_mapping.at(key)) != 0;
    }

    bool args::has(char key) const
    {
        return _key_value.count(key) != 0;
    }

    const std::string& args::executable_name() const
    {
        return _exec_name;
    }
}
