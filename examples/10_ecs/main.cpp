#include "vec.hpp"
#include "mat.hpp"
#include "quat.hpp"

#include <gfx/gfx.hpp>


volatile float x;

constexpr gfx::vec4 myvec(1.f, 1.f, 0.2f, 1.f);

auto t0()
{
	gfx::vec4 dm1(1.f);
    const auto now = std::chrono::steady_clock::now();
	for (int i = 0; i < 100000; ++i)
	{
		x = transposed(inverse(gfx::outer_prod(dm1, dm1)))[1][1];
    }
    return std::chrono::steady_clock::now() - now;
}
auto t1()
{
	glm::vec4 dm1(1.f);
    const auto now = std::chrono::steady_clock::now();
	for (int i = 0; i < 100000; ++i)
	{
		x = transpose(inverse(glm::outerProduct(dm1, dm1)))[1][1];
    }
    return std::chrono::steady_clock::now() - now;
}

int main()
{
	const float f = gfx::det(gfx::mat4(gfx::identity));
	const auto c = gfx::cols(gfx::mat4(8.f, gfx::diagonal));
	const auto inv = gfx::inverse(gfx::mat4(2.f, gfx::diagonal));

	gfx::quat quat(gfx::to_rad(90), gfx::vec3(1, 0, 0));
	const auto iq = inverse(quat) * gfx::vec3(0, 0, 1);

	constexpr auto so = sizeof(gfx::u8vec3);

	gfx::vec3 larger(1.f, 4.f, 9.f);
	gfx::vec3 smaller(0.2f, 89.1f, 4.f);

	auto xaxax = larger > smaller;
	const auto comp = all_of(xaxax);

	gfx::mat<float, 7> mine(gfx::mat4(8.f, gfx::diagonal));
	gfx::mat<float, 7> inve = gfx::inverse(mine);

	gfx::vec<bool, 3> vasdc(true, false, false);
	vasdc == gfx::vec<bool, 3>(false, false, true);



	gfx::mat4 trafo = gfx::as_mat(quat);

	const auto xscad = -8.f + (mine + inve);

	using namespace std::complex_literals;
	gfx::tcvec3<float> complex1_vec3(8.0+19i, 8i, 2i);
	gfx::tcvec3<float> complex2_vec3(8i, 4i, 56i);

	auto x = complex1_vec3.real();

	auto dm1 = gfx::make_diag<float, 4, 4>(1.f);
	auto dm2 = gfx::vec<float, 4>(1.f);
	dm1[0][0] = 2.f;
	dm1[1][1] = 2.f;
	auto mul = dm1 * dm2;

	auto dm3 = gfx::make_diag<float, 4, 4>(1.f);
	dm3[0][1] = 5.22f;
	transpose(dm3);

	dm3 = std::floor(dm3);

	auto xsaxca = det(gfx::make_diag<float, 4, 4>(2.f));

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