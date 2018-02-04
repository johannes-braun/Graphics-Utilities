#include "transform.hpp"
#include <glm/ext.hpp>
#include "glm/gtx/matrix_decompose.hpp"
#include <utility>

namespace res
{
    transform::transform(const glm::vec3 position, const glm::vec3 scale, const glm::quat rotation)
        : position(position), scale(scale), rotation(rotation)
    {

    }

    bool transform::operator==(const transform& other) const
    {
        return position == other.position &&
            scale == other.scale &&
            rotation == other.rotation;
    }

    bool transform::operator!=(const transform& other) const
    {
        return position != other.position ||
            scale != other.scale ||
            rotation != other.rotation;
    }

    transform::transform(const glm::mat4 mat)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(mat, scale, rotation, position, skew, perspective);
    }

    transform::operator glm::mat4() const
    {
        return glm::translate(glm::mat4(1.f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.f), scale);
    }

    glm::vec3 transform::up() const
    {
        return glm::rotate(rotation, glm::vec3(0, 1, 0));
    }

    glm::vec3 transform::down() const
    {
        return glm::rotate(rotation, glm::vec3(0, -1, 0));
    }

    glm::vec3 transform::left() const
    {
        return glm::rotate(rotation, glm::vec3(1, 0, 0));
    }

    glm::vec3 transform::right() const
    {
        return glm::rotate(rotation, glm::vec3(-1, 0, 0));
    }

    glm::vec3 transform::forward() const
    {
        return glm::rotate(rotation, glm::vec3(0, 0, -1));
    }

    glm::vec3 transform::backward() const
    {
        return glm::rotate(rotation, glm::vec3(0, 0, 1));
    }

    transform transform::operator*(const transform& other) const
	{
		return transform(static_cast<glm::mat4>(*this) * static_cast<glm::mat4>(other));
	}

	transform& transform::operator*=(const transform& other)
	{
		return *this = *this * other;
	}
}
