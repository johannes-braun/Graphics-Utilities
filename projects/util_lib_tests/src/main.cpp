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
        bundle.write(&mat[0][0], LengthR * LengthC);
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

        std::string str() const { return _state_code.str(); }
        const bundle<float>& get_bundle() const noexcept { return _data; }

    private:
        int _size;
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
        std::stringstream _state_code;
        bundle<float> _data;
    };

    uint32_t hash32(const std::string& str)
    {
        const auto h = std::hash<std::string>()(str);
        return uint32_t((h & 0xffffffff) ^ ((h >> 32) & 0xffffffff));
    }

    struct parameter
    {
        std::string name;
        size_t length = 0;
        size_t offset = 0;
    };

    struct named_code
    {
    public:
        named_code(const std::string& name, const std::string& code, int float_count = 0)
            : _hash(hash32(name)), _name(name), _code(code), _float_count(float_count) {}

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
            std::initializer_list<size_t> x ={ ts.param.length ... };
            std::initializer_list<size_t> o ={ ts.param.offset ... };
            const size_t offset = o.size()==0?0:std::data(o)[0];
            instance._size = int(std::accumulate(x.begin(), x.end(), 0ull));
            instance._state_code << "sdf_primitive" << "(" << hash() << ", "<<offset<<", sdf_position)";
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
            std::initializer_list<size_t> x ={ ts.param.length ... };
            std::initializer_list<size_t> o ={ ts.param.offset ... };
            const size_t offset = o.size()==0?0:std::data(o)[0];
            instance._size = int(std::accumulate(x.begin(), x.end(), 0ull));
            instance._state_code << "sdf_modifier(" << hash() <<", "<< offset<<", sdf_position)";
            return instance;
        }

        template<typename... Ts>
        position_instance operator()(const position_instance& mod, const Ts& ... ts) const
        {
            position_instance instance;
            std::initializer_list<size_t> x ={ ts.param.length ... };
            std::initializer_list<size_t> o ={ ts.param.offset ... };
            const size_t offset = o.size()==0?0:std::data(o)[0];
            instance._size = int(std::accumulate(x.begin(), x.end(), 0ull));
            instance._state_code << "sdf_modifier(" << hash() <<", "<< offset <<", " << mod._state_code.str() << ")";
            return instance;
        }
    };


    template<typename... Ts>
    distance_instance primitive::operator()(const position_instance& mod, const Ts& ... ts) const
    {
        distance_instance instance;
        instance._data.append(mod._data);
        std::initializer_list<size_t> x ={ ts.param.length ... };
        std::initializer_list<size_t> o ={ ts.param.offset ... };
        const size_t offset = o.size()==0?0:std::data(o)[0];
        instance._size = int(std::accumulate(x.begin(), x.end(), 0ull));
        instance._state_code << "sdf_primitive(" << hash() << ", "<< offset << ", " << mod._state_code.str() <<")"; // TODO: parameters.
        return instance;
    }

    template<typename... Ts>
    distance_instance op::operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const
    {
        distance_instance instance;
        instance._data.append(lhs._data);
        instance._data.append(rhs._data);
        std::initializer_list<size_t> x ={ ts.param.length ... };
        std::initializer_list<size_t> o ={ ts.param.offset ... };
        const size_t offset = o.size()==0?0:std::data(o)[0];
        instance._size = int(std::accumulate(x.begin(), x.end(), 0ull));
        instance._state_code << "sdf_operator(" << hash() <<", " << offset << ", " << lhs._state_code.str() << ", " << rhs._state_code.str()  << ")";
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


    template<typename T>
    struct placeholder
    {
        explicit placeholder(parameter param)
            : param(param)
        {
        }

        parameter param;
    };

    class group_instance;
    class group : public named_code, public std::enable_shared_from_this<group>
    {
    public:
        group(const std::string& name) : named_code(name, "", 0) {}

        template<typename T>
        placeholder<T> param(const std::string& name)
        {
            if (const auto it = std::find_if(parameters().begin(), parameters().end(), [&name](const parameter& pm) { return pm.name == name; });
                it != parameters().end())
            {
                assert(it->length == sizeof(T)/sizeof(float) && "Parameter existing with different size.");
                return placeholder<T>(*it);
            }
            _parameters.push_back({ name, sizeof(T)/sizeof(float), _parameters.empty() ? 0 : (_parameters[_parameters.size()-1].offset + _parameters[_parameters.size()-1].length) });
            return placeholder<T>(_parameters.back());
        }

        void assemble(const distance_instance& instance)
        {
            _code = instance.str();
            _float_count = static_cast<int>(instance.get_bundle().stream_data().length());
        }

        const std::vector<parameter>& parameters() const noexcept { return _parameters; }
        std::unique_ptr<group_instance> instantiate() const;

    private:
        std::vector<parameter> _parameters;
    };

    class group_instance
    {
    public:
        group_instance(const std::shared_ptr<const group>& g) : _group(g)
        {
            _param_data.resize(_group->parameters().back().offset + _group->parameters().back().length);
        }

        template<typename T>
        void set(const std::string& pname, T&& value)
        {
            if (const auto it = std::find_if(_group->parameters().begin(), _group->parameters().end(), [&pname](const parameter& pm) { return pm.name == pname; });
            it != _group->parameters().end())
            {
                const parameter& p = *it;
                assert(sizeof(T)/sizeof(float) >= p.length && "Non-fitting parameter length.");
                memcpy(_param_data.data() + p.offset, &value, sizeof(T));
                return;
            }
            assert(false && "Parameter not found.");
        }

        const group& group_class() const { return *_group; }
        uint32_t param_count() const { return uint32_t(_param_data.size()); }
        const std::vector<float>& param_data() const { return _param_data; }

    private:
        std::shared_ptr<const group> _group;
        std::vector<float> _param_data;
    };

    std::unique_ptr<group_instance> group::instantiate() const
    {
        // shared_from_this may only return a valid ptr, if there exist any other shared_ptr of this object.
        // If invalid, it will probably be managed in another way.
        if (!shared_from_this()) return std::make_unique<group_instance>(std::shared_ptr<const group>(this, [](auto) {}));
        return std::make_unique<group_instance>(shared_from_this());
    }

    class host
    {
    public:
        host() {
            _op_stream << op_header;
            _prim_stream << prim_header;
            _mod_stream << mod_header;
            _class_stream << class_header;
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
            _mod_stream << "case " << m.hash() << " /* " << m.name() << " */ : { return " << m.code() << ";}\n";
            ++_mod_count;
            _hashes.emplace(m.hash());
        }

        void enable_group(const group& g)
        {
            _class_stream << "case " << g.hash() << " /* " << g.name() << " */ : {" << g.code() << ";}\n";
        }

        std::string assemble() const {
            return _op_stream.str() + end + _prim_stream.str() + end + _mod_stream.str() + end + _class_stream.str() + end;
        }

        int operators() const noexcept { return _op_count; }
        int primitives() const noexcept { return _prim_count; }
        int modifiers() const noexcept { return _mod_count; }

        struct draw_cmd
        {
            uint32_t group_hash;
            uint32_t param_count;
            uint32_t param_offset;
            uint32_t ihavenoidea;
        };

        void enqueue(const group_instance& instance)
        {
            draw_cmd cmd;
            cmd.group_hash = instance.group_class().hash();
            cmd.param_count = instance.param_count();
            cmd.param_offset = _draw_commands.empty() ? 0 : (_draw_commands[_draw_commands.size()-1].param_offset + _draw_commands[_draw_commands.size()-1].param_count);
            _draw_commands.push_back(cmd);

            _draw_data.insert(_draw_data.end(), instance.param_data().begin(), instance.param_data().end());
        }

    private:
        std::vector<draw_cmd> _draw_commands;
        std::vector<float> _draw_data;

        constexpr static const char* op_header = "float sdf_operator(uint id, uint sdf_instance_id, float sdf_op_lhs, float sdf_op_rhs) {\nswitch(id) {\n default: return 0;\n";
        int _op_count = 0;
        std::stringstream _op_stream;

        constexpr static const char* prim_header = "float sdf_primitive(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n default: return 0;\n";
        int _prim_count = 0;
        std::stringstream _prim_stream;

        constexpr static const char* mod_header = "vec3 sdf_modifier(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n default: return vec3(0);\n";
        int _mod_count = 0;
        std::stringstream _mod_stream;

        constexpr static const char* class_header = "float sdf_class(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n default: return 0;\n";
        int _class_count = 0;
        std::stringstream _class_stream;

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
    struct creator<op, Params...> : detail::creator_base<op, placeholder<Params>...>
    {
        creator(const std::string& name, const std::string& code) : detail::creator_base<op, placeholder<Params>...>(name, code) {}

        typename op::return_type make(placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<op, placeholder<Params>...>::object()(params...);
        }

        typename op::return_type make(const distance_instance& lhs, const distance_instance& rhs, placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<op, placeholder<Params>...>::object()(lhs, rhs, params...);
        }

        typename op::return_type operator()(placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<op, placeholder<Params>...>::object()(params...);
        }

        typename op::return_type operator()(const distance_instance& lhs, const distance_instance& rhs, placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<op, placeholder<Params>...>::object()(lhs, rhs, params...);
        }
    };

    template<typename... Params>
    struct creator<primitive, Params...> : detail::creator_base<primitive, placeholder<Params>...>
    {
        creator(const std::string& name, const std::string& code) : detail::creator_base<primitive, placeholder<Params>...>(name, code) {}

        typename primitive::return_type make(placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<primitive, placeholder<Params>...>::object()(params...);
        }

        typename primitive::return_type make(const position_instance& pos, placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<primitive, placeholder<Params>...>::object()(pos, params...);
        }

        typename primitive::return_type operator()(placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<primitive, placeholder<Params>...>::object()(params...);
        }

        typename primitive::return_type operator()(const position_instance& pos, placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<primitive, placeholder<Params>...>::object()(pos, params...);
        }
    };

    template<typename... Params>
    struct creator<modifier, Params...> : detail::creator_base<modifier, placeholder<Params>...>
    {
        creator(const std::string& name, const std::string& code) : detail::creator_base<modifier, placeholder<Params>...>(name, code) {}

        typename modifier::return_type make(placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<modifier, placeholder<Params>...>::object()(params...);
        }

        typename modifier::return_type make(const position_instance& pos, placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<modifier, placeholder<Params>...>::object()(pos, params...);
        }

        typename modifier::return_type operator()(placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<modifier, placeholder<Params>...>::object()(params...);
        }

        typename modifier::return_type operator()(const position_instance& pos, placeholder<Params>&&... params) const noexcept
        {
            return detail::creator_base<modifier, placeholder<Params>...>::object()(pos, params...);
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
    const static creator<sdf::modifier, glm::vec3> md_repeat{ "repeat", R"(
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
uint _sdf_index = 0;
uint sdf_index_reset(uint val) { return (_sdf_index = val); }
uint sdf_index_offset(uint size) { 
    const uint id = _sdf_index;
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

namespace sdf2
{
    struct distance_function
    {
        distance_function(const std::string& name, const std::string& code, int floats) {}
    };
    struct position_function
    {
        position_function(const std::string& name, const std::string& code, int floats) {}
    };

    /* Random thought.
    class sphere : public sdf::primitive
    {
        // maybe in shader? compute shader?
        bounds get_bounds() const noexcept {
            bounds b;
            b.min = vec3(-_rad.as_float());
            b.max = vec3(_rad.as_float());
            return b;
        }

        sdf::primitive_instance instantiate() {
            return new_instance();
        }

    private:
        param _rad{1}; // parameter with 1 float value
    }



    base:
    sdf function class
     - name (hash for switch or name for function? default functions (simple obj, like sphere, box, etc)?)
     - main function code
     - param float count (evtlly int?)
     - param proxy?
     - "lazy" bounds computation from instantiated proxy parameters?
           -> if so, must happen on draw command creation time.

    instances:
    all:
     - parameter buffer (vector)
     - class ptr for back-reference of parameter sizes (& AABB computation?)

    op
     - dist lhs & dist rhs -> dist

    prim
     - pos -> dist

    mod
     - pos -> pos


    lifetime:
    -- on  init --
        1) base(name, fun, param("pos", 3), param("radius", 1), param("scale", 3))
        --- here is a good point to finalize shader compilation.
        2) instance = base.instantiate();
        --- some step to combine base objects, maybe later instance.left = instance2 or sth.
        3) instance.set("pos", glm::value_ptr(pos)); ... // float*?
    -- per frame --
        4) host.enqueue_draw(instance)
            a) cmd.hash = instance.base.hash
            b) cmd.param_offset = last_offset + last_size;
            c) cmd.param_size = instance.param_count
            d) cmd.bounds_index = (all_bounds.emplace_back(instance.compute_bounds()), all_bounds.size()-1);
            e) cmd_vec.push_back(cmd);
        5) host.draw()
            a) cmd_buffer.assign(cmd_vec) // make bigger if necessary.
            b) aabb_buffer.assign(all_bounds) // same
            c) (build bvh/kd?)
            d) render()

    json:
        1) base objects
            "functions" : [ { 
                "name":"sphere",    
                "type":"primitive",
                "function":"return length(sdf_position)-sdf_get_vec3(sdf_id, 0)", 
                "fparams" : [
                    { "name":"pos",     "length":3 },
                    { "name":"radius",  "length":1 },
                    { "name":"scale",   "length":3 }
                ]
            }, ... ]

        2) instances
            "instances" : [ { "function":"sphere", "input0":"sdf_default", "pos":[3.0, 1.0, 6.0], "radius":[0.2], "scale":[0.4, 0.6, 0.2] } ]

        3) misc stuff (camera, lights? maybe not even needed because reasons)

    input0 and input1 are other sub-instances, or sdf_default (only input0 and only if function type is primitive or modifier)
     */

    struct op : distance_function
    {
        // op = distance & distance -> distance
        std::shared_ptr<distance_function> left;
        std::shared_ptr<distance_function> right;
        
        // Missing parameters, but might be used like op->apply(prim1, prim2)
        void apply(const std::shared_ptr<distance_function>& left, const std::shared_ptr<distance_function>& right) {}
    };

    struct primitive : distance_function
    {
        // primitive = position -> distance
        std::shared_ptr<position_function> position;
    };

    struct modifier : position_function
    {
        // modifier = position -> position
        std::shared_ptr<position_function> position;
    };
}

struct par
{
    par(const std::string& name, const size_t length) : _name(name), _length(length) {}
    const std::string& name() const noexcept { return _name; }
    size_t length() const noexcept { return _length; }

private:
    std::string _name;
    size_t _length;
};

int main()
{
    gl::shader::set_include_directories({ "../shd", SOURCE_DIRECTORY "/global/shd" });

    sdf::host host;
    // Add basic operators
    host.enable(sdf::op_union);
    host.enable(sdf::op_subtract);
    host.enable(sdf::op_intersect);
    // Add basic primitives
    host.enable(sdf::pr_box);
    host.enable(sdf::pr_sphere);
    host.enable(sdf::pr_torus);
    // Add basic modifiers
    host.enable(sdf::md_repeat);
    host.enable(sdf::md_transform);

    // Create a group object consisting of multiple primitives/operators/modifiers and parameters.
    const auto random_object= std::make_shared<sdf::group>("random_object");
    const auto transform    = sdf::md_transform(random_object->param<glm::mat4>("inverse_transform"));
    const auto rep_box      = sdf::pr_box(sdf::md_repeat(transform, random_object->param<glm::vec3>("rep_01_count")), random_object->param<glm::vec3>("box_01_dimen"));
    const auto rep_sphere   = sdf::pr_sphere(sdf::md_repeat(transform, random_object->param<glm::vec3>("rep_01_count")), random_object->param<float>("sphere_01_radius"));
    const auto box          = sdf::pr_box(transform, random_object->param<glm::vec3>("box_02_dimen"));
    const auto object       = sdf::op_union(sdf::op_union(rep_box, box), rep_sphere);
    // Idea for AABB: make dependant on parameters such that the class is more modular. Compute shader AABB computation for all classes?
    random_object->assemble(object);
    host.enable_group(*random_object);

    const auto sphere_object= std::make_shared<sdf::group>("Sphere");
    sphere_object->assemble(sdf::pr_sphere(sphere_object->param<float>("radius")));
    host.enable_group(*sphere_object);

    // Assemble the shader
    const auto opfun = host.assemble();

    // ... COMPILE SHADER ETC ...

    gfx::transform tf;
    tf.position ={ 0, 0, 4 };

    // instantiate an object and set necessary parameters.
    std::unique_ptr<sdf::group_instance> instance = random_object->instantiate();
    instance->set("inverse_transform", inverse(glm::mat4(tf)));
    instance->set("rep_01_count", glm::vec3(3, 3, 3));
    instance->set("box_01_dimen", glm::vec3(1, 2, 1));
    instance->set("sphere_01_radius", 0.4f);
    // JSON: "params" : { "name":"box_02_dimen", "type":"vec3", "value":"0.2, 0.4, 0.32" } 
    instance->set("box_02_dimen", glm::vec3(0.2f, 0.4f, 0.32f));

    // enqueue instance(s) for later drawing.
    // data is being copied, so all changes after that will only be shown after the draw call.
    host.enqueue(*instance);
    // move object somewhere else, rotate it and draw again.
    tf.position ={ 0, 3, 1 };
    tf.rotation = glm::angleAxis(glm::radians(43.f), glm::vec3(1, 0, 0));
    instance->set("inverse_transform", inverse(glm::mat4(tf)));
    host.enqueue(*instance);

    auto sphere = sphere_object->instantiate();
    sphere->set("radius", 0.2f);
    host.enqueue(*sphere);





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
        ImGui::Image(uint32_t(mygl::texture(tex)), { 512, 512 });
        imgui.render();
    }

    return 0;
}

