#pragma once

#include <array>
#include <gfx.core/flags.hpp>
#include <gfx.ecs/ecs.hpp>
#include <gfx.graphics/graphics.hpp>
#include <gfx.math/datastructure/bvh.hpp>
#include <gfx.math/geometry.hpp>
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

struct prototype
{
    template<typename Ii>
    friend class prototype_instantiator;
    constexpr static size_t max_submeshes = 8;
    struct submesh
    {
        mesh*     base;
        transform relative_transform;
    };

    std::string                                       name;
    std::array<std::optional<submesh>, max_submeshes> meshes{std::nullopt};

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

//private:
    u32      _index_count    = 0;
    u32      _vertex_count   = 0;
    u32      _bvh_node_count = 0;
    u32      _base_index     = 0;
    u32      _base_vertex    = 0;
    u32      _base_bvh_node  = 0;
    bounds3f _bounds;
};

using unique_prototype = std::unique_ptr<prototype, std::function<void(prototype* m)>>;
using unique_mesh      = std::unique_ptr<mesh, std::function<void(mesh* m)>>;
using shared_prototype = std::shared_ptr<prototype>;
using shared_mesh      = std::shared_ptr<mesh>;

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

    mesh_allocator(device& device, mesh_allocator_flags flags = {});

    mesh* allocate_mesh(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                        std::optional<bounds3f> bounds = std::nullopt);
    mesh* allocate_mesh(const mesh3d& mesh, const submesh3d& submesh);
    void  free_mesh(const mesh* m);

    unique_mesh allocate_mesh_unique(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                     std::optional<bounds3f> bounds = std::nullopt);
    unique_mesh allocate_mesh_unique(const mesh3d& mesh, const submesh3d& submesh);
    shared_mesh allocate_mesh_shared(const gsl::span<const vertex3d>& vertices, const gsl::span<const index32>& indices,
                                     std::optional<bounds3f> bounds = std::nullopt);
    shared_mesh allocate_mesh_shared(const mesh3d& mesh, const submesh3d& submesh);

    const device&               get_device() const;
    const buffer<vertex3d>&     vertex_buffer() const noexcept;
    const buffer<index32>&      index_buffer() const noexcept;
    const buffer<bvh<3>::node>& bvh_buffer() const noexcept;

    gsl::span<mesh* const> meshes() const noexcept;

private:
    device*                                                     _device;
    mesh_allocator_flags                                        _flags;
    std::unordered_map<std::string, std::unique_ptr<prototype>> _prototypes;
    std::vector<std::unique_ptr<mesh>>                          _meshes;

    mapped<vertex3d>     _staging_vertex_buffer;
    mapped<index32>      _staging_index_buffer;
    mapped<bvh<3>::node> _staging_bvh_buffer;

    std::unique_ptr<buffer<vertex3d>>     _vertex_buffer;
    std::unique_ptr<buffer<index32>>      _index_buffer;
    std::unique_ptr<buffer<bvh<3>::node>> _bvh_buffer;

    bvh<3> _bvh_generator;
};

enum class prototype_handle : u64
{
};

template<typename InstanceInfo>
class prototype_instantiator
{
public:
    using instance_info_type = InstanceInfo;

    struct basic_instance
    {
        u32                index_count    = 0;
        u32                instance_count = 0;
        u32                base_index     = 0;
        i32                base_vertex    = 0;
        u32                base_instance  = 0;
        u32                base_bvh_node  = 0;
        u32                vertex_count   = 0;
        u32                bvh_node_count = 0;
        glm::mat4          transform{};
        instance_info_type info;
    };

    prototype_instantiator(mesh_allocator& alloc);

    prototype* allocate_prototype(std::string name, std::initializer_list<mesh*> meshes);
    prototype* allocate_prototype(std::string name, const gsl::span<mesh* const>& meshes);
    prototype* allocate_prototype(std::string name, const mesh3d& m);

    void free_prototype(const prototype* proto);

    unique_prototype allocate_prototype_unique(std::string name, const mesh3d& m);
    unique_prototype allocate_prototype_unique(std::string name, const gsl::span<mesh*>& meshes);
    shared_prototype allocate_prototype_shared(std::string name, const mesh3d& m);
    shared_prototype allocate_prototype_shared(std::string name, const gsl::span<mesh*>& meshes);

    prototype* prototype_by_name(const std::string& name);

    prototype_handle enqueue(prototype* p, const transform& t, gsl::span<instance_info_type> properties);
    void             dequeue(prototype_handle handle);

    void                          clear();
    const mapped<basic_instance>& instances() const noexcept;

private:
    mesh_allocator*                                                 _alloc;
    std::unordered_map<std::string, std::unique_ptr<prototype>>     _prototypes;
    std::unordered_map<prototype_handle, std::pair<size_t, size_t>> _enqueued_ranges;
    mapped<basic_instance>                                          _instance_descriptions;
};

template<typename Info>
struct instance_component : ecs::component<instance_component<Info>>
{
    instance_component() = default;
    instance_component(prototype* hnd, Info info) : handle(hnd), info(std::move(info)) {}

    prototype*       handle   = nullptr;
    prototype_handle instance = {};
    Info             info;
};

template<typename Info>
class instance_system : public ecs::system
{
public:
    using instance_type = instance_component<Info>;

    explicit instance_system(device& device, mesh_allocator_flags flags = {});

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