#include <gfx/window.hpp>
#include <gfx/log.hpp>
#include <gfx/imgui.hpp>

// float operator  (dist1(p), dist2(p), ...)      // SDF operator -> applied to two distances
// float primitive (p, ...)                       // SDF function -> calculates the distance of p to the field.
// vec3  modifier  (vec3, ...)                    // SDF operator -> modifies the point.

#include <sstream>
#include <numeric>

namespace sdf
{
    struct root_position;
    struct op;
    struct primitive;
    struct modifier;

    template<typename T>
    struct bundle;

    template<typename T, typename = decltype(std::declval<T>().serialize(std::declval<bundle<float>>()))>
    int serialize(const T& value, bundle<float>& bundle)
    {
        return value.serialize(bundle);
    }

    template<typename T>
    struct bundle
    {
        int write(const T* t, int count)
        {
            _stream.write(t, count);
            return count;
        }

        void append(const bundle& b) { _stream << b._stream.str(); }

        std::basic_string<float> stream_data() const { return _stream.str(); }

    private:
        std::basic_stringstream<float> _stream;
    };

    template<glm::length_t Length, glm::qualifier Precision>
    int serialize(const glm::vec<Length, float, Precision>& vec, bundle<float>& bundle)
    {
        bundle.write(&vec[0], Length);
        return Length;
    }

    template<glm::length_t LengthC, glm::length_t LengthR, glm::qualifier Precision>
    int serialize(const glm::mat<LengthC, LengthR, float, Precision>& mat, bundle<float>& bundle)
    {
        bundle.write(&mat[0], LengthR * LengthC);
        return LengthR * LengthC;
    }

    int serialize(float f, bundle<float>& bundle)
    {
        bundle.write(&f, 1);
        return 1;
    }

    struct distance_instance
    {
        friend primitive;
        friend op;

        std::string str() const { return _state_code.str() + ")"; }
        const bundle<float>& get_bundle() const noexcept { return _data; }

    private:
        int _size;
        int _offset;
        std::stringstream _state_code;
        bundle<float> _data;
    };

    struct position_instance
    {
        friend modifier;
        friend primitive;
        std::string str() const { return _state_code.str() + ")"; }
        const bundle<float>& get_bundle() const noexcept { return _data; }

    private:
        int _size;
        int _offset;
        std::stringstream _state_code;
        bundle<float> _data;
    };

    struct named_code
    {
    public:
        named_code(const std::string& name, const std::string& code, int float_count = 0)
            : _hash([&name]() {
            const auto h = std::hash<std::string>()(name);
            return uint32_t((h & 0xffffffff) ^ ((h >> 32) & 0xffffffff));
        }()), _name(name), _code(code), _float_count(float_count) {}

        const std::string& name() const noexcept { return _name; }
        const std::string& code() const noexcept { return _code; }
        uint32_t           hash() const noexcept { return _hash; }
        int         float_count() const noexcept { return _float_count; }

    protected:
        uint32_t _hash;
        std::string _name;
        std::string _code;
        int _float_count;
    };

    struct op : named_code
    {
        using return_type = distance_instance;

        op(const std::string& name, const std::string& code, int float_count = 0) : named_code(name, code, float_count) {}

        template<typename... Ts>
        distance_instance operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const;
    };

    struct primitive : named_code
    {
        friend op;
        friend distance_instance;
        using return_type = distance_instance;

        primitive(const std::string& name, const std::string& code, int float_count = 0) : named_code(name, code, float_count) {}

        template<typename... Ts>
        distance_instance operator()(const position_instance& mod, const Ts& ... ts) const;

        template<typename... Ts>
        distance_instance operator()(const Ts& ... ts) const
        {
            distance_instance instance;
            std::vector<int> x ={ serialize(ts, instance._data)... };
            instance._size = std::accumulate(x.begin(), x.end(), 0);
            instance._offset = 0;
            instance._state_code << _name << "(sdf_position"; // TODO: parameters.
            return instance;
        }
    };

    struct modifier : named_code
    {
        using return_type = position_instance;

        modifier(const std::string& name, const std::string& code, int float_count = 0) : named_code(name, code, float_count) {}

        template<typename... Ts>
        position_instance operator()(const Ts& ... ts) const
        {
            position_instance instance;
            std::vector<int> x ={ serialize(ts, instance._data)... };
            instance._size = std::accumulate(x.begin(), x.end(), 0);
            instance._offset = 0;
            instance._state_code << _name << "(sdf_position";
            return instance;
        }
    };


    template<typename... Ts>
    distance_instance primitive::operator()(const position_instance& mod, const Ts& ... ts) const
    {
        distance_instance instance;
        instance._data.append(mod._data);
        std::vector<int> x ={ serialize(ts, instance._data)... };
        instance._size = std::accumulate(x.begin(), x.end(), 0);
        instance._offset = mod._offset + mod._size;
        instance._state_code << _name << "(" << mod._state_code.str() << ", sdf_index_offset(" << mod._size << "))"; // TODO: parameters.
        return instance;
    }

    template<typename... Ts>
    distance_instance op::operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const
    {
        distance_instance instance;
        instance._data.append(lhs._data);
        instance._data.append(rhs._data);
        std::vector<int> x ={ serialize(ts, instance._data)... };
        instance._state_code << _name << "(" << lhs._state_code.str() << ", sdf_index_offset(" << lhs._size << ")), " << rhs._state_code.str()  << ", sdf_index_offset(" << rhs._size << "))";
        instance._size = std::accumulate(x.begin(), x.end(), 0);
        instance._offset = lhs._offset +lhs._size + rhs._size + std::accumulate(x.begin(), x.end(), 0);
        return instance;
    }

    std::string build_function(const sdf::op& code)
    {
        std::stringstream str;
        str << "float " << code.name() << "(float sdf_op_lhs, float sdf_op_rhs, int sdf_instance_id) {\n"
            << code.code() << ";\n}\n";
        return str.str();
    }

    std::string build_function(const sdf::primitive& code)
    {
        std::stringstream str;
        str << "float " << code.name() << "(vec3 sdf_position, int sdf_instance_id) {\n"
            << code.code() << ";\n}\n";
        return str.str();
    }

    std::string build_function(const sdf::modifier& code)
    {
        std::stringstream str;
        str << "vec3 " << code.name() << "(vec3 sdf_position, int sdf_instance_id) {\n"
            << code.code() << ";\n}\n";
        return str.str();
    }

    class host
    {
    public:
        host() {
            _op_stream << op_header;
            _prim_stream << prim_header;
            _mod_stream << mod_header;
        }

        void enable(const op& o) {
            if (_hashes.count(o.hash()) != 0)
            {
                log_w << "Operator with name " << o.hash() << " already enabled.";
                return;
            }
            _op_stream << "case " << o.hash() << " /* " << o.name() << " */ : {" << o.code() << ";}\n";
            ++_op_count;
            _hashes.emplace(o.hash());
        }
        void enable(const primitive& p)
        {
            if (_hashes.count(p.hash()) != 0)
            {
                log_w << "Primitive with name " << p.name() << " already enabled.";
                return;
            }
            _prim_stream << "case " << p.hash() << " /* " << p.name() << " */ : {" << p.code() << ";}\n";
            ++_prim_count;
            _hashes.emplace(p.hash());
        }

        void enable(const modifier& m)
        {
            if (_hashes.count(m.hash()) != 0)
            {
                log_w << "Modifier with name " << m.name() << " already enabled.";
                return;
            }
            _mod_stream << "case " << m.hash() << " /* " << m.name() << " */ : {" << m.code() << ";}\n";
            ++_mod_count;
            _hashes.emplace(m.hash());
        }

        std::string assemble() const {
            return _op_stream.str() + end + _prim_stream.str() + end + _mod_stream.str() + end;
        }

        int operators() const noexcept { return _op_count; }
        int primitives() const noexcept { return _prim_count; }
        int modifiers() const noexcept { return _mod_count; }

    private:
        constexpr static const char* op_header = "float sdf_operator(uint id, uint sdf_instance_id, float sdf_op_lhs, float sdf_op_rhs) {\nswitch(id) {\n default: return 0;\n";
        int _op_count = 0;
        std::stringstream _op_stream;

        constexpr static const char* prim_header = "float sdf_primitive(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n default: return 0;\n";
        int _prim_count = 0;
        std::stringstream _prim_stream;

        constexpr static const char* mod_header = "vec3 sdf_modifier(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n default: return vec3(0);\n";
        int _mod_count = 0;
        std::stringstream _mod_stream;

        std::set<uint32_t> _hashes;

        constexpr static const char* end = "}}\n\n";
    };

    namespace detail {
        template<typename T, typename... Params>
        struct creator_base
        {
            creator_base(const std::string& name, const std::string& code)
                : _obj(name, code, []() {
                std::initializer_list<size_t> list ={ sizeof(Params)... };
                return int(std::accumulate(list.begin(), list.end(), 0ull) / sizeof(float));
            }()) {
            }

            const T& object() const noexcept
            {
                return _obj;
            }

            operator const T&() const noexcept
            {
                return object();
            }

        protected:
            T _obj;
        };
    }

    template<typename T, typename... Params>
    struct creator;

    template<typename... Params>
    struct creator<op, Params...> : detail::creator_base<op, Params...>
    {
        creator(const std::string& name, const std::string& code) : detail::creator_base<op, Params...>(name, code) {}

        typename op::return_type make(Params&&... params) const noexcept
        {
            return detail::creator_base<op, Params...>::object()(params...);
        }

        typename op::return_type make(const distance_instance& lhs, const distance_instance& rhs, Params&&... params) const noexcept
        {
            return detail::creator_base<op, Params...>::object()(lhs, rhs, params...);
        }

        typename op::return_type operator()(Params&&... params) const noexcept
        {
            return detail::creator_base<op, Params...>::object()(params...);
        }

        typename op::return_type operator()(const distance_instance& lhs, const distance_instance& rhs, Params&&... params) const noexcept
        {
            return detail::creator_base<op, Params...>::object()(lhs, rhs, params...);
        }
    };

    template<typename... Params>
    struct creator<primitive, Params...> : detail::creator_base<primitive, Params...>
    {
        creator(const std::string& name, const std::string& code) : detail::creator_base<primitive, Params...>(name, code) {}

        typename primitive::return_type make(Params&&... params) const noexcept
        {
            return detail::creator_base<primitive, Params...>::object()(params...);
        }

        typename primitive::return_type make(const position_instance& pos, Params&&... params) const noexcept
        {
            return detail::creator_base<primitive, Params...>::object()(pos, params...);
        }

        typename primitive::return_type operator()(Params&&... params) const noexcept
        {
            return detail::creator_base<primitive, Params...>::object()(params...);
        }

        typename primitive::return_type operator()(const position_instance& pos, Params&&... params) const noexcept
        {
            return detail::creator_base<primitive, Params...>::object()(pos, params...);
        }
    };

    template<typename... Params>
    struct creator<modifier, Params...> : detail::creator_base<modifier, Params...>
    {
        creator(const std::string& name, const std::string& code) : detail::creator_base<modifier, Params...>(name, code) {}

        typename modifier::return_type make(Params&&... params) const noexcept
        {
            return detail::creator_base<modifier, Params...>::object()(params...);
        }

        typename modifier::return_type make(const position_instance& pos, Params&&... params) const noexcept
        {
            return detail::creator_base<modifier, Params...>::object()(pos, params...);
        }

        typename modifier::return_type operator()(Params&&... params) const noexcept
        {
            return detail::creator_base<modifier, Params...>::object()(params...);
        }

        typename modifier::return_type operator()(const position_instance& pos, Params&&... params) const noexcept
        {
            return detail::creator_base<modifier, Params...>::object()(pos, params...);
        }
    };

    /* SDF Box function.
    @param glm::vec3 dimensions. */
    const static creator<sdf::primitive, glm::vec3> pr_box{ "box", R"(
            vec3 size = sdf_get_vec3(sdf_instance_id, 0);
            return length(max(abs(sdf_position) - size, 0.0)); )" };

    /* SDF sphere function.
    @param float radius. */
    const static creator<sdf::primitive, float> pr_sphere{ "sphere", "return length(sdf_position) - sdf_get_float(sdf_instance_id, 0)" };

    /*SDF torus function.
    @param glm::vec2 inner (x) and outer (y) radii. */
    const static creator<sdf::primitive, glm::vec2> pr_torus{ "torus", R"(
            vec2 t = sdf_get_vec2(sdf_instance_id, 0);
            vec2 q = vec2(length(sdf_position.xz) - t.x, sdf_position.y);
            return length(sdf_position) - t.y; )" };

    /* SDF union operator. */
    const static creator<sdf::op> op_union{ "union", "return min(sdf_op_lhs, sdf_op_rhs)" };

    /* SDF subtract operator. */
    const static creator<sdf::op> op_subtract{ "subtract", "return max(-sdf_op_lhs, sdf_op_rhs)" };

    /* SDF intersect operator. */
    const static creator<sdf::op> op_intersect{ "intersect", "return max(sdf_op_lhs, sdf_op_rhs)" };

    /* SDF repeat modifier.
    @param glm::vec3 distances between repeated instances. */
    const static creator<sdf::modifier, glm::vec3> md_repeat{"repeat", R"(
            vec3 c = sdf_get_vec3(sdf_instance_id, 0);
            return mod(sdf_position, c) - 0.5 * c; )" };

    /* SDF transform modifier.
    @param glm::mat4 the *INVERSE* transformation matrix. */
    const static creator<sdf::modifier, glm::mat4> md_transform{ "transform", R"(
            mat4 invmat = sdf_get_mat4(sdf_instance_id, 0);
            return (invmat * vec4(p, 1)).xyz;
        )" };
}

constexpr const char* get_offset = R"(
int _sdf_index = 0;
int sdf_index_reset(int val) { return (_sdf_index = val); }
int sdf_index_offset(int size) { 
    const int id = _sdf_index;
    _sdf_index += size;
    return id;
}
)";

constexpr const char* sdf_prefix = R"(
float sdf_distance(vec3 sdf_position)
{
    sdf_index_reset(0);
    float distance = 0;
)";

int main()
{
    gl::shader::set_include_directories({ "../shd", SOURCE_DIRECTORY "/global/shd" });

    sdf::host host;
    host.enable(sdf::op_union);
    host.enable(sdf::op_subtract);
    host.enable(sdf::op_intersect);

    host.enable(sdf::pr_box);
    host.enable(sdf::pr_sphere);
    host.enable(sdf::pr_torus);

    host.enable(sdf::md_repeat);
    host.enable(sdf::md_transform);

    const auto opfun = host.assemble();

    const auto rep_box      = sdf::pr_box(sdf::md_repeat(glm::vec3(9, 2, 3)), glm::vec3(1, 4, 4));
    const auto rep_sphere   = sdf::pr_sphere(sdf::md_repeat(glm::vec3(9, 2, 3)), 0.3f);
    const auto box          = sdf::pr_box(glm::vec3(3, 2, 1));
    const auto object       = sdf::op_union(sdf::op_union(rep_box, box), rep_sphere);
    auto code = object.str();
    auto data = object.get_bundle().stream_data();
    std::vector<float> dvec(data.begin(), data.end());

    auto window = std::make_shared<gfx::window>("opengl", "Title", 1280, 720);
    window->set_icon(gfx::image_file("ui/logo.svg", 1.f));
    gfx::imgui imgui(window);

    std::vector<float> colors(512*512);
    gl::texture tex(GL_TEXTURE_2D, 512, 512, GL_R32F, 1);
    tex.assign(GL_RED, GL_FLOAT, colors.data());

    while (window->update())
    {
        imgui.new_frame();
        ImGui::Image(uint32_t(gl_texture_t(tex)), { 512, 512 });
        imgui.render();
    }

    return 0;
}
