#include <gfx/gfx.hpp>

namespace gfx {
inline namespace v1 {
enum class binding
{
    invalid = 0,
    buf_uniform,
    buf_storage,
    img_sampled,
    img_storage
};

struct bindings
{
    std::map<u32, binding> bindings;
};

// This approach is kinda not good.

struct set
{
	void bind(...);
};

struct desc
{
	std::unique_ptr<set> create_set(u32 index) 
	{

	}

    std::map<u32, bindings> sets;
};

void blub(u32 set, std::reference_wrapper<const gfx::desc> desc) {}
}    // namespace v1
}    // namespace gfx

int main()
{
    gfx::sampler s;

    gfx::desc d;
    d.sets[0] = {{{0, gfx::binding::buf_storage}, {1, gfx::binding::buf_uniform}}};
    d.sets[1] = {{{0, gfx::binding::img_storage}}};

	auto x = d.create_set(0);
	x->bind(0, s);
}