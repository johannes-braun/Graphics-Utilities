#include <gfx/window.hpp>
#include <gfx/log.hpp>

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

    private:
        int _size;
        int _offset;
        std::stringstream _state_code;
        bundle<float> _data;
    };

    struct op
    {
        using return_type = distance;

        op(const std::string& name, const std::string& code)
            : _name(name), _code(code), _state_code("") {}
        // op can reference (op, op), (op, primitive), (primitive, op) or (primitive, primitive)
        // returns a distance

        template<typename... Ts>
        distance_instance operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const;

    private:
        int _id_offset;
        std::stringstream _state_code;
        std::string _name;
        std::string _code;
    };

    struct primitive
    {
        friend op;
        friend distance_instance;
        using return_type = distance;

        primitive(const std::string& name, const std::string& code)
            : _name(name), _code(code) {}
        // primitive can reference (root_pos) or (modifier)
        // returns a distance to the primitive
        
        template<typename... Ts>
        distance_instance operator()(const position_instance& mod, const Ts& ... ts) const;
        
        template<typename... Ts>
        distance_instance operator()(const Ts& ... ts) const
        {
            distance_instance instance;
            std::vector<int> x = {serialize(ts, instance._data)...};
            instance._size = std::accumulate(x.begin(), x.end(), 0);
            instance._offset = 0;
            instance._state_code << _name << "(sdf_position"; // TODO: parameters.
            return instance;
        }

    private:
        std::string _name;
        std::string _code;
    };

    struct modifier
    {
        using return_type = position;

        modifier(const std::string& name, const std::string& code)
            : _name(name), _code(code) {}

        template<typename... Ts>
        position_instance operator()(const Ts& ... ts) const
        {
            position_instance instance;
            std::vector<int> x ={ serialize(ts, instance._data)... };
            instance._size = std::accumulate(x.begin(), x.end(), 0);
            instance._offset = 0;
            instance._state_code << _name << "(sdf_position"; // TODO: parameters.
            return instance;
        }
        // modifier can reference (root_pos) or (modifier) 
        // returns a modified position
    private:
        std::string _name;
        std::string _code;
    };


    template<typename... Ts>
    distance_instance primitive::operator()(const position_instance& mod, const Ts& ... ts) const
    {
        distance_instance instance;
        std::vector<int> x ={ serialize(ts, instance._data)... };
        instance._size = std::accumulate(x.begin(), x.end(), 0);
        instance._offset = mod._offset + mod._size;
        instance._state_code << _name << "(" << mod._state_code.str() << ", " << mod._offset << ")"; // TODO: parameters.
        return instance;
    }

    struct root_position
    {
        constexpr static const char* code = "position";
    };

    template<typename... Ts>
    distance_instance op::operator()(const distance_instance& lhs, const distance_instance& rhs, const Ts& ... ts) const
    {
        distance_instance instance;
        std::vector<int> x ={ serialize(ts, instance._data)... };
        instance._state_code << _name << "(" << lhs._state_code.str() << ", sdf_base_id + " << lhs._offset << "), " << rhs._state_code.str()  << ", sdf_base_id + " << lhs._offset <<"+"<< lhs._size << ")";
        instance._size = std::accumulate(x.begin(), x.end(), 0);
        instance._offset = lhs._offset +lhs._size + rhs._size + std::accumulate(x.begin(), x.end(), 0);
        return instance;
    }
}

int main() 
{
    /*
    float sdf_op_rhs;
    float sdf_op_lhs;

    buffer sdf_params;
    int sdf_id;
    vec3 sdf_position;

     */
    sdf::op         op_union("union", "return min(sdf_op_lhs, sdf_op_rhs)");
    sdf::primitive  prim_box("box",
        R"(
            vec3 size = vec3(sdf_params[sdf_id], sdf_params[sdf_id+1], sdf_params[sdf_id+2]);
            return length(max(abs(sdf_position)-size,0.0));
        )"
    );
    sdf::modifier   mod_rep("repeat",
        R"(
            vec3 c = vec3(sdf_params[sdf_id], sdf_params[sdf_id+1], sdf_params[sdf_id+2]);
            return mod(p,c)-0.5*c;
        )"
    );

    auto code = op_union(op_union(prim_box(mod_rep(glm::vec3(9, 2, 3)), glm::vec3(1, 4, 4)), prim_box(glm::vec3(3, 2, 1))), prim_box(mod_rep(glm::vec3(9, 2, 3)), glm::vec3(2, 9, 0.2f))).str();


    gfx::window window("opengl", "Title", 1280, 720);
    window.set_icon(gfx::image_file("ui/logo.svg", 1.f));
    
    while(window.update())
    {
        log_i << window.delta_time();
    }

    return 0;
}
