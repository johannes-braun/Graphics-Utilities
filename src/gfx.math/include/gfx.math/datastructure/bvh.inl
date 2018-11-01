#pragma once

namespace gfx
{
    template<size_t Dimension>
    template<typename It, typename GetFun, typename>
    void bvh<Dimension>::sort(It begin, It end, GetFun get_vertex)
    {
        const int count = int(std::distance(begin, end) / float(_shape));
        if (count <= 0) return;
        _get_vertex = [begin, get_vertex](size_t index) -> vec_type { 
            auto v = get_vertex(*std::next(begin, index));
            return reinterpret_cast<vec_type&>(v); };

        _node_count = 0;
        _depth = 0;
        _nodes.clear();

        temporaries.centroids.resize(count, vec_type(0));
        temporaries.index_ranges.resize(count);
        temporaries.centroid_bounds.resize(count);
        temporaries.swap_index_ranges.resize(count);
        temporaries.swap_centroid_bounds.resize(count);
        _nodes.resize(2 * count);

        temporaries.index_ranges[0] = { /*start=*/ 0, /*end=*/ int(count - 1), /*parent=*/ -1 };

        const int concurrency = std::thread::hardware_concurrency();
        std::vector<bounds> bounds(concurrency);

#pragma omp parallel for schedule(static)
        for (int i = 0; i < count; ++i)
        {
            vec_type& centroid = temporaries.centroids[i];
            for (int c = 0; c < int(_shape); ++c)
                centroid += _get_vertex(i * int(_shape) + c);
            centroid /= float(_shape);
            bounds[omp_get_thread_num()] += centroid;
        }
        for (const auto& b : bounds)
            temporaries.centroid_bounds[0] += b;

        _node_count = 0;
        temporaries.range_position.store(0);
        for (std::atomic_int i{ 1 }; i.load() != 0;)
        {
            ++_depth;
            temporaries.range_position.store(0);
            const int count = i.load();
#pragma omp parallel for schedule(dynamic)
            for (int gid = 0; gid < count; ++gid)
                i += split_sah(begin, end, gid);

            _node_count += count;

#pragma omp parallel for schedule(static)
            for (auto p = 0; p < i; ++p)
            {
                temporaries.index_ranges[p] = std::move(temporaries.swap_index_ranges[p]);
                temporaries.centroid_bounds[p] = std::move(temporaries.swap_centroid_bounds[p]);
            }
        }

        temporaries.range_position.store(0);
        temporaries.centroids.resize(0);
        temporaries.index_ranges.resize(0);
        temporaries.centroid_bounds.resize(0);
        temporaries.swap_index_ranges.resize(0);
        temporaries.swap_centroid_bounds.resize(0);

        if (_mode != bvh_mode::persistent_iterators)
            _get_vertex = [](size_t index) { return vec_type(); };

		_nodes.resize(_node_count);
    }

    template<size_t Dimension>
    const std::vector<uint8_t>& bvh<Dimension>::pack(size_t vertex_stride, size_t vertex_offset, size_t index_stride, size_t index_offset)
    {
        using u32 = uint32_t;
        u32 vstr = u32(vertex_stride);
        u32 voff = u32(vertex_offset);
        u32 istr = u32(index_stride);
        u32 ioff = u32(index_offset);

        _packed_bytes.resize(_node_count * sizeof(node) + sizeof(u32) * 4);
        memcpy(_packed_bytes.data() + 0 * sizeof(u32), &vstr, sizeof(u32));
        memcpy(_packed_bytes.data() + 1 * sizeof(u32), &voff, sizeof(u32));
        memcpy(_packed_bytes.data() + 2 * sizeof(u32), &istr, sizeof(u32));
        memcpy(_packed_bytes.data() + 3 * sizeof(u32), &ioff, sizeof(u32));
        memcpy(_packed_bytes.data() + 4 * sizeof(u32), _nodes.data(), _node_count * sizeof(node));
        return _packed_bytes;
    }

    template<size_t Dimension>
    const std::vector<uint8_t>& bvh<Dimension>::get_packed() const noexcept
    {
        return _packed_bytes;
    }

    template<size_t Dimension>
    bvh<Dimension>::bvh(shape s, bvh_mode mode) : _shape(s), _mode(mode)
    {

    }

    template<size_t Dimension>
    bvh<Dimension>::bvh(const bvh& other)
    {
        operator=(std::forward<const bvh&>(other));
    }

    template<size_t Dimension>
    bvh<Dimension>& bvh<Dimension>::operator=(const bvh& other)
    {
        _nodes = other._nodes;
        _node_count = other._node_count;
        _depth = other._depth;
        _packed_bytes = other._packed_bytes;
        _get_vertex = other._get_vertex;
        return *this;
    }

    template<size_t Dimension>
    bvh<Dimension>::bvh(bvh&& other) noexcept {
        operator=(std::forward<bvh&&>(other));
    }

    template<size_t Dimension>
    bvh<Dimension>& bvh<Dimension>::operator=(bvh&& other) noexcept
    {
        _nodes = std::move(other._nodes);
        _node_count = other._node_count;
        _depth = other._depth;
        _packed_bytes = std::move(other._packed_bytes);
        _get_vertex = std::move(other._get_vertex);
        return *this;
    }

    template<size_t Dimension>
    typename bvh<Dimension>::bounds bvh<Dimension>::get_bounds() const
    {
        return _nodes[0].aabb;
    }

    template<size_t Dimension>
    bool bvh<Dimension>::intersect_ray_bounds(const vec_dim_type origin, const vec_dim_type direction, const bounds& bounds, const float max_distance, float* tmin) const noexcept
    {
        const vec_dim_type inv_direction = 1.f / direction;

        //intersections with box planes parallel to x, y, z axis
        const vec_dim_type t135 = (vec_dim_type(bounds.min) - origin) * inv_direction;
        const vec_dim_type t246 = (vec_dim_type(bounds.max) - origin) * inv_direction;

        const vec_dim_type min_values = min(t135, t246);
        const vec_dim_type max_values = max(t135, t246);

        const float t = glm::max(glm::max(min_values.x, min_values.y), min_values.z);
        const float tmax = glm::min(glm::min(max_values.x, max_values.y), max_values.z);
        if (tmin) *tmin = tmax;
        return tmax >= 0 && t <= tmax && t <= max_distance;
    }

    inline bool intersect_ray_triangle(
            const glm::vec3& origin,
            const glm::vec3& direction,
            const glm::vec3& v1,
            const glm::vec3& v2,
            const glm::vec3& v3,
            glm::vec2& barycentric,
            float& t)
    {
        using glm::vec3;
        float float_epsilon = 1e-23f;
        float border_epsilon = 1e-6f;

        //Find vectors for two edges sharing V1
        vec3 e1 = v2 - v1;
        vec3 e2 = v3 - v1;

        //if determinant is near zero, ray lies in plane of triangle
        vec3 P = cross(vec3(direction), e2);
        float det = dot(e1, P);
        if (det > -float_epsilon && det < float_epsilon)
            return false;

        //Calculate u parameter and test bound
        float inv_det = 1.f / det;
        vec3 T = vec3(origin) - v1;
        barycentric.x = dot(T, P) * inv_det;

        //The intersection lies outside of the triangle
        if (barycentric.x < -border_epsilon || barycentric.x > 1.f + border_epsilon)
            return false;

        //Calculate V parameter and test bound
        vec3 Q = cross(T, e1);
        barycentric.y = dot(vec3(direction), Q) * inv_det;
        //The intersection lies outside of the triangle
        if (barycentric.y < -border_epsilon || barycentric.x + barycentric.y  > 1.f + border_epsilon)
            return false;

        return (t = dot(e2, Q) * inv_det) > float_epsilon;
    }

    template<size_t Dimension>
    typename bvh<Dimension>::hit_result bvh<Dimension>::intersect_ray(const glm::vec3& origin, const glm::vec3& direction, const float max_distance, bool any) const
    {
        if (_mode != bvh_mode::persistent_iterators)
        {
            hit_result result;
            result.distance = max_distance;
            result.hits = false;
            return result;
        }
        switch (_shape)
        {
        case shape::triangle:
            return intersect_ray_with(origin, direction, max_distance, [&](hit_result last, int i, const std::list<vec_dim_type>& vectors) -> hit_result {
                auto beg_iter = vectors.begin();
                glm::vec2 barycentric;
                float distance = 0;
                const bool intersects = intersect_ray_triangle(origin, direction, *(beg_iter++), *(beg_iter++), *(beg_iter++), barycentric, distance);
                if (intersects && distance > 0 && distance < last.distance)
                {
                    last.hits = true;
                    last.distance = distance;
                    last.indices.clear();
                    last.indices.push_back({ uint32_t(3 * i + 1), barycentric.x });
                    last.indices.push_back({ uint32_t(3 * i + 2), barycentric.y });
                    last.indices.push_back({ uint32_t(3 * i + 0), 1 - barycentric.x - barycentric.y });
                }
                return last;
            }, any);
            break;

        default:
            if (!_custom_intersect) throw std::invalid_argument("For arbitrary shapes, you have to provide an explicit intersection function.");
            else return intersect_ray_with(origin, direction, max_distance, _custom_intersect, any);
        }
    }

    template<size_t Dimension>
    typename bvh<Dimension>::hit_result bvh<Dimension>::intersect_ray_with(const glm::vec3& origin, const glm::vec3& direction, const float max_distance, const intersect_function_type& intersect_function, bool any) const
    {
        hit_result result;
        result.distance = max_distance;
        result.hits = false;

        const node* root_node = _nodes.data();
        const node* current_node = _nodes.data();
        std::stack<const node*> node_stack;

        bool hits_scene = intersect_ray_bounds(origin, direction, current_node->aabb, max_distance, nullptr);

        while (hits_scene)
        {
            while (current_node->type == node_type::inner)
            {
                const node* left = root_node + current_node->child_left;
                const node* right = root_node + current_node->child_right;
                float min_left = std::numeric_limits<float>::max();
                float min_right = std::numeric_limits<float>::max();
                bool hits_left = intersect_ray_bounds(origin, direction, left->aabb, result.distance, &min_left);
                bool hits_right = intersect_ray_bounds(origin, direction, right->aabb, result.distance, &min_right);

                if (!hits_left && !hits_right)
                    break;

                bool nrm = min_left < min_right;
                const node* first = nrm ? left : right;
                const node* second = nrm ? right : left;
                bool hits_first = nrm ? hits_left : hits_right;
                bool hits_second = nrm ? hits_right : hits_left;

                if (hits_first && hits_second)
                    node_stack.push(second);
                current_node = hits_first ? first : second;
            }

            if (current_node->type == node_type::leaf)
            {
                for (uint32_t i = current_node->child_left; i != current_node->child_right + 1; ++i)
                {
                    std::list<vec_dim_type> vectors;
                    for (int s = 0; s < int(_shape); ++s)
                        vectors.push_back(vec_dim_type(_get_vertex(int(_shape)*i + s)));

                    result = intersect_function(result, i, vectors);
                    if (result && any)
                        return result;
                }
            }

            if (node_stack.empty()) return result;
            current_node = node_stack.top();
            node_stack.pop();
        }
        return result;
    }

    template<size_t Dimension>
    template<typename It>
    int bvh<Dimension>::split_sah(It begin, It end, int gid) noexcept
    {
        constexpr static int bin_count = 16;
        constexpr static int plane_count = bin_count - 1;
        constexpr static float bin_epsilon = 0.01f;

        const range& current_range = temporaries.index_ranges[gid];
        const bounds& current_bounds = temporaries.centroid_bounds[gid];
        const int candidates = int(current_range.end + 1 - current_range.start);
        const int current_node = int(gid + _node_count);

        if ((candidates > 1 || current_range.parent == -1) && !current_bounds.empty())
        {
            const int32_t range_index = temporaries.range_position.fetch_add(1);

            struct {
                float cost = std::numeric_limits<float>::max();
                float centbox_min = 0;
                float k = 0;
                int axis = 0;
                int plane = 0;
                bounds left_bounds;
                bounds right_bounds;
            } best;

            struct bin
            {
                bounds aabb;
                int objects = 0;
            };

            for (auto axis = 0; axis < Dimension; ++axis)
            {
                std::array<bin, bin_count> bins;

                auto cb_axis_min = current_bounds.min[axis];
                auto cb_axis_max = current_bounds.max[axis];

                const float k = bin_count * (1 - bin_epsilon) / (cb_axis_max - cb_axis_min);

                for (int candidate = current_range.start; candidate <= current_range.end; ++candidate)
                {
                    const auto centroid = temporaries.centroids[candidate];
                    const auto id = static_cast<int32_t>(k * (centroid[axis] - cb_axis_min));
                    bins[id].aabb += centroid;
                    ++bins[id].objects;
                }

                std::array<bin, plane_count> bins_left;
                bins_left[0].aabb += bins[0].aabb;
                bins_left[0].objects = bins[0].objects;
                for (int plane = 1; plane < plane_count; ++plane)
                {
                    bins_left[plane].aabb += bins_left[plane - 1].aabb;
                    bins_left[plane].aabb += bins[plane].aabb;
                    bins_left[plane].objects = bins_left[plane - 1].objects + bins[plane].objects;
                }

                std::array<bin, plane_count> bins_right;
                for (int plane = plane_count - 1; plane >= 0; --plane)
                {
                    bins_right[plane].aabb += bins[plane + 1].aabb;
                    bins_right[plane].objects = bins[plane + 1].objects;

                    if (plane != plane_count - 1)
                    {
                        bins_right[plane].aabb += bins_right[plane + 1].aabb;
                        bins_right[plane].objects += bins_right[plane + 1].objects;
                    }

                    const float surface_left = bins_left[plane].aabb.surface();
                    const float surface_right = bins_right[plane].aabb.surface();

                    const float exponent = 2;
                    const float cost = std::pow(surface_left, exponent) * bins_left[plane].objects + std::pow(surface_right, exponent) * bins_right[plane].objects;
                    if (cost < best.cost)
                    {
                        best.cost = cost;
                        best.axis = axis;
                        best.plane = plane;
                        best.centbox_min = cb_axis_min;
                        best.k = k;
                        best.left_bounds = bins_left[plane].aabb;
                        best.right_bounds = bins_right[plane].aabb;
                    }
                }
            }

            int left = current_range.start;
            int right = current_range.end;
            bool left_stopped = false;
            bool right_stopped = false;

            while (left < right)
            {
                if (!left_stopped)
                {
                    if (const int bin_id = int(best.k * (temporaries.centroids[left][best.axis] - best.centbox_min)); bin_id > best.plane)
                        left_stopped = true;
                    else
                        ++left;
                }
                if (!right_stopped)
                {
                    if (const int bin_id = int(best.k * (temporaries.centroids[right][best.axis] - best.centbox_min)); bin_id <= best.plane)
                        right_stopped = true;
                    else
                        --right;
                }
                if (left_stopped && right_stopped)
                {
                    //swap triangles and centroids
                    for (int off = 0; off < int(_shape); ++off)
                        std::iter_swap(std::next(begin, int(_shape)*left + off), std::next(begin, int(_shape)*right + off));
                    std::swap(temporaries.centroids[left], temporaries.centroids[right]);

                    left_stopped = false;
                    right_stopped = false;
                    ++left;
                    --right;
                }
            }

            //determing left_range, right_range for the following threads
            range left_range;
            range right_range;
            left_range.start = current_range.start;
            right_range.end = current_range.end;
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
                    const int bin_id = int(best.k * (temporaries.centroids[left][best.axis] - best.centbox_min));

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

            left_range.parent = current_node;
            right_range.parent = current_node;

            //writing the id_ranges and centboxes to the buffers for processing in following threads
            temporaries.swap_index_ranges[range_index * 2] = left_range;
            temporaries.swap_index_ranges[range_index * 2 + 1] = right_range;
            temporaries.swap_centroid_bounds[range_index * 2] = best.left_bounds;
            temporaries.swap_centroid_bounds[range_index * 2 + 1] = best.right_bounds;

            node node;
            for (int i = current_range.start; i <= current_range.end; ++i)
                for (int j = 0; j < int(_shape); ++j)
                    node.aabb += _get_vertex(i * int(_shape) + j);

            node.type = node_type::inner;
            node.parent = current_range.parent;

            //5. writing to the node_buffer for traversal
            _nodes[current_node] = node;
            if (current_range.parent != -1)
            {
                if (current_node % 2 == 0)
                    _nodes[current_range.parent].child_right = current_node;
                else
                    _nodes[current_range.parent].child_left = current_node;
            }

            return 1;
        }
        else
        {
            // The current node is a leaf node.
            auto&& new_leaf = _nodes[current_node];
            for (int i = current_range.start; i <= current_range.end; ++i)
                for (int j = 0; j < int(_shape); ++j)
                    new_leaf.aabb += _get_vertex(i * int(_shape) + j);

            new_leaf.type = node_type::leaf;
            new_leaf.child_left = current_range.start;
            new_leaf.child_right = current_range.end;
            new_leaf.parent = current_range.parent;

            if (current_range.parent != -1)
            {
                if (current_node % 2 == 0)
                    _nodes[new_leaf.parent].child_right = current_node;
                else
                    _nodes[new_leaf.parent].child_left = current_node;
            }

            return -1;
        }
    }
}