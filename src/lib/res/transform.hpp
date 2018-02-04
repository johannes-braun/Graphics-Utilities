#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace res
{
	struct transform
	{
		transform() = default;
		transform(transform&& other) noexcept = default;
		transform(const transform& other) = default;
		transform& operator=(transform&& other) noexcept = default;
		transform& operator=(const transform& other) = default;
        transform(glm::vec3 position, glm::vec3 scale = glm::vec3{1,1,1}, glm::quat rotation = glm::angleAxis(0.f, glm::vec3{0}));

		bool operator==(const transform& other) const;
		bool operator!=(const transform& other) const;

		transform operator*(const transform& other) const;
		transform& operator*=(const transform& other);

		transform(glm::mat4 mat);
		operator glm::mat4() const;

        glm::vec3 up() const;
        glm::vec3 down() const;
        glm::vec3 left() const;
        glm::vec3 right() const;
        glm::vec3 forward() const;
        glm::vec3 backward() const;

		glm::vec3 position{ 0, 0, 0 };
		glm::vec3 scale{ 1, 1, 1 };
		glm::quat rotation = glm::angleAxis(0.f, glm::vec3{0});
	};
}
