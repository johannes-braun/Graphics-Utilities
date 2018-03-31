#include "huffman.hpp"

namespace huffman
{
    struct node {
        uint32_t f;
        uint8_t val;
        int32_t left;
        int32_t right;
        int32_t parent;
        int8_t tag;
        constexpr node(uint32_t f, uint8_t v, int32_t l, int32_t r, int32_t p, int8_t t)
            : f(f), val(v), left(l), right(r), parent(p), tag(t) {}
    };

    struct node_comparator {
        constexpr bool operator()(node* one, node* other) const noexcept
        {
            return one->f > other->f;
        }
    };

    std::vector<node> build_tree(const std::array<uint32_t, 256>& histogram, uint32_t non_zero_count)
    {
        std::vector<node> nodes;
        nodes.reserve(2 * non_zero_count);
        std::priority_queue<node*, std::vector<node*>, node_comparator> queue;
        const auto index_of = [&nodes](const node* n) { return size_t(n-nodes.data()); };

        for (int i=0; i<256; ++i)
        {
            if (histogram[i] > 0)
            {
                nodes.emplace_back(histogram[i], uint8_t(i), -1, -1, -1, -1);
                queue.push(&nodes.back());
            }
        }

        while (!queue.empty())
        {
            node* left = queue.top();
            queue.pop();
            if (!queue.empty())
            {
                node* right = queue.top();
                queue.pop();

                nodes.emplace_back(left->f + right->f, 0, int32_t(index_of(left)), int32_t(index_of(right)), -1, -1);
                left->parent = index_of(&nodes.back());
                left->tag = 0;
                right->parent = index_of(&nodes.back());
                right->tag = 1;
                queue.push(&nodes.back());
            }
        }
        return nodes;
    }

    struct code {
        uint32_t signature;
        uint32_t length;
    };

    std::array<code, 256> generate_codes(const std::vector<node>& tree, const uint32_t leaf_nodes)
    {
        std::array<code, 256> codes{ 0 };
        for (int i=0; i<256; ++i)
        {
            if (const auto it = std::find_if(tree.begin(), tree.begin() + leaf_nodes, [i](const node& n) { return n.val == uint8_t(i); }); it != tree.end())
            {
                const node* n       = &*it;
                codes[i].signature  = 0;
                codes[i].length     = 0;
                while (n->tag != -1)
                {
                    codes[i].signature <<= 1;
                    codes[i].signature |= (uint32_t(n->tag) & 0x1);
                    n = &tree[n->parent];
                    ++codes[i].length;
                }
            }
        }
        return codes;
    }
    std::basic_string<uint8_t> encode(const std::basic_string<uint8_t>& in)
    {
        return encode(in.data(), in.size());
    }

    std::basic_string<uint8_t> encode(const std::vector<uint8_t>& in)
    {
        return encode(in.data(), in.size());
    }

    std::basic_string<uint8_t> encode(const uint8_t* in, size_t in_length)
    {
        uint32_t count = 0;
        std::array<uint32_t, 256> histogram{ 0 };
        std::array<bool, 256> count_lock{ false };
        std::basic_stringstream<uint8_t> stream;

        for (size_t i = 0; i < in_length; ++i)
        {
            if (histogram[in[i]] == 0)
                ++count;
            ++histogram[in[i]];
        }
        stream.write(reinterpret_cast<uint8_t*>(histogram.data()), sizeof(uint32_t) * histogram.size());

        std::vector<node>     nodes = build_tree(histogram, count);
        std::array<code, 256> codes = generate_codes(nodes, count);

        int bp = 0;
        uint8_t c = 0;
        const auto write_bit = [&stream, &bp, &c](uint8_t bit)
        {
            c = c | (bit << bp);
            bp = (bp+1) & 0x7;
            if (bp==0)
            {
                stream.put(c);
                c = 0;
            }
        };

        for (size_t x = 0; x < in_length; ++x)
            for (int i=0; i<codes[in[x]].length; ++i)
                write_bit(((codes[in[x]].signature & (1 << i)) >> i) & 0x1);

        if (c != 0)
            stream.put(c);

        return stream.str();
    }

    std::basic_string<uint8_t> decode(const std::basic_string<uint8_t>& in)
    {
        std::basic_stringstream<uint8_t> stream;
        std::array<uint32_t, 256> histogram{ 0 };
        uint32_t count = 0;
        memcpy(&histogram[0], in.data(), 256*sizeof(uint32_t));
        int in_ptr = 256 * sizeof(uint32_t);
        for (int i=0; i<256; ++i)
        {
            count += uint32_t(histogram[i] != 0);
        }

        std::vector<node>     nodes = build_tree(histogram, count);
        std::array<code, 256> codes = generate_codes(nodes, count);

        int bp = 0;
        uint8_t c = in[in_ptr++];
        const auto read_bit = [&in, &in_ptr, &bp, &c]()
        {
            uint8_t bit = (c >> bp) % 2;
            bp = (bp+1) & 0x7;
            if (bp==0)
            {
                if (in_ptr < in.size())
                    c = in[in_ptr++];
                else
                    c = 0;
            }
            return bit;
        };

        /* nodes.back() is the tree root. */
        uint32_t symb_count = nodes.back().f;
        while (symb_count > 0)
        {
            const node* n = &nodes.back();
            while (n->right != -1)
            {
                const uint32_t bit = read_bit();
                n = &nodes[bit ? n->right : n->left];
            }
            stream.put(n->val);
            --symb_count;
        }

        return stream.str();
    }
}