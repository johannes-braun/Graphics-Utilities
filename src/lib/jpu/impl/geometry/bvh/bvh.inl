#pragma once

namespace jpu::impl
{
    template<int Dim, int Bins>
    template<typename Index>
    typename bvh_creator<Dim, Bins>::bvh_type bvh_creator<Dim, Bins>::create(position_attribute p_attr, index_attribute<Index> i_attr, const void* pos_data, void* idx_data)
    {
        bvh_type bvh;
        bvh.attributes.position_stride = p_attr.stride;
        bvh.attributes.position_offset = p_attr.offset;
        bvh.attributes.index_stride = i_attr.stride;
        bvh.attributes.index_offset = i_attr.offset;
        if (i_attr.count == 0) return bvh;

        construction construction(bvh, i_attr.count);
        construction.ranges[0].start = 0;
        construction.ranges[0].end = static_cast<int32_t>(i_attr.count - 1);
        construction.ranges[0].parent = -1;

        pos_data = reinterpret_cast<const uint8_t*>(pos_data) + p_attr.offset;
        idx_data = reinterpret_cast<uint8_t*>(idx_data) + i_attr.offset;
        p_attr.offset = 0;
        i_attr.offset = 0;

        const int32_t concurrency = std::thread::hardware_concurrency();
        std::vector<impl::bounds> bounds(concurrency);

#pragma omp parallel for schedule(static)
        for (auto obj = 0; obj < static_cast<int32_t>(i_attr.count); ++obj)
        {
            impl::xyzw centroid(0);
            for (uint32_t part = 0; part < i_attr.per_object; ++part)
            {
                centroid = centroid + position_xyzw(obj, part, p_attr, i_attr, pos_data, idx_data);
            }

            auto&& cnt = construction.centroids[obj] = (centroid / static_cast<float>(i_attr.per_object));
            bounds[omp_get_thread_num()].expand(cnt);
        }

        for (const auto& b : bounds)
            construction.centroid_bounds[0].expand(b);

        for (std::atomic_int32_t dispatches = 1, ranges_offset = 0; const int count = dispatches; ++bvh.depth, ranges_offset = 0)
        {
#pragma omp parallel for schedule(dynamic)
            for (auto gid = 0; gid < count; ++gid)
                dispatches += sah_split(p_attr, i_attr, pos_data, idx_data, construction[gid], ranges_offset);

            bvh.node_count += count;

#pragma omp parallel for schedule(static)
            for (auto i = 0; i < dispatches; ++i)
            {
                construction.ranges[i] = construction.safe_ranges[i];
                construction.centroid_bounds[i] = construction.safe_centroid_bounds[i];
            }
        }

        return construction.bvh;
    }

    template <int Dim, int Bins>
    bvh_creator<Dim, Bins>::construction::construction(bvh_type& bvh, const size_t size) : bvh(bvh),
        centroid_bounds(size),
        ranges(size),
        safe_centroid_bounds(size),
        safe_ranges(size),
        centroids(size)
    {
        bvh.nodes.resize(2 * size);
    }

    template <int Dim, int Bins>
    typename bvh_creator<Dim, Bins>::construction::element bvh_creator<Dim, Bins>::construction::operator[](
        const int32_t gid)
    {
        return {
            bvh,
            gid + bvh.node_count,
            centroid_bounds[gid],
            ranges[gid],
            centroids,
            centroid_bounds,
            ranges,
            safe_centroid_bounds,
            safe_ranges
        };
    }

    template <int Dim, int Bins>
    template <typename Index>
    const float* bvh_creator<Dim, Bins>::position_index(uint32_t object_id, uint32_t object_internal_offset,
        position_attribute p_attr, index_attribute<Index> i_attr,
        const void* pos_data, void* idx_data)
    {
        return reinterpret_cast<const float*>(static_cast<const uint8_t*>(pos_data) + static_cast<Index*>(idx_data)[object_id*i_attr.per_object +
            object_internal_offset] * p_attr.stride);
    }

    template <int Dim, int Bins>
    template <typename Index>
    impl::xyzw bvh_creator<Dim, Bins>::position_xyzw(uint32_t object_id, uint32_t object_internal_offset,
        position_attribute p_attr, index_attribute<Index> i_attr,
        const void* pos_data, void* idx_data)
    {
        impl::xyzw vec = *reinterpret_cast<const impl::xyzw*>(position_index(object_id, object_internal_offset, p_attr, i_attr, pos_data, idx_data));
        return vec;
    }

    template<int Dim, int Bins>
    template<typename Index>
    int32_t bvh_creator<Dim, Bins>::sah_split(const position_attribute& p_attr, const index_attribute<Index>& i_attr,
        const void* pos_data, void* idx_data, const typename construction::element& element, std::atomic_int32_t& ranges_offset)
    {
        int32_t new_nodes = -1;
        const int32_t candidates = static_cast<int32_t>(element.current_range.end + 1 - element.current_range.start);

        if ((candidates > 1 || element.current_range.parent == -1) && !element.current_centroid_bounds.empty())
        {
            new_nodes += 2;

            // The current node is an inner node.
            const int32_t range_index = ranges_offset.fetch_add(1);

            bvh_best_sah best = sah_get_best(element);
            auto[left_range, right_range] = sah_sort_best(p_attr, i_attr, pos_data, idx_data, element, best);

            left_range.parent = element.current_node;
            right_range.parent = element.current_node;

            //writing the id_ranges and centboxes to the buffers for processing in following threads
            element.safe_ranges[range_index * 2] = left_range;
            element.safe_ranges[range_index * 2 + 1] = right_range;
            element.safe_centroid_bounds[range_index * 2] = best.left_bounds;
            element.safe_centroid_bounds[range_index * 2 + 1] = best.right_bounds;

            typename bvh_type::node node;
            for (auto i = element.current_range.start; i <= element.current_range.end; ++i)
            {
                for (uint32_t j = 0; j < i_attr.per_object; ++j)
                {
                    node.bounds.expand(position_xyzw(i, j, p_attr, i_attr, pos_data, idx_data));
                }
            }

            node.type = bvh_type::node_type::inner;
            node.parent = element.current_range.parent;

            //5. writing to the node_buffer for traversal in another kernel
            element.bvh.nodes[element.current_node] = node;
            if (element.current_range.parent != -1)
            {
                if (element.current_node % 2 == 0)
                    element.bvh.nodes[element.current_range.parent].child_right = element.current_node;
                else
                    element.bvh.nodes[element.current_range.parent].child_left = element.current_node;
            }
        }
        else
        {
            // The current node is a leaf node.
            auto&& new_leaf = element.bvh.nodes[element.current_node];
            for (auto i = element.current_range.start; i <= element.current_range.end; ++i)
            {
                for (uint32_t j = 0; j < i_attr.per_object; ++j)
                {
                    new_leaf.bounds.expand(position_xyzw(i, j, p_attr, i_attr, pos_data, idx_data));
                }
            }

            new_leaf.type = bvh_type::node_type::leaf;
            new_leaf.child_left = element.current_range.start;
            new_leaf.child_right = element.current_range.end;
            new_leaf.parent = element.current_range.parent;

            if (element.current_range.parent != -1)
            {
                if (element.current_node % 2 == 0)
                    element.bvh.nodes[new_leaf.parent].child_right = element.current_node;
                else
                    element.bvh.nodes[new_leaf.parent].child_left = element.current_node;
            }
        }

        return new_nodes;
    }

    template<int Dim, int Bins>
    typename bvh_creator<Dim, Bins>::bvh_best_sah bvh_creator<Dim, Bins>::sah_get_best(const typename construction::element& element)
    {
        bvh_best_sah best;

        for (auto axis = 0; axis < dimensions; ++axis)
        {
            std::array<typename bvh_type::bin, bin_count> bins;

            auto cb_axis_min = element.current_centroid_bounds.min[axis];
            auto cb_axis_max = element.current_centroid_bounds.max[axis];

            const float k = bin_count * (1 - bin_epsilon) / (cb_axis_max - cb_axis_min);

            for (auto candidate = element.current_range.start; candidate <= element.current_range.end; ++candidate)
            {
                const auto centroid = element.centroids[candidate];
                const auto id = static_cast<int32_t>(k * (centroid[axis] - cb_axis_min));
                bins[id].bounds.expand(centroid);
                ++bins[id].objects;
            }

            std::array<typename bvh_type::bin, plane_count> bins_left;
            bins_left[0].bounds.expand(bins[0].bounds);
            bins_left[0].objects = bins[0].objects;
            for (auto plane = 1; plane < plane_count; ++plane)
            {
                bins_left[plane].bounds.expand(bins_left[plane - 1].bounds);
                bins_left[plane].bounds.expand(bins[plane].bounds);
                bins_left[plane].objects = bins_left[plane - 1].objects + bins[plane].objects;
            }

            std::array<typename bvh_type::bin, plane_count> bins_right;
            for (auto plane = plane_count - 1; plane >= 0; --plane)
            {
                bins_right[plane].bounds.expand(bins[plane + 1].bounds);
                bins_right[plane].objects = bins[plane + 1].objects;

                if (plane != plane_count - 1)
                {
                    bins_right[plane].bounds.expand(bins_right[plane + 1].bounds);
                    bins_right[plane].objects += bins_right[plane + 1].objects;
                }

                const float surface_left = bins_left[plane].bounds.surface();
                const float surface_right = bins_right[plane].bounds.surface();

                const float cost = surface_left * bins_left[plane].objects + surface_right * bins_right[plane].objects;
                if (cost < best.cost)
                {
                    best.cost = cost;
                    best.axis = axis;
                    best.plane = plane;
                    best.centbox_min = cb_axis_min;
                    best.k = k;
                    best.left_bounds = bins_left[plane].bounds;
                    best.right_bounds = bins_right[plane].bounds;
                }
            }
        }
        return best;
    }

    template <int Dim, int Bins>
    template<typename Index>
    std::tuple<typename bvh<Dim, Bins>::range, typename bvh<Dim, Bins>::range> bvh_creator<Dim, Bins>::sah_sort_best(
        const position_attribute& p_attr, const index_attribute<Index>& i_attr,
        const void* pos_data, void* idx_data,
        const typename construction::element& element, bvh_best_sah& best)
    {

        //3. sorting the triangle ids to their correct ranges
        int left = element.current_range.start;
        int right = element.current_range.end;
        bool left_stopped = false;
        bool right_stopped = false;

        while (left < right)
        {
            if (!left_stopped)
            {
                const int bin_id = static_cast<int>(best.k * (element.centroids[left][best.axis] - best.centbox_min));
                if (bin_id > best.plane)
                {
                    left_stopped = true;
                }
                else
                {
                    ++left;
                }
            }
            if (!right_stopped)
            {
                const int bin_id = static_cast<int>(best.k * (element.centroids[right][best.axis] - best.centbox_min));
                if (bin_id <= best.plane)
                {
                    right_stopped = true;
                }
                else
                {
                    --right;
                }
            }
            if (left_stopped && right_stopped)
            {
                //swap triangles and centroids

                auto indices = reinterpret_cast<uint8_t*>(idx_data);

                for (uint32_t off = 0; off < i_attr.per_object; ++off)
                {
                    std::iter_swap(reinterpret_cast<Index*>(indices + i_attr.per_object * left * i_attr.stride) + off, 
                        reinterpret_cast<Index*>(indices + i_attr.per_object * right * i_attr.stride) + off);
                }
                std::swap(element.centroids[left], element.centroids[right]);

                left_stopped = false;
                right_stopped = false;
                ++left;
                --right;
            }
        }

        //determing left_range, right_range for the following threads
        typename bvh_type::range left_range;
        typename bvh_type::range right_range;
        left_range.start = element.current_range.start;
        right_range.end = element.current_range.end;
        if (left > right)
        {
            left_range.end = right;
            right_range.start = left;
        }
        else
        {
            if (left_stopped)
            {
                left_range.end = left - 1;
                right_range.start = left;
            }
            else if (right_stopped)
            {
                left_range.end = right;
                right_range.start = right + 1;
            }
            else
            {
                const int bin_id = static_cast<int>(best.k * (element.centroids[left][best.axis] - best.centbox_min));

                if (bin_id > best.plane)
                {
                    left_range.end = left - 1;
                    right_range.start = left;
                }
                else
                {
                    left_range.end = left;
                    right_range.start = left + 1;
                }
            }
        }

        return std::make_tuple(left_range, right_range);
    }
}
