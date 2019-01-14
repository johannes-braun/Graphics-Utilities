#pragma once

#include <array>
#include <gfx.core/flags.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.math/datastructure/bvh.hpp>
#include <gfx.math/geometry.hpp>
#include <gsl/span>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gfx {
inline namespace v1 {
class mesh_allocator;
template<typename Ii>
class prototype_instantiator;
struct mesh;
struct prototype;

using shared_prototype = std::shared_ptr<prototype>;
using weak_prototype   = std::weak_ptr<prototype>;
using shared_mesh      = std::shared_ptr<mesh>;
using weak_mesh        = std::weak_ptr<mesh>;

struct prototype
{
    template<typename Ii>
    friend class prototype_instantiator;
    constexpr static size_t max_submeshes = 8;
    struct submesh
    {
        shared_mesh base;
        transform   relative_transform;
    };

    std::string                                       name;
    std::array<std::optional<submesh>, max_submeshes> meshes {std::nullopt};

private:
    uint32_t _first_draw_command = 0;
    uint32_t _draw_command_count = 0;
};

struct mesh
{
    friend class mesh_deleter;
    friend class mesh_allocator;
    template<typename Ii>
    friend class prototype_instantiator;

    // private:
    u32      _index_count    = 0;
    u32      _vertex_count   = 0;
    u32      _bvh_node_count = 0;
    u32      _base_index     = 0;
    u32      _base_vertex    = 0;
    u32      _base_bvh_node  = 0;
    bounds3f _bounds;
};

enum class mesh_allocator_flag
{
    use_bvh = 1 << 0,
};
using mesh_allocator_flags = flags<mesh_allocator_flag>;

class mesh_allocator
{
public:
    template<typename InstanceInfo>
    friend class prototype_instantiator;

    class proxy
    {
    public:
        virtual ~proxy()                                                                                           = default;
        virtual std::tuple<gsl::span<vertex3d>, gsl::span<index32>, gsl::span<bvh<3>::node>> data() const noexcept = 0;
        virtual void resize_stages(ptrdiff_t delta_vertices, ptrdiff_t delta_indices, ptrdiff_t delta_bvh)         = 0;
        virtual void reserve_stages(ptrdiff_t delta_vertices, ptrdiff_t delta_indices, ptrdiff_t delta_bvh)        = 0;
        virtual void update_buffers(bool vertices, bool indices, bool bvh)                                         = 0;
    };

    mesh_allocator(proxy* p, mesh_allocator_flags flags = {});

    shared_mesh allocate_mesh(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                              std::optional<bounds3f> bounds = std::nullopt, bool manual_commit = false);
    shared_mesh allocate_mesh(const mesh3d& mesh, const submesh3d& submesh, bool manual_commit = false);

    void commit() const { _proxy->update_buffers(true, true, _flags.has(mesh_allocator_flag::use_bvh)); }

    void reserve_for(size_t vertices, size_t indices) const;

protected:
    shared_mesh allocate_mesh_impl(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                   std::optional<bounds3f> bounds = std::nullopt, bool manual_commit = false);
    shared_mesh allocate_mesh_impl(const mesh3d& mesh, const submesh3d& submesh, bool manual_commit = false);
    void        free_mesh_impl(const mesh* m);

private:
    proxy*                 _proxy;
    mesh_allocator_flags   _flags;
    std::vector<weak_mesh> _meshes;
    bvh<3>                 _bvh_generator;
};

enum class prototype_handle : u64
{
};

template<typename T, typename = void>
struct resize_multiple_of_16
{
    using value_type                = T;
    static constexpr auto type_size = sizeof(T);

    value_type value;

private:
    std::byte _p[(16 - (type_size % 16))] {};
};

template<typename T>
struct resize_multiple_of_16<T, std::enable_if_t<sizeof(T) % 16 == 0>>
{
    using value_type                = T;
    static constexpr auto type_size = sizeof(T);

    value_type value;
};

template<typename InstanceInfo>
class prototype_instantiator
{
public:
    using instance_info_type = InstanceInfo;

    struct basic_instance
    {
        u32                                       index_count    = 0;
        u32                                       instance_count = 0;
        u32                                       base_index     = 0;
        i32                                       base_vertex    = 0;
        u32                                       base_instance  = 0;
        u32                                       base_bvh_node  = 0;
        u32                                       vertex_count   = 0;
        u32                                       bvh_node_count = 0;
        glm::mat4                                 transform {};
        resize_multiple_of_16<instance_info_type> info;
    };

    class proxy
    {
    public:
        using range_type                                                                             = std::pair<size_t, size_t>;
        virtual ~proxy()                                                                             = default;
        virtual void                                             free_range(const range_type& range) = 0;
        virtual std::pair<range_type, gsl::span<basic_instance>> allocate_range(size_t count)        = 0;
        virtual void                                             clear()                             = 0;
    };

    prototype_instantiator(mesh_allocator& alloc, proxy* p);

    shared_prototype allocate_prototype(std::string name, const mesh3d& m);
    shared_prototype allocate_prototype(std::string name, const gsl::span<shared_mesh const>& meshes);
    shared_prototype prototype_by_name(const std::string& name);

    prototype_handle enqueue(const prototype& p, const transform& t, gsl::span<instance_info_type> properties);
    prototype_handle enqueue(const shared_prototype& p, const transform& t, gsl::span<instance_info_type> properties);
    void             dequeue(prototype_handle handle);
    void             clear();

    proxy*                                                 get_proxy() const noexcept { return _proxy; }
    const std::unordered_map<std::string, weak_prototype>& prototypes() const noexcept { return _prototypes; }

protected:
    shared_prototype allocate_prototype_impl(std::string name, std::initializer_list<shared_mesh> meshes);
    shared_prototype allocate_prototype_impl(std::string name, const gsl::span<shared_mesh const>& meshes);
    shared_prototype allocate_prototype_impl(std::string name, const mesh3d& m);
    void             free_prototype_impl(const prototype* proto);

private:
    proxy*                                                          _proxy;
    mesh_allocator*                                                 _alloc;
    std::unordered_map<std::string, weak_prototype>                 _prototypes;
    std::unordered_map<prototype_handle, std::pair<size_t, size_t>> _enqueued_ranges;
};

template<typename InstanceInfo>
class combined_proxy_view
{
public:
    using proto_t = typename prototype_instantiator<InstanceInfo>::proxy;
    using mesh_t  = mesh_allocator::proxy;

    combined_proxy_view(proto_t& p, mesh_t& m) : proto_proxy(&p), mesh_proxy(&m) {}

    template<typename Combo,
             typename = std::enable_if_t<std::conjunction_v<std::is_base_of<proto_t, Combo>, std::is_base_of<mesh_t, Combo>>>>
    combined_proxy_view(Combo& c) : proto_proxy(&c), mesh_proxy(&c)
    {}

    proto_t* proto_proxy;
    mesh_t*  mesh_proxy;
};

template<typename Info>
struct instance_component : ecs::component<instance_component<Info>>
{
    instance_component() = default;
    instance_component(shared_prototype proto, Info info) : handle(std::move(proto)), info(std::move(info)) {}

    shared_prototype handle   = nullptr;
    prototype_handle instance = {};
    Info             info;
};

template<typename Info>
class instance_system : public ecs::system
{
public:
    using instance_type = instance_component<Info>;

    explicit instance_system(combined_proxy_view<Info> p, mesh_allocator_flags flags = {});

    void pre_update() override;
    void update(duration_type delta, ecs::component_base** components) const override;

    const prototype_instantiator<Info>& get_instantiator() const noexcept;
    const mesh_allocator&               get_mesh_allocator() const noexcept;
    prototype_instantiator<Info>&       get_instantiator() noexcept;
    mesh_allocator&                     get_mesh_allocator() noexcept;

private:
    mesh_allocator                       _alloc;
    mutable prototype_instantiator<Info> _instantiator;
};
}    // namespace v1
}    // namespace gfx

#include "prototype.inl"