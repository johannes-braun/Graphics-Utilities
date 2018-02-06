#include <jpu/log>

struct col
{
    constexpr col(uint32_t c) : _c(c) {}
    friend std::ostream& operator<<(std::ostream& s, const col& c);

    constexpr col operator+(uint32_t v) const { return _c+v; }

private:
    uint32_t _c;
};

std::ostream& operator<<(std::ostream& s, const col& c)
{
    return s << "\033[" << c._c << "m";
}

namespace cmd
{
    constexpr col fg_default{ 39 };
    constexpr col bg_default{ 49 };
    constexpr col bold{ 1 };
    constexpr col reset{ 0 };
    constexpr col fg_color(int r, int g, int b, int bright)
    {
        return 30 + 1 * (r != 0) + 2 * (g != 0) + 4 * (b != 0) + 60 * (bright != 0);
    }
    constexpr col bg_color(int r, int g, int b, int bright)
    {
        return fg_color(r, g, b, bright) + 10;
    }
}

int main(int argc, const char** args)
{
    log_i << ".." << "\n";

    std::cout << cmd::bg_color(1, 0, 0, 1) << cmd::fg_color(1, 1, 1, 1) << cmd::bold << " err " << cmd::reset
        << cmd::fg_color(0, 0, 0, 1) << " [ main.cpp:21 ] " << cmd::reset;


    system("pause");
    return 0;
}
