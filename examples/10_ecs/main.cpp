#include "vec.hpp"

#include <gfx/gfx.hpp>


volatile float x;

auto t0()
{
    const auto now = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000; ++i)
    {
        gfx::vec<float, 3> vector1(10);
        x = cross(vector1, vector1 * vector1)[2];
    }
    return std::chrono::steady_clock::now() - now;
}
auto t1()
{
    const auto now = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000; ++i)
    {
        glm::vec<3, float> vector1(10);
        x = cross(vector1, vector1 * vector1)[2];
    }
    return std::chrono::steady_clock::now() - now;
}

int main()
{
    {
        long long avg = 0;
        for (int i = 0; i < 100; ++i) avg += t0().count() / 100;
        std::cout << avg << "ns\n";
    }

    {
        long long avg = 0;
        for (int i = 0; i < 100; ++i) avg += t1().count() / 100;
        std::cout << avg << "ns\n";
    }

    glm::vec4     test;
    gfx::vec<float, 4> other = test;
    test += other;

    std::array<float, 4> arr{0};
    gfx::vec<float, 4>   v(arr.data());

    return 0;
}