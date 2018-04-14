#pragma once

namespace gfx
{
    namespace
    {
        constexpr glm::quat quat_conjugate(const glm::quat& q)
        {
            return glm::quat(q.w, -q.x, -q.y, -q.z);
        }

        constexpr glm::quat quat_mul(const glm::quat& pp, const glm::quat& q)
        {
            glm::quat dst{};
            const glm::quat p(pp);

            dst.w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
            dst.x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
            dst.y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
            dst.z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
            return dst;
        }

        constexpr glm::vec3 quat_mul(const glm::quat& q, const glm::vec3& p)
        {
            const glm::quat q_vec(0, p.x, p.y, p.z);
            const glm::quat result = quat_mul(quat_mul(q, q_vec), quat_conjugate(q));
            return glm::vec3(result.x, result.y, result.z);
        }

        constexpr glm::vec3 vec3_mul(const glm::vec3& p, const glm::vec3& q)
        {
            return glm::vec3(
                p.x * q.x,
                p.y * q.y,
                p.z * q.z
            );
        }

        constexpr glm::vec3 vec3_div(const glm::vec3& p, const glm::vec3& q)
        {
            return glm::vec3(
                p.x / q.x,
                p.y / q.y,
                p.z / q.z
            );
        }

        template<typename T>
        constexpr T sqrt_newton_raphson(const T x, const T curr, const T prev)
        {
            return curr == prev
                ? curr
                : sqrt_newton_raphson<T>(x, T(0.5) * (curr + x / curr), curr);
        }

        template<typename T>
        constexpr T constexpr_sqrt(const T x)
        {
            return x >= 0 && x < std::numeric_limits<T>::infinity()
                ? sqrt_newton_raphson<T>(x, x, 0)
                : std::numeric_limits<T>::quiet_NaN();
        }

        constexpr float vec3_len(const glm::vec3& p)
        {
            return constexpr_sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
        }
    }

    constexpr transform::transform(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation) noexcept
        : position(position), scale(scale), rotation(rotation) {}

    constexpr bool transform::operator==(const transform& other) const noexcept
    {
        return
            position.x == other.position.x &&
            position.y == other.position.y &&
            position.z == other.position.z &&

            scale.x == other.scale.x &&
            scale.y == other.scale.y &&
            scale.z == other.scale.z &&

            rotation.w == other.rotation.w &&
            rotation.x == other.rotation.x &&
            rotation.y == other.rotation.y &&
            rotation.z == other.rotation.z;
    }

    constexpr bool transform::operator!=(const transform& other) const noexcept { return !operator==(other); }

    constexpr transform transform::operator*(const transform& other) const noexcept
    {
        transform result = *this;
        result *= other;
        return result;
    }

    constexpr vertex transform::operator*(const vertex& other) const noexcept
    {
        vertex p(other);
        p.position.x *= scale.x;
        p.position.y *= scale.y;
        p.position.z *= scale.z;
        p.position = quat_mul(rotation, p.position);
        p.position.x += position.x;
        p.position.y += position.y;
        p.position.z += position.z;

        p.normal.x *= scale.x;
        p.normal.y *= scale.y;
        p.normal.z *= scale.z;
        p.normal = quat_mul(rotation, p.normal);
        return p;
    }

    constexpr transform& transform::operator*=(const transform& other) noexcept
    {
        position.x += other.position.x;
        position.y += other.position.y;
        position.z += other.position.z;

        scale.x *= other.scale.x;
        scale.y *= other.scale.y;
        scale.z *= other.scale.z;

        quat_mul(rotation, other.rotation);
        return *this;
    }

    constexpr transform::transform(const glm::mat4& mat) noexcept
    {
        glm::vec3 ax(mat[0]);
        glm::vec3 ay(mat[1]);
        glm::vec3 az(mat[2]);
        position = mat[3];
        position.x /= mat[3].w;
        position.y /= mat[3].w;
        position.z /= mat[3].w;

        ax = vec3_div(ax, glm::vec3(scale.x = vec3_len(ax)));
        ay = vec3_div(ay, glm::vec3(scale.y = vec3_len(ay)));
        az = vec3_div(az, glm::vec3(scale.z = vec3_len(az)));

        const float m00 = ax.x;
        const float m11 = ay.y;
        const float m22 = az.z;
        const float m01 = ax.y;
        const float m10 = ay.x;
        const float m02 = ax.z;
        const float m20 = az.x;
        const float m12 = ay.z;
        const float m21 = az.y;
        float t = 0;
        if (m22 < 0)
        {
            if (m00 > m11)
            {
                t = 1 + m00 - m11 - m22;
                rotation = glm::quat(m12-m21, t, m01+m10, m20+m02);
            }
            else
            {
                t = 1 - m00 + m11 - m22;
                rotation = glm::quat(m20-m02, m01+m10, t, m12+m21);
            }
        }
        else
        {
            if (m00 < -m11)
            {
                t = 1 - m00 - m11 + m22;
                rotation = glm::quat(m01-m10, m20+m02, m12+m21, t);
            }
            else
            {
                t = 1 + m00 + m11 + m22;
                rotation = glm::quat(t, m12-m21, m20-m02, m01-m10);
            }
        }
        const float half_t_sqrt = 0.5f / constexpr_sqrt(t);
        rotation.w *= half_t_sqrt;
        rotation.x *= half_t_sqrt;
        rotation.y *= half_t_sqrt;
        rotation.z *= half_t_sqrt;
    }

    constexpr transform::operator glm::mat4() const noexcept
    {
        const glm::vec3 x = vec3_mul(left(), glm::vec3(scale.x));
        const glm::vec3 y = vec3_mul(up(), glm::vec3(scale.y));
        const glm::vec3 z = vec3_mul(backward(), glm::vec3(scale.z));
        return glm::mat4(glm::vec4(x, 0), glm::vec4(y, 0), glm::vec4(z, 0), glm::vec4(position, 1));
    }

    constexpr glm::vec3 transform::up() const noexcept { return quat_mul(rotation, glm::vec3(0, 1, 0)); }
    constexpr glm::vec3 transform::down() const noexcept { return quat_mul(rotation, glm::vec3(0, -1, 0)); }
    constexpr glm::vec3 transform::left() const noexcept { return quat_mul(rotation, glm::vec3(1, 0, 0)); }
    constexpr glm::vec3 transform::right() const noexcept { return quat_mul(rotation, glm::vec3(-1, 0, 0)); }
    constexpr glm::vec3 transform::forward() const noexcept { return quat_mul(rotation, glm::vec3(0, 0, -1)); }
    constexpr glm::vec3 transform::backward() const noexcept { return quat_mul(rotation, glm::vec3(0, 0, 1)); }
}