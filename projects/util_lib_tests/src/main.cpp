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

    enum class distance : uint32_t {};
    enum class position : uint32_t {};

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
            : _hash(std::hash<std::string>()(name)), _name(name), _code(code), _float_count(float_count) {}

        const std::string& name() const noexcept { return _name; }
        const std::string& code() const noexcept { return _code; }
        size_t             hash() const noexcept { return _hash; }
        int         float_count() const noexcept { return _float_count; }

    protected:
        size_t _hash;
        std::string _name;
        std::string _code;
        int _float_count;
    };

    struct op : named_code
    {
        using return_type = distance;

        op(const std::string& name, const std::string& code, int float_count = 0) : named_code(name, code, float_count) {}

        template<typename... Ts>
        distance_instance operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const;
    };

    struct primitive : named_code
    {
        friend op;
        friend distance_instance;
        using return_type = distance;

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
        using return_type = position;

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

        void enable(const op& o, bool force = false) {
            if (!force && _hashes.count(o.hash()) != 0)
            {
                log_w << "Operator with name " << o.name() << " already enabled. Set force parameter to true to override.";
                return;
            }
            _op_stream << "case " << _op_count << ": {" << o.code() << ";}\n";
            ++_op_count;
            _hashes.emplace(o.hash());
        }
        void enable(const primitive& p, bool force = false)
        {
            if (!force && _hashes.count(p.hash()) != 0)
            {
                log_w << "Primitive with name " << p.name() << " already enabled. Set force parameter to true to override.";
                return;
            }
            _prim_stream << "case " << _prim_count << ": {" << p.code() << ";}\n";
            ++_prim_count;
            _hashes.emplace(p.hash());
        }

        void enable(const modifier& m, bool force = false)
        {
            if (!force && _hashes.count(m.hash()) != 0)
            {
                log_w << "Modifier with name " << m.name() << " already enabled. Set force parameter to true to override.";
                return;
            }
            _mod_stream << "case " << _mod_count << ": {" << m.code() << ";}\n";
            ++_mod_count;
            _hashes.emplace(m.hash());
        }

        std::string assemble() const {
            return _op_stream.str() + end + _prim_stream.str() + end + _mod_stream.str() + end;
        }

    private:
        constexpr static const char* op_header = "float sdf_operator(uint id, uint sdf_instance_id, float sdf_op_lhs, float sdf_op_rhs) {\nswitch(id) {\n";
        int _op_count = 0;
        std::stringstream _op_stream;

        constexpr static const char* prim_header = "float sdf_primitive(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n";
        int _prim_count = 0;
        std::stringstream _prim_stream;

        constexpr static const char* mod_header = "vec3 sdf_modifier(uint id, uint sdf_instance_id, vec3 sdf_position) {\nswitch(id) {\n";
        int _mod_count = 0;
        std::stringstream _mod_stream;

        std::set<uint64_t> _hashes;

        constexpr static const char* end = "}}\n\n";
    };
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

    sdf::op op_union("union", "return min(sdf_op_lhs, sdf_op_rhs)");
    sdf::op op_sub("subtract", "return max(-sdf_op_lhs, sdf_op_rhs)");
    sdf::op op_intersect("intersect", "return max(sdf_op_lhs, sdf_op_rhs)");


    sdf::primitive prim_box("box", R"(
            vec3 size = sdf_get_vec3(sdf_instance_id, 0);
            return length(max(abs(sdf_position)-size,0.0));
        )", 3);
    sdf::primitive prim_sphere("sphere", "return length(sdf_position)-sdf_get_float(sdf_instance_id, 0)", 1);
    sdf::primitive prim_torus("torus", R"(
            vec2 t = sdf_get_vec2(sdf_instance_id, 0);
            vec2 q = vec2(length(sdf_position.xz)-t.x, sdf_position.y);
            return length(sdf_position)-t.y;
        )", 1);


    sdf::modifier mod_rep("repeat", R"(
            vec3 c = sdf_get_vec3(sdf_instance_id, 0);
            return mod(sdf_position,c)-0.5*c;
        )", 3);
    sdf::modifier mod_transform("transform", R"(
            mat4 invmat = sdf_get_mat4(sdf_instance_id, 0); // Inverse Mat4!
            return (invmat * vec4(p, 1)).xyz;
        )", 3);


    sdf::host host;
    host.enable(op_union);
    host.enable(op_sub);
    host.enable(op_intersect);

    host.enable(prim_box);
    host.enable(prim_sphere);
    host.enable(prim_torus);

    host.enable(mod_rep);
    host.enable(mod_transform);
    host.enable(mod_transform);

    const auto opfun = host.assemble();



    auto code = op_union(op_union(prim_box(mod_rep(glm::vec3(9, 2, 3)), glm::vec3(1, 4, 4)), prim_box(glm::vec3(3, 2, 1))), prim_box(mod_rep(glm::vec3(9, 2, 3)), glm::vec3(2, 9, 0.2f)));

    std::stringstream shader;
    shader << build_function(op_union);
    shader << build_function(prim_box);
    shader << build_function(mod_rep);
    shader << get_offset;
    shader << sdf_prefix;
    shader << "distance=" << code.str() << ";\nreturn distance;\n}";
    std::string shader_code = shader.str();

    auto data = code.get_bundle().stream_data();
    std::vector<float> datatata(data.begin(), data.end());

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
