#pragma once

#include <glm/glm.hpp>
#include "geo/transform.hpp"
#include <memory>

namespace io
{
	// You can derive from this to create your own Controller.
	class CameraController
	{
	public:
		virtual ~CameraController() = default;
		virtual bool updateTransform(geo::transform& transform, double delta) = 0;
	};

	// The camera is just a controllable wrapper object to contain the Perspective and View matrices.

	class Camera
	{
	public:
		// Use default camera controller.
		Camera();

		// Use custom camera controller.
		Camera(std::shared_ptr<CameraController> controller);

        void setController(std::shared_ptr<CameraController> controller);

		// Pushes the view- and projection-matrices into a ModelViewProjection object (if available).
		// If mvp is set to nullptr, only the controller will be updated.
		void update(double delta);

		glm::mat4 viewMatrix() const;
		glm::mat4 projectionMatrix() const;

		geo::transform transform;
        float field_of_view = glm::radians(80.f);
        float clip_near = 0.1f;
        float clip_far = 1000.f;

	private:
		std::shared_ptr<CameraController> m_controller;
		bool m_controller_enabled = true;
	};

	// The default controller type for the Camera steering.
	// Use with WASD to normally navigate, use E to go up and Q to go down along the local up vector.
	// Right click (with release) to make the mouse cursor grabbed. It will disappear and you can then you can drag the mouse to look around.
	class DefaultCameraController : public CameraController
	{
	public:
		explicit DefaultCameraController(float rotation_speed = 1.f, float movement_speed = 10.f);

		bool updateTransform(geo::transform& transform, double delta) override;

	private:
		bool m_mode_changed = false;

		glm::dvec2 m_last_cursor_position{};

		float m_rotation_speed = 1.f;
		float m_movement_speed = 10.f;
	};
}
