#pragma once

#include <array>
#if __has_include(<omp.h>)
#include <omp.h>
#else
inline int omp_get_thread_num() { return 1; }
#endif
#include <algorithm>
#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

#include "bvh_config.hpp"

#ifdef JPU_BVH_HEADER_ONLY
#define JPU_BVH_INL inline
#else
#define JPU_BVH_INL
#endif
#include "attr.hpp"

#include "bvh_impl/xyzw.hpp"
#ifdef JPU_BVH_HEADER_ONLY
#include "bvh_impl/xyzw.cpp"
#endif

#include "bvh_impl/bounds.hpp"
#ifdef JPU_BVH_HEADER_ONLY
#include "bvh_impl/bounds.cpp"
#endif

namespace jpu
{
    template<int Dim, int Bins>
    class bvh;

    namespace impl
    {
        template<int Dim, int Bins = 16>
        class bvh_creator
        {
        public:
            constexpr static int32_t dimensions = Dim;

            using node_type = typename bvh<Dim, Bins>::node;
            using bvh_type = bvh<Dim, Bins>;

            struct bvh_best_sah
            {
                float cost = std::numeric_limits<float>::max();
                float centbox_min = 0;
                float k = 0;
                int axis = 0;
                int plane = 0;
                bounds left_bounds;
                bounds right_bounds;
            };

            constexpr static int bin_count = Bins;
            constexpr static int plane_count = bin_count - 1;
            constexpr static float bin_epsilon = 0.01f;

            template<typename Index>
            static bvh_type create(position_attribute p_attr, index_attribute<Index> i_attr, const void* pos_data, void* idx_data);

        private:
            struct construction
            {
                explicit construction(bvh_type& bvh, size_t size);

                struct element
                {
                    element() = delete;
                    bvh_type& bvh;
                    int32_t current_node;
                    bounds current_centroid_bounds;
                    typename bvh_type::range current_range;
                    std::vector<xyzw>& centroids;
                    std::vector<bounds>& centroid_bounds;
                    std::vector<typename bvh_type::range>& ranges;
                    std::vector<bounds>& safe_centroid_bounds;
                    std::vector<typename bvh_type::range>& safe_ranges;
                };

                element operator[](int32_t gid);

                bvh_type& bvh;

                std::vector<bounds> centroid_bounds;
                std::vector<typename bvh_type::range> ranges;
                std::vector<bounds> safe_centroid_bounds;
                std::vector<typename bvh_type::range> safe_ranges;
                std::vector<xyzw> centroids;
            };

            template<typename Index>
            static const float* position_index(uint32_t object_id, uint32_t object_internal_offset, position_attribute p_attr, index_attribute<Index> i_attr, const void* pos_data, void* idx_data);

            template<typename Index>
            static xyzw position_xyzw(uint32_t object_id, uint32_t object_internal_offset, position_attribute p_attr, index_attribute<Index> i_attr, const void* pos_data, void* idx_data);

            template<typename Index>
            static int32_t sah_split(const position_attribute& p_attr, const index_attribute<Index>& i_attr, const void* pos_data, void* idx_data, const typename construction::element& element, std::atomic_int32_t& ranges_offset);

            static bvh_best_sah sah_get_best(const typename construction::element& element);

            template<typename Index>
            static std::tuple<typename bvh_type::range, typename bvh_type::range> sah_sort_best(const index_attribute<Index>& i_attr, void* idx_data, const typename construction::element& element, bvh_best_sah& best);
        };
    } // namespace impl

    enum class bvh_primitive_type
    {
        vertices = 1,
        lines,
        triangles,
        quads
    };

    template<int Dim, int Bins = 16>
    class bvh
    {
        friend impl::bvh_creator<Dim, Bins>;
        struct bin
        {
            impl::bounds bounds;
            int32_t objects = 0;
        };

        struct range
        {
            int32_t start;
            int32_t end;
            int32_t parent;
        };

    public:
        enum class node_type : uint32_t
        {
            inner = 0,
            leaf
        };

        struct node
        {
            impl::bounds bounds;
            node_type type = node_type::leaf;

            // Hierarchial info
            int32_t child_left = -1;
            int32_t child_right = -1;
            int32_t parent = -1;
        };

        size_t byte_size() const
        {
            return nodes.size() * sizeof(node);
        }

        const node* data() const
        {
            return nodes.data();
        }

        size_t packed_size() const
        {
            return node_count * sizeof(nodes[0]) + sizeof(attributes);
        }

        std::vector<uint8_t> pack() const
        {
            std::vector<uint8_t> packed_bvh(packed_size());
            memcpy(packed_bvh.data(), &attributes, sizeof(attributes));
            memcpy(packed_bvh.data() + sizeof(attributes), data(), node_count * sizeof(node));
            return packed_bvh;
        }
        
        int depth = 0;
        int node_count = 0;
        std::vector<node> nodes;
        struct attribute_t
        {
            uint32_t position_stride;
            uint32_t position_offset;
            uint32_t index_stride;
            uint32_t index_offset;
        } attributes;

        template<typename VertexContainer, typename IndexContainer, typename Member>
        bvh& assign_to(IndexContainer& indices, const VertexContainer& vertices, Member VertexContainer::value_type::* position_field, bvh_primitive_type type)
        {
            const auto count = static_cast<uint32_t>(type);
            return (*this = std::move(impl::bvh_creator<Dim, Bins>::create(
                make_position_attrib(position_field),
                make_index_attrib<std::decay_t<decltype(indices[0])>>(static_cast<uint32_t>(indices.size()) / count, count),
                vertices.data(),
                indices.data()
            )));
        }

        template<typename Positions, typename IndexContainer, typename Member>
        bvh& assign_to(IndexContainer& indices, const Positions& positions, bvh_primitive_type type)
        {
            position_attribute attr{ sizeof(typename Positions::value_type), 0 };
            const auto count = static_cast<uint32_t>(type);
            return (*this = std::move(impl::bvh_creator<Dim, Bins>::create(
                attr,
                make_index_attrib<typename IndexContainer::value_type>(static_cast<uint32_t>(indices.size()) / count, count),
                positions.data(),
                indices.data()
            )));
        }
    };
} // namespace jpu

#include "bvh.inl"