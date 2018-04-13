#include "../transform.hpp"

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace res
{
    transform::transform(const glm::vec3 position, const glm::vec3 scale, const glm::quat rotation) noexcept
        : position(position), scale(scale), rotation(rotation)
    {

    }

    bool transform::operator==(const transform& other) const noexcept
    {
        return position == other.position &&
            scale == other.scale &&
            rotation == other.rotation;
    }

    bool transform::operator!=(const transform& other) const noexcept
    {
        return position != other.position ||
            scale != other.scale ||
            rotation != other.rotation;
    }

    transform::transform(const glm::mat4 mat) noexcept
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        decompose(mat, scale, rotation, position, skew, perspective);
    }

    transform::operator glm::mat4() const noexcept
    {
        return translate(glm::mat4(1.f), position) * toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
    }

    glm::vec3 transform::up() const noexcept
    {
        return rotate(rotation, glm::vec3(0, 1, 0));
    }

    glm::vec3 transform::down() const noexcept
    {
        return rotate(rotation, glm::vec3(0, -1, 0));
    }

    glm::vec3 transform::left() const noexcept
    {
        return rotate(rotation, glm::vec3(1, 0, 0));
    }

    glm::vec3 transform::right() const noexcept
    {
        return rotate(rotation, glm::vec3(-1, 0, 0));
    }

    glm::vec3 transform::forward() const noexcept
    {
        return rotate(rotation, glm::vec3(0, 0, -1));
    }

    glm::vec3 transform::backward() const noexcept
    {
        return rotate(rotation, glm::vec3(0, 0, 1));
    }

    transform transform::operator*(const transform& other) const noexcept
	{
		return transform(static_cast<glm::mat4>(*this) * static_cast<glm::mat4>(other));
	}

	transform& transform::operator*=(const transform& other) noexcept
	{
		return *this = *this * other;
	}
}
