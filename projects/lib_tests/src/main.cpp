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
        named_code(const std::string& name, const std::string& code)
            : _name(name), _code(code) {}

        const std::string& name() const noexcept { return _name; }
        const std::string& code() const noexcept { return _code; }

    protected:
        std::string _name;
        std::string _code;
    };

    struct op : named_code
    {
        using return_type = distance;

        op(const std::string& name, const std::string& code) : named_code(name, code) {}

        template<typename... Ts>
        distance_instance operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const;
    };

    struct primitive : named_code
    {
        friend op;
        friend distance_instance;
        using return_type = distance;

        primitive(const std::string& name, const std::string& code) : named_code(name, code) {}

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

        modifier(const std::string& name, const std::string& code) : named_code(name, code) {}

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
        str << "float " << code.name() << "(float sdf_op_lhs, float sdf_op_rhs, int sdf_id) {\n"
            << code.code() << ";\n}\n";
        return str.str();
    }

    std::string build_function(const sdf::primitive& code)
    {
        std::stringstream str;
        str << "float " << code.name() << "(vec3 sdf_position, int sdf_id) {\n"
            << code.code() << ";\n}\n";
        return str.str();
    }

    std::string build_function(const sdf::modifier& code)
    {
        std::stringstream str;
        str << "vec3 " << code.name() << "(vec3 sdf_position, int sdf_id) {\n"
            << code.code() << ";\n}\n";
        return str.str();
    }
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

    sdf::op         op_union("union", "return min(sdf_op_lhs, sdf_op_rhs)");
    sdf::primitive  prim_box("box", R"(
            vec3 size = vec3(sdf_params[sdf_id], sdf_params[sdf_id+1], sdf_params[sdf_id+2]);
            return length(max(abs(sdf_position)-size,0.0));
        )");
    sdf::modifier   mod_rep("repeat", R"(
            vec3 c = vec3(sdf_params[sdf_id], sdf_params[sdf_id+1], sdf_params[sdf_id+2]);
            return mod(p,c)-0.5*c;
        )");

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
