#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace res
{
	struct transform
	{
		transform() noexcept = default;
		transform(transform&& other) noexcept = default;
		transform(const transform& other) noexcept = default;
		transform& operator=(transform&& other) noexcept = default;
		transform& operator=(const transform& other) noexcept = default;
        transform(glm::vec3 position, glm::vec3 scale = glm::vec3{1,1,1}, glm::quat rotation = { 1, 0, 0, 0 }) noexcept;

		bool operator==(const transform& other) const noexcept;
		bool operator!=(const transform& other) const noexcept;

		transform operator*(const transform& other) const noexcept;
		transform& operator*=(const transform& other) noexcept;

		transform(glm::mat4 mat) noexcept;
		operator glm::mat4() const noexcept;

        glm::vec3 up() const noexcept;
        glm::vec3 down() const noexcept;
        glm::vec3 left() const noexcept;
        glm::vec3 right() const noexcept;
        glm::vec3 forward() const noexcept;
        glm::vec3 backward() const noexcept;

		glm::vec3 position{ 0, 0, 0 };
		glm::vec3 scale{ 1, 1, 1 };
		glm::quat rotation{1, 0, 0, 0};
	};
}
