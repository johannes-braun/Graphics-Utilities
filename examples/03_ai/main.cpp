#include <algorithm>
#include <any>
#include <cassert>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>

template<typename Data>
class af;

template<typename T>
struct af_node
{
    friend class af<T>;

    T&       get() { return const_cast<T&>(_framework_ptr->_nodes[_handle_ptr]); }
    const T& get() const { return _framework_ptr->_nodes[_handle_ptr]; }

	void attack(const af_node& other, float weight = 1.f)
	{
		const_cast<af<T>*>(_framework_ptr)->attacks(*this, other, weight);
	}
	bool operator==(const af_node& b);
	bool operator!=(const af_node& b);
private:
    af_node() = default;
    template<typename D>
    af_node(ptrdiff_t h, const af<D>& a) : _handle_ptr(h), _framework_ptr(&a)
    {}

    ptrdiff_t    _handle_ptr;
    const af<T>* _framework_ptr;
};

template<typename T>
bool af_node<T>::operator==(const af_node<T>& b)
{
	return _framework_ptr == b._framework_ptr && _handle_ptr == b._handle_ptr;
}
template<typename T>
bool af_node<T>::operator!=(const af_node<T>& b)
{
	return _framework_ptr != b._framework_ptr || _handle_ptr != b._handle_ptr;
}

template<typename Data>
class af
{
public:
    friend struct af_node<Data>;
    using node_type = af_node<Data>;

    [[nodiscard]] node_type push(const Data& d) {
        node_type node;
        node._framework_ptr = static_cast<const af*>(this);
        node._handle_ptr    = static_cast<ptrdiff_t>(&_nodes.emplace_back(d) - &_nodes.front());
        return node;
    }[[nodiscard]] node_type push(Data&& d)
    {
        node_type node;
        node._framework_ptr = static_cast<const af*>(this);
        node._handle_ptr    = static_cast<ptrdiff_t>(&_nodes.emplace_back(std::forward<Data&&>(d)) - &_nodes.front());
        return node;
    }

    void attacks(const node_type& n, const node_type& target, float weight = 1.f)
    {
        assert(n._framework_ptr == this && target._framework_ptr == this);
        _attacks.emplace(n._handle_ptr, attack{target._handle_ptr, weight});
    }

    bool conflict_free(const std::vector<node_type>& nodes) const
    {
        for (auto& a : nodes)
        {
            assert(a._framework_ptr == this);
            for (auto& b : nodes)
                if (does_attack(a._handle_ptr, b._handle_ptr)) return false;
        }
        return true;
    }

    std::vector<std::vector<node_type>> all_preferred() const
    {
        auto pref = all_admissible();

        for (auto it = pref.begin(); it != pref.end();)
        {
            bool inc = false;
            for (auto oit = it + 1; oit != pref.end(); ++oit)
                if (std::includes(oit->begin(), oit->end(), it->begin(), it->end(),
                                  [](const auto& a, const auto& b) { return a._handle_ptr < b._handle_ptr; }))
                {
                    it  = pref.erase(it);
                    inc = true;
                    break;
                }
            if (!inc) ++it;
        }
        return pref;
    }

	std::vector<std::vector<node_type>> all_complete() const
	{
		auto adm = all_admissible();
		for (auto it = adm.begin(); it != adm.end(); ++it)
		{
			if (characteristic(*it) != *it)
				it = adm.erase(it);
			else
				++it;
		}
		return adm;
	}

    std::vector<std::vector<node_type>> all_admissible() const
    {
        auto ad = all_conflict_free();

        for (auto it = ad.begin(); it != ad.end();)
        {
            if (it->size() == 1)
            {
                const auto defends_all = [&](ptrdiff_t from) {
                    for (ptrdiff_t n = 0; n < static_cast<ptrdiff_t>(_nodes.size()); ++n)
                    {
                        if (does_attack(n, it->front()._handle_ptr, false) && !does_attack(from, n, false)) { return false; }
                    }
                    return true;
                };
                if (!defends_all(it->front()._handle_ptr))
                    it = ad.erase(it);
                else
                    ++it;
            }
            else
            {
                const bool any_defends = [&] {
                    for (auto& m : *it)
                    {
                        for (ptrdiff_t n = 0; n < static_cast<ptrdiff_t>(_nodes.size()); ++n)
                        {
                            if (does_attack(n, m._handle_ptr, false))
                            {
                                const auto defends_this = [&] {
                                    for (auto& c : *it)
                                    {
                                        if (does_attack(c._handle_ptr, n, false)) { return true; }
                                    }
                                    return false;
                                }();
                                if (!defends_this) return false;
                            }
                        }
                    }
                    return true;
                }();
                if (!any_defends) { it = ad.erase(it); }
                else
                {
                    ++it;
                }
            }
            if (it == ad.end()) break;
        }
        return ad;
    }
    std::vector<std::vector<node_type>> all_conflict_free() const
    {
		std::vector<std::vector<node_type>> cf{ {} };

        std::vector<node_type> permutation{node_type(0, *this)};
        for (int i = 0; i < _nodes.size(); ++i)
        {
            permutation[0]._handle_ptr = i;
            handle_combination(permutation, [&] {
                if (conflict_free(permutation)) cf.push_back(permutation);
            });
        }

        for (auto it = cf.begin(); it != cf.end(); ++it)
        {
            for (auto oit = it + 1; oit != cf.end();)
            {
                if (std::is_permutation(it->begin(), it->end(), oit->begin(), oit->end(),
                                        [](const auto& a, const auto& b) { return a._handle_ptr == b._handle_ptr; }))
                { oit = cf.erase(oit); } else
                {
                    ++oit;
                }
            }
        }

        std::sort(cf.begin(), cf.end(), [](const auto& a, const auto& b) { return a.size() < b.size(); });
        for (auto& v : cf)
        {
            std::sort(v.begin(), v.end(), [](const auto& a, const auto& b) { return a._handle_ptr < b._handle_ptr; });
        }

        return cf;
    }
    std::vector<node_type> characteristic(const std::vector<node_type>& input = {}) const
    {
        std::vector<node_type> set;

        for (auto i = 0ll; i < static_cast<ptrdiff_t>(_nodes.size()); ++i)
        {
            const auto any_attack = [&] {
                for (auto j = 0ll; j < static_cast<ptrdiff_t>(_nodes.size()); ++j)
                {
                    if (does_attack(j, i, false))
                    {
                        if (input.empty()) return true;
                        const auto aa = [&] {
                            for (auto& inp : input)
                                if (defends_via_impl(inp._handle_ptr, j, i)) return true;
                            return false;
                        }();
                        if (!aa) return true;
                    }
                }
                return false;
            }();
            if (!any_attack) set.push_back(node_type(i, *this));
        }
        return set;
    }

	std::vector<node_type> grounded() const
	{
		std::vector<node_type> cha;
		std::vector<node_type> next = characteristic({});
		while (cha.size() != next.size())
		{
			cha = std::move(next);
			next = characteristic(cha);
		}
		return cha;
	}
		 
private:
    bool defends_via_impl(ptrdiff_t a, ptrdiff_t via, ptrdiff_t b) const
    {
        return does_attack(via, b, false) && does_attack(a, via, false);
    }

    bool defends_impl(ptrdiff_t a, ptrdiff_t b) const
    {
        for (int i = 0; i < _nodes.size(); ++i)
            if (defends_via_impl(a, i, b)) return true;
        return false;
    }

    template<typename Fun>
    void handle_combination(std::vector<node_type>& c, Fun&& f) const
    {
        f();
        c.push_back(node_type(0, *this));
        for (int i = 0; i < _nodes.size(); ++i)
        {
            if (const auto it = std::find_if(c.begin(), c.end(), [&](const node_type& n) { return n._handle_ptr == i; }); it == c.end())
            {
                c.back()._handle_ptr = i;
                handle_combination(c, f);
            }
        }
        c.pop_back();
    }

    struct attack
    {
        ptrdiff_t target;
        float     weight = 1.f;
    };


    bool does_attack(ptrdiff_t a, ptrdiff_t b, bool also_reverse = true) const
    {
        const bool has_a = _attacks.count(a) != 0;

        if (has_a)
        {
            const auto rangea = _attacks.equal_range(a);
            for (auto i = rangea.first; i != rangea.second; ++i)
            {
                if (b == i->second.target) return true;
            }
        }

        if (!also_reverse) return false;

        const bool has_b = _attacks.count(b) != 0;

        if (has_b)
        {
            const auto rangeb = _attacks.equal_range(b);
            for (auto i = rangeb.first; i != rangeb.second; ++i)
            {
                if (a == i->second.target) return true;
            }
        }
        return false;
    }

    std::vector<Data>                          _nodes;
    std::unordered_multimap<ptrdiff_t, attack> _attacks;
};

int main()
{
    struct info
    {
        std::string val = "";
    };

    af<info>      framework;
    af_node<info> a  = framework.push({"A"});
    af_node<info> b   = framework.push({"B"});
    af_node<info> c    = framework.push({"C"});
    af_node<info> d = framework.push({"D"});
    af_node<info> e   = framework.push({"E"});
	af_node<info> f   = framework.push({"F"});

	a.attack(c);
	b.attack(d);
	d.attack(e);
	e.attack(f);
	f.attack(b);
	c.attack(e);

    const auto acf = framework.all_conflict_free();
    const auto aad = framework.all_admissible();
    const auto apr = framework.all_preferred();
	const auto grn = framework.grounded();
	for (auto&& x : apr)
	{
		std::cout << "{";
		for (auto&& c : x)
			std::cout << c.get().val << ", ";
		std::cout << "}, ";
	}
	//const auto cmp = framework.all_complete();

    return 0;
}