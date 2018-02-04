#pragma once

#include <array>
#include <string>
#include <random>
#include <sstream>

namespace jpu
{
    namespace
    {
        template<typename Dst, typename Src> constexpr Dst color_component_cast(Src s) { return Dst(s); }
        template<> constexpr uint8_t color_component_cast<uint8_t, float>(const float s) { return std::clamp(static_cast<uint8_t>(255.f*s), 0ui8, 255ui8); }
        template<> constexpr uint8_t color_component_cast<uint8_t, double>(const double s) { return std::clamp(static_cast<uint8_t>(255.f*s), 0ui8, 255ui8); }
        template<> constexpr float color_component_cast<float, uint8_t>(const uint8_t s) { return s / 255.f; }
        template<> constexpr double color_component_cast<double, uint8_t>(const uint8_t s) { return s / 255.0; }
    }

    template<typename T, size_t C>
    struct col;

    template<typename T>
    struct col<T, 1>
    {
        using value_type = T;

        union
        {
            T data[1];
            struct
            {
                T r;
            };
        };

        constexpr col() : r(0) {}
        constexpr col(T r) : r(r) {}
        template<typename P> constexpr col(col<P, 1> other) 
            : r(color_component_cast<T>(other.r)) {}

        T& operator[](size_t i) { return data[i]; }
        const T& operator[] (size_t i) const { return data[i]; }
    };

    template<typename T>
    struct col<T, 2>
    {
        using value_type = T;

        union
        {
            T data[2];
            struct
            {
                T r, g;
            };
        };

        constexpr col() : r(0), g(0) {}
        constexpr explicit col(T t) : r(t), g(t) {}
        constexpr explicit col(T r, T g) : r(r), g(g) {}

        constexpr col(col&&) = default;
        constexpr col(const col&) = default;
        template<typename P> constexpr col(col<P, 2> other) 
            : r(color_component_cast<T>(other.r)), g(color_component_cast<T>(other.g)) {}

        T& operator[](size_t i) { return data[i]; }
        const T& operator[] (size_t i) const { return data[i]; }
        
        explicit operator col<T, 1>() const { return col<T, 1>(r); }
    };

    template<typename T>
    struct col<T, 3>
    {
        using value_type = T;

        union
        {
            T data[3];
            struct
            {
                T r, g, b;
            };
        };

        constexpr col() : r(0), g(0), b(0) {}
        constexpr explicit col(T t) : r(t), g(t), b(t) {}
        constexpr explicit col(T r, T g, T b) : r(r), g(g), b(b) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(col<P, 2> rg, T b) : r(rg.r), g(rg.g), b(b) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(T r, col<P, 2> gb) : r(r), g(gb.g), b(gb.b) {}

        constexpr col(col&&) = default;
        constexpr col(const col&) = default;
        template<typename P> constexpr col(col<P, 3> other) 
            : r(color_component_cast<T>(other.r)), g(color_component_cast<T>(other.g)), b(color_component_cast<T>(other.b)) {}

        T& operator[](size_t i) { return data[i]; }
        const T& operator[] (size_t i) const { return data[i]; }

        explicit operator col<T, 2>() const { return col<T, 2>(r, g); }
        explicit operator col<T, 1>() const { return col<T, 1>(r); }
    };

    template<typename T>
    struct col<T, 4>
    {
        using value_type = T;

        union
        {
            T data[4];
            struct
            {
                T r, g, b, a;
            };
        };

        constexpr col() : r(0), g(0), b(0), a(0) {}
        constexpr explicit col(T t) : r(t), g(t), b(t), a(t) {}
        constexpr explicit col(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(col<P, 2> rg, T b, T a) : r(rg.r), g(rg.g), b(b), a(a) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(T r, T g, col<P, 2> ba) : r(r), g(g), b(ba.b), a(ba.a) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(T r, col<P, 2> gb, T a) : r(r), g(gb.g), b(gb.b), a(a) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(col<P, 3> rgb, T a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

        template<typename P, typename = std::enable_if_t<std::is_convertible_v<P, T>>>
        constexpr explicit col(T r, col<P, 3> gba) : r(r), g(gba.g), b(gba.b), a(gba.a) {}

        constexpr col(col&&) = default;
        constexpr col(const col&) = default;
        template<typename P> constexpr col(col<P, 4> other) 
            : r(color_component_cast<T>(other.r)), g(color_component_cast<T>(other.g)), b(color_component_cast<T>(other.b)), a(color_component_cast<T>(other.a)) {}

        T& operator[](size_t i) { return data[i]; }
        const T& operator[] (size_t i) const { return data[i]; }

        explicit operator col<T, 3>() const { return col<T, 3>(r, g, b); }
        explicit operator col<T, 2>() const { return col<T, 2>(r, g); }
        explicit operator col<T, 1>() const { return col<T, 1>(r); }
    };

    using col1f = col<float, 1>;
    using col2f = col<float, 2>;
    using col3f = col<float, 3>;
    using col4f = col<float, 4>;

    using col1u8 = col<uint8_t, 1>;
    using col2u8 = col<uint8_t, 2>;
    using col3u8 = col<uint8_t, 3>;
    using col4u8 = col<uint8_t, 4>;

    namespace color
    {
        namespace preset
        {
            const col4f white{ 1, 1, 1, 1 };
            const col4f black{ 0, 0, 0, 1 };
            const col4f clear{ 0, 0, 0, 0 };
            const col4f grey{ 0.5, 0.5, 0.5, 0.5 };

            const col4f red{ 1, 0, 0, 1 };
            const col4f green{ 0, 1, 0, 1 };
            const col4f blue{ 0, 0, 1, 1 };

            const col4f cyan{ 0, 1, 1, 1 };
            const col4f magenta{ 1, 0, 1, 1 };
            const col4f yellow{ 1, 1, 0, 1 };
        }

        template<typename T> struct components { constexpr static size_t length = sizeof(T) / sizeof(typename T::value_type); };

        template<typename TVec> struct ubyte_equivalent;
        template<size_t L, typename T>
        struct ubyte_equivalent<col<T, L>>
        {
            using type = col<uint8_t, L>;
        };
        template<typename T> using ubyte_equivalent_t = typename ubyte_equivalent<T>::type;

        template<typename T>
        struct component_count
        {
            constexpr static size_t value = sizeof(T) / sizeof(typename T::value_type);
        };
        template<typename T> constexpr size_t component_count_v = component_count<T>::value;

        template<typename TOut, typename TIn>
        std::enable_if_t<std::is_same<TOut, TIn>::value, TOut> convert(const std::enable_if_t<std::is_same<TOut, TIn>::value, TIn> &in)
        {
            return in;
        }

        template<typename TOut, typename TIn>
        std::enable_if_t<std::is_same<uint8_t, typename TOut::value_type>::value, TOut> convert(const std::enable_if_t<std::is_same<float, typename TIn::value_type>::value, TIn> &in)
        {
            TOut output;
            for (auto c = 0; c < component_count_v<TOut>; ++c)
            {
                if (c < component_count_v<TIn>)
                {
                    output[c] = static_cast<uint8_t>(in[c] * 255.f);
                }
                else
                {
                    output[c] = static_cast<uint8_t>(c == 3) * 255;
                }
            }

            return output;
        }

        template<typename TOut, typename TIn>
        std::enable_if_t<std::is_same<float, typename TOut::value_type>::value, TOut> convert(const std::enable_if_t<std::is_same<uint8_t, typename TIn::value_type>::value, TIn> &in)
        {
            TOut output;
            for (auto c = 0; c < component_count_v<TOut>; ++c)
            {
                if (c < component_count_v<TIn>)
                {
                    output[c] = static_cast<float>(in[c]) / 255.f;
                }
                else
                {
                    output[c] = static_cast<float>(c == 3); // 1 for alpha, 0 for all others.
                }
            }

            return output;
        }

        template<typename T>
        uint32_t as_hex(const std::enable_if_t<std::is_same<uint8_t, typename T::value_type>::value, T> &in)
        {
            uint32_t output = 0;
            for (auto c = 0; c < component_count_v<T>; ++c)
            {
                output |= in[c] << (c * 8);
            }

            if (component_count_v<T> < 4)
            {
                output |= 0xff << 24;
            }

            return output;
        }

        template<typename T>
        uint32_t as_hex(const std::enable_if_t<std::is_same<float, typename T::value_type>::value, T> &in)
        {
            return as_hex<ubyte_equivalent_t<T>>(convert<ubyte_equivalent_t<T>, T>(in));
        }

        //formatted as 0xaarrggbb
        //implementation for ubyte types
        template<typename T>
        std::enable_if_t<std::is_same<uint8_t, typename T::value_type>::value, T> hex(uint32_t in)
        {
            col4u8 tmp(reinterpret_cast<col3u8&>(in), ((in) & 0xFF)); 

            return T(tmp);
        }

        //implementation for float types
        template<typename T>
        std::enable_if_t<std::is_same<float, typename T::value_type>::value, T> hex(uint32_t in)
        {
            return convert<T, ubyte_equivalent_t<T>>(hex<ubyte_equivalent_t<T>>(in));
        }

        template<typename T>
        T random()
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<uint32_t> dist(0x0, 0xffffffff);
            return hex<T>(dist(gen));
        }

        //Format: "R G B A" or equivalent
        // "0.1 0.1 0.1"
        // "1 0.5 0.3 1"
        // "1,0.5,0.3,1"
        // etc.
        template<typename T>
        std::enable_if_t<std::is_same<float, typename T::value_type>::value, T> from_array_string(const std::string &in, char separator = ' ')
        {
            T result;
            std::istringstream ss(in);
            std::string token;
            unsigned index = 0;
            while (std::getline(ss, token, separator))
            {
                if (index > static_cast<unsigned>(component_count_v<TIn> -1))
                {
                    break;
                }

                result[index++] = static_cast<float>(atof(token.c_str()));
            }

            if (index < 3 && component_count_v<T> == 4)
            {
                result[3] = 1.f;
            }

            return result;
        }

        //Format: "R G B A" or equivalent
        // "10 10 10"
        // "255 211 140 255"
        // "255,113,24,12"
        // etc.
        template<typename T>
        std::enable_if_t<std::is_same<uint8_t, typename T::value_type>::value, T> from_array_string(const std::string &in, char separator = ' ')
        {
            T result;
            std::istringstream ss(in);
            std::string token;
            unsigned index = 0;
            while (std::getline(ss, token, separator))
            {
                if (index > unsigned(component_count_v<TIn> -1))
                {
                    break;
                }

                result[index++] = static_cast<uint8_t>(std::clamp(atoi(token.c_str()), 0, 255));
            }

            if (index < 3 && component_count_v<TIn> == 4)
            {
                result[3] = 255;
            }

            return result;
        }

        //implementation for parsing strings.
        // Format: #RRGGBBAA
        // Parses string formats like:
        // 0x324    -> b value to 0x24, r value to 0x03, g to 0x00, a to 0xff
        // 333      -> rgb values to 0x33
        // #3		-> r, g, b values to 0x33, a value to 0xff
        // #36      -> rgb each to 0x36, a to 0xff
        // #333		-> r, g and b values to 0x33, a value to 0xff
        // #3333	-> r, g, b and a values to 0x33
        // #33351   -> rgb each to 0x33, a to 0x51
        // #333333	-> rgb each to 0x33, a to 0xff
        // #3333336 -> rgb each 0x33, a to 0x66
        // #33333333-> same as #3333
        // #4790124774120 -> clamped down to #47901247
        template<typename T>
        T from_hex_string(const std::string &in)
        {
            if (in.length() >= 1 && in[0] == '#')
            {
                // I'll let this here to support formats like "#ff00ff"
                return from_hex_string<T>(in.data() + 1);
            }

            if (in.length() >= 2 && in[0] == '0' && in[1] == 'x')
            {
                uint32_t value = static_cast<uint32_t>(strtoul(in.c_str(), nullptr, 16));
                //call the uint-hex-function
                return hex<T>(value);
            }

            //now the string has to be sized accordingly.
            switch (in.size())
            {
            case 0:
                return T(0);
            case 1:
                // #a -> #aaaaaaff
                return from_hex_string<T>(std::string(6, in[0]) + "ff");
            case 2:
                // #3f -> #3f3f3fff
                return from_hex_string<T>(in + in + in + "ff");
            case 3:
                // #a92 -> #aa9922ff
                return from_hex_string<T>(std::string(2, in[0]) + std::string(2, in[1]) + std::string(2, in[2]) + "ff");
            case 4:
                // #abc7 -> #aabbcc77
                return from_hex_string<T>(std::string(2, in[0]) + std::string(2, in[1]) + std::string(2, in[2]) + std::string(2, in[3]));
            case 5:
                // #a94f3 -> #aa9944f3 (alpha & 0xff, rest & 0xf)
                return from_hex_string<T>(std::string(2, in[0]) + std::string(2, in[1]) + std::string(2, in[2]) + std::string(in.c_str() + 3));
            case 6:
                // #a99a7c -> #a99a7cff
                return from_hex_string<T>(in + "ff");
            case 7:
                // #2014fa8 -> #2014fa88 (alpha & 0xf, rest & 0xff)
                return from_hex_string<T>(in.substr(0, 6) + std::string(2, in[6]));
            case 8:
            {
                uint32_t value = static_cast<uint32_t>(strtoul(in.c_str(), nullptr, 16));
                ////Swap around from css-like 0xrrggbbaa format to easier to convert 0xaarrggbb format.
                //value = ((value >> 8) & 0xffffff) | ((value & 0xff) << 24);
                //call the uint-hex-function
                return hex<T>(value);
            }
            default:
                // larger than 8, resize down
                std::string new_input = in;
                new_input.resize(8);
                return from_hex_string<T>(new_input);
            }
        }
    }
}
