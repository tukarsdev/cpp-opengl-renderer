#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

enum class CameraMode {
	Flying, // Free flying mode - uses update_flying
	Watch,  // TODO: looks at subject
};

class Camera {
public:
	using CameraUpdateFunction = std::function<void(double)>;

	static constexpr double PI = glm::pi<double>();
	static constexpr double TAU = PI * 2;
	static constexpr double HALF_PI = PI / 2;

	static constexpr float VELOCITY_STOP_THRESHOLD_SQ = 1e-6f;
	static constexpr double MAX_DY = glm::radians(89.9f);
	static constexpr double MIN_DY = glm::radians(-89.9f);
private:
	struct Self {
		CameraMode mode = CameraMode::Flying;
		CameraUpdateFunction update_function;

		double mouse_sensitivity = 0.0008f;
		double movement_speed = 50.0f;
		double damping_factor = 10.0f;
		double current_vertical_input = 0.0f;

		double dx = 0;
		double dy = 0;

		glm::vec3 position = glm::vec3(0.f);
		glm::vec3 front = glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 right = glm::vec3(1.f, 0.f, 0.f);
		glm::vec3 world_up = glm::vec3(0.f, 1.f, 0.f);

		glm::vec3 movement_velocity = glm::vec3(0.f);
		glm::vec2 move_input = glm::vec2(0.f);
		glm::dvec2 mouse_delta = glm::dvec2(0.0f);
	} self;

	Camera() = default;

private:
	void update_flying(double dt) {
		if (glm::length(self.mouse_delta) > 0) {
			double delta_x = self.mouse_delta.x * self.mouse_sensitivity;
			double delta_y = self.mouse_delta.y * self.mouse_sensitivity;

			self.dx += delta_x;
			self.dy -= delta_y;

			self.dx = std::fmod(self.dx, TAU);
			if (self.dx < 0) { self.dx += TAU; }

			self.dy = std::clamp(self.dy, MIN_DY, MAX_DY);

			update_camera_vectors();
		}

		glm::vec3 target_velocity = glm::vec3(0.0f);
		if (glm::length(self.move_input) > 1e-6f) {
			target_velocity += self.front
				* self.move_input.y * (float) self.movement_speed;
			target_velocity += self.right
				* self.move_input.x * (float) self.movement_speed;
		}
		if (std::abs(self.current_vertical_input) > 1e-6f) {
			target_velocity += self.world_up
				* (float) self.current_vertical_input
				* (float) self.movement_speed;
		}

		float blend_factor = (float) (1.0 - std::exp(-self.damping_factor * dt));
		self.movement_velocity = glm::mix(self.movement_velocity, target_velocity, blend_factor);

		self.position += self.movement_velocity * ((float) dt);

		if (glm::length(self.movement_velocity) < VELOCITY_STOP_THRESHOLD_SQ) {
			self.movement_velocity = glm::vec3(0.0f);
		}
	}

	void update_watch(double dt) {
		// TODO
	}

	void update_camera_vectors() {
		glm::vec3 new_front = glm::vec3(
			cos(self.dx) * cos(self.dy),
			sin(self.dy),
			sin(self.dx) * cos(self.dy)
		);
		self.front = glm::normalize(new_front);
		self.right = glm::normalize(glm::cross(self.front, self.world_up));
		self.up = glm::normalize(glm::cross(self.right, self.front));
	}

public:
	static std::unique_ptr<Camera> New(
		glm::vec3 position = glm::vec3(0.0f),
		float init_dx = -HALF_PI,
		float init_dy = 0.0f,
		glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f)
	) {
		auto cam = std::unique_ptr<Camera>(new Camera());
		auto& self = cam->self;
		self.position = position;
		self.dx = init_dx;
		self.dy = init_dy;
		self.world_up = world_up;

		cam->update_camera_vectors();
		cam->set_camera_mode(CameraMode::Flying);

		return cam;
	};

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(Camera&& other) = delete;
	
	void update(double dt) const {
		self.update_function(dt);
	}

	void set_camera_mode(CameraMode mode) {
		self.mode = mode;
		switch (self.mode) {
		case CameraMode::Flying:
			self.update_function = [this](double dt) { this->update_flying(dt); };
			break;
		case CameraMode::Watch:
			self.update_function = [this](double dt) { this->update_watch(dt); };
			break;
		}
		self.movement_velocity = glm::vec3(0.0f);
	}

	CameraMode get_camera_mode() const { return self.mode; }

	void process_movement_input(glm::vec2 direction) {
		self.move_input = direction;
	}

	void process_vertical_input(float amount) {
		self.current_vertical_input = amount;
	}

	void process_mouse_movement(glm::dvec2 delta) {
		self.mouse_delta = delta;
	}

	void process_mouse_scroll(float y_offset) {
		// TODO
	}

	glm::vec3 get_position() const { return self.position; }
	glm::vec3 get_front() const { return self.front; }
	glm::vec3 get_up() const { return self.up; }
	glm::vec3 get_right() const { return self.right; }

	glm::mat4 get_view() const {
		return glm::lookAt(self.position, self.position + self.front, self.up);
	}

	void set_position(const glm::vec3& position) {
		self.position = position;
	}

	void set_orientation(double dx_rad, double dy_rad) {
		self.dx = dx_rad;
		self.dy = std::clamp(dy_rad, MIN_DY, MAX_DY);
		update_camera_vectors();
	}


	void look_at(const glm::vec3& target) {
		self.front = glm::normalize(target - self.position);
		self.right = glm::normalize(glm::cross(self.front, self.world_up));
		self.up = glm::normalize(glm::cross(self.right, self.front));

		self.dy = asin(self.front.y);
		self.dx = atan2(self.front.z, self.front.x);

		self.dy = std::clamp(self.dy, MIN_DY, MAX_DY);
	}

	double get_speed() const { return self.movement_speed; }
	void set_speed(double speed) {
		self.movement_speed = speed;
	}
};

