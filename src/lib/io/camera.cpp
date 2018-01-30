#include "camera.hpp"
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <glm/ext.hpp>

namespace io
{
	Camera::Camera()
		: Camera(std::make_shared<DefaultCameraController>())
	{
	}

	Camera::Camera(const std::shared_ptr<CameraController> controller)
		: m_controller(std::move(controller)), m_controller_enabled(static_cast<bool>(m_controller))
	{
	}

    void Camera::setController(std::shared_ptr<CameraController> controller)
    {
        m_controller = std::move(controller);
        m_controller_enabled = static_cast<bool>(m_controller);
    }

    void Camera::update(double delta)
	{
		m_controller ? m_controller->updateTransform(transform, delta) : 0;
	}

	glm::mat4 Camera::viewMatrix() const
	{
		return glm::lookAt(transform.position,
			transform.position + transform.rotation * glm::vec3(0, 0, -1),
			transform.rotation * glm::vec3(0, 1, 0));
	}

	glm::mat4 Camera::projectionMatrix() const
	{
		GLint last_viewport[4];
		glGetIntegerv(GL_VIEWPORT, last_viewport);
		return glm::perspectiveFov(glm::radians(70.f), static_cast<float>(last_viewport[2]), static_cast<float>(last_viewport[3]), 0.01f, 1000.f);
	}

	DefaultCameraController::DefaultCameraController(const float rotation_speed, const float movement_speed)
		: m_rotation_speed(rotation_speed), m_movement_speed(movement_speed)
	{
	}

	bool DefaultCameraController::updateTransform(geo::transform& transform, double delta_time)
	{
		auto&& window = glfwGetCurrentContext();

		const auto mode = glfwGetInputMode(window, GLFW_CURSOR);

		switch (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
		{
		case GLFW_PRESS:
			if (!m_mode_changed)
			{
				glfwGetCursorPos(window, &m_last_cursor_position.x, &m_last_cursor_position.y);
				glfwSetInputMode(window, GLFW_CURSOR, mode != GLFW_CURSOR_DISABLED ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
				m_mode_changed = true;
			}
			break;

		default:
		case GLFW_RELEASE:
			m_mode_changed = false;
			break;
		}

		const auto two_key_check = [window](int key_fwd, int key_bwd) {
			return static_cast<float>(static_cast<int>(glfwGetKey(window, key_fwd) == GLFW_PRESS) - static_cast<int>(glfwGetKey(window, key_bwd) == GLFW_PRESS));
		};

		transform.position += glm::rotate(transform.rotation, glm::vec3{
			two_key_check(GLFW_KEY_D, GLFW_KEY_A),
			two_key_check(GLFW_KEY_E, GLFW_KEY_Q),
			two_key_check(GLFW_KEY_S, GLFW_KEY_W)
		}) * 10.f * delta_time;

		glm::dvec2 current_position;
		glfwGetCursorPos(window, &current_position.x, &current_position.y);
		const glm::dvec2 delta = m_last_cursor_position - current_position;
		m_last_cursor_position = current_position;

		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			// Use the global y-Axis. This brings a more natural control feel.
			transform.rotation = glm::quat(glm::vec3(0, glm::radians(delta.x / 10) * m_rotation_speed, 0.f)) * transform.rotation;
			transform.rotation *= glm::quat(glm::vec3(glm::radians(delta.y / 10) * m_rotation_speed, 0.f, 0.f));
		}
		return true;
	}
}
