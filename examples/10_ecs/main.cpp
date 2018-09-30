#include "vec.hpp"
#include "mat.hpp"

#include <gfx/gfx.hpp>


volatile float x;

constexpr gfx::vec4 myvec(1.f, 1.f, 0.2f, 1.f);

auto t0()
{
	gfx::vec4 dm1(1.f);
    const auto now = std::chrono::steady_clock::now();
	for (int i = 0; i < 100000; ++i)
	{
		x = gfx::outer_prod(dm1, dm1)[1][1];
    }
    return std::chrono::steady_clock::now() - now;
}
auto t1()
{
		glm::vec4 dm1(1.f);
    const auto now = std::chrono::steady_clock::now();
	for (int i = 0; i < 100000; ++i)
	{
		x = glm::outerProduct(dm1, dm1)[1][1];
    }
    return std::chrono::steady_clock::now() - now;
}

int main()
{
	using namespace std::complex_literals;
	gfx::tcvec3<float> complex1_vec3(8.0+19i, 8i, 2i);
	gfx::tcvec3<float> complex2_vec3(8i, 4i, 56i);

	auto x = complex1_vec3.real();

	auto dm1 = gfx::diag<float, 4, 4>(1.f);
	auto dm2 = gfx::vec<float, 4>(1.f);
	dm1[0][0] = 2.f;
	dm1[1][1] = 2.f;
	auto mul = dm1 * dm2;

	auto dm3 = gfx::diag<float, 4, 4>(1.f);
	dm3[0][1] = 5.22f;
	transpose(dm3);

	dm3 = std::floor(dm3);

	auto xsaxca = det(gfx::diag<float, 4, 4>(2.f));

	std::cout << xsaxca << "\n";
	const auto mm = minmax_element(dm3);


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

    gfx::vec<float, 4> nv{1, 1, 1, 0};
    nv = normalize(nv);

    std::array<float, 4> arr{0};
    gfx::vec<float, 4>   v(arr.data());

    return 0;
}