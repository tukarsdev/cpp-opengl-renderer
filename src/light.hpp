#pragma once


enum class LightType {
	Directional = 0,
	Positional = 1,
	Spot = 2
};

class Light {
public:
	static constexpr float SPOT_MAX_RAD = glm::radians(179.999f);
	static constexpr float SPOT_MIN_RAD = glm::radians(0.f);
	static constexpr float ORTHO_SIZE = 400.f;
	static constexpr float NEAR_PLANE = 3.f;
	static constexpr float FAR_PLANE = 400.f;

private:
	struct Self {
		LightType type;
		glm::vec3 dir;
		glm::vec3 pos;
		glm::vec3 col;
		float range;
		float attl;
		float attq;
		float spinn;
		float spout;
		glm::mat4 projlmat;
	} self;

	Light() = default;

private:
	static glm::vec3 calculate_up_vector(const glm::vec3& direction) {
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		if (glm::abs(glm::dot(glm::normalize(direction), up)) > 0.999f) {
			up = glm::vec3(0.0f, 0.0f, (direction.y > 0.0f) ? -1.0f : 1.0f);
		}
		return up;
	}

	static glm::mat4 get_directional_projlmat(Self& self) {
		glm::mat4 projection = glm::ortho(
			-ORTHO_SIZE, ORTHO_SIZE, -ORTHO_SIZE, ORTHO_SIZE,
			NEAR_PLANE, FAR_PLANE
		);


		glm::vec3 ortho_pos = self.pos - (self.dir * (FAR_PLANE / 2));
		glm::mat4 view = glm::lookAt(ortho_pos, ortho_pos + self.dir, glm::vec3(0.f, 1.f, 0.f));
		return projection * view;
	}

	static glm::mat4 get_positional_projlmat(Self& self) {
		return glm::mat4(1.f);
	}

	static glm::mat4 get_spot_projlmat(Self& self) {
		float fov_degrees = glm::clamp(self.spout * 2, SPOT_MIN_RAD, SPOT_MAX_RAD);

		glm::mat4 projection = glm::perspective(
			fov_degrees,
			1.0f, 3.f, 400.f
		);

		glm::vec3 up = calculate_up_vector(self.dir);

		glm::mat4 view = glm::lookAt(self.pos, self.pos + self.dir, up);

		return projection * view;
	}

	static glm::mat4 calc_projlmat(Self& self) {
		glm::mat4 projlmat;
		switch (self.type) {
		case LightType::Directional:
			projlmat = get_directional_projlmat(self);
			break;
		case LightType::Positional:
			projlmat = get_positional_projlmat(self);
			break;
		case LightType::Spot:
			projlmat = get_spot_projlmat(self);
			break;
		default: projlmat = glm::mat4(1.f); break;
		}
		return projlmat;
	}

	static std::tuple<float, float> get_attl_attq(float range) {
		const float INTENSITY_THRESHOLD = 255.0f;
		float attl = 0.f;
		float attq = 0.f;

		if (range <= 1e-4f) {
			attq = std::numeric_limits<float>::max();
		} else {
			attq = std::max(INTENSITY_THRESHOLD / (range * range), 0.f); // do not change this
		}
		return std::make_tuple(attl, attq);
	}

public:
	static std::unique_ptr<Light> New(
		LightType type,
		glm::vec3 dir,
		glm::vec3 pos,
		glm::vec3 col = glm::vec3(0.f),
		float range = 100.f,
		float spinn = 0.f,
		float spout = 0.f
	) {
		auto [attl, attq] = get_attl_attq(range);

		auto light = std::unique_ptr<Light>(new Light());
		auto& self = light->self;

		self.type = type;
		self.pos = pos;
		self.dir = dir;
		self.col = col;
		self.range = range;
		self.attl = attl;
		self.attq = attq;
		self.spinn = glm::radians(spinn);
		self.spout = glm::radians(spout);
		self.projlmat = calc_projlmat(self);

		return light;
	}

	std::array<glm::mat4, 6> get_cubemap_face_matrices() const {
		glm::mat4 shadow_proj = glm::perspective(
			glm::radians(90.0f), 1.0f, 3.f, 400.f//self.range
		);
		std::array<glm::mat4, 6> shadow_transforms;

		glm::vec3 right = glm::vec3( 1.f,  0.f,  0.f);
		glm::vec3 left  = glm::vec3(-1.f,  0.f,  0.f);
		glm::vec3 up    = glm::vec3( 0.f,  1.f,  0.f);
		glm::vec3 down  = glm::vec3( 0.f, -1.f,  0.f);
		glm::vec3 forw  = glm::vec3( 0.f,  0.f,  1.f);
		glm::vec3 back  = glm::vec3( 0.f,  0.f, -1.f);

		auto look = [&](glm::vec3 look, glm::vec3 up) { 
			return shadow_proj * glm::lookAt(
				self.pos, self.pos + look, up
			);
		};

		shadow_transforms[0] = look(right, down);
		shadow_transforms[1] = look(left, down);
		shadow_transforms[2] = look(up, forw);
		shadow_transforms[3] = look(down, back);
		shadow_transforms[4] = look(forw, down);
		shadow_transforms[5] = look(back, down);

		return shadow_transforms;
	}

	static inline glm::vec3 norm(glm::vec3 dir) {
		return (glm::length(dir) > 1e-6f)
			? glm::normalize(dir)
			: glm::vec3(0.0f, -1.0f, 0.0f);
	}

	LightType get_type() const { return self.type; }
	void set_type(LightType type) {
		self.type = type;
		self.projlmat = calc_projlmat(self);
	}

	void set_pos_dir(glm::vec3 pos, glm::vec3 dir) {
		self.pos = pos;
		self.dir = norm(dir);
		self.projlmat = calc_projlmat(self);
	}

	glm::vec3 get_dir() const { return self.dir; };
	void set_dir(glm::vec3 dir) {
		self.dir = norm(dir);
		self.projlmat = calc_projlmat(self);
	};

	glm::vec3 get_pos() const { return self.pos; };
	void set_pos(glm::vec3 pos) {
		self.pos = pos;
		self.projlmat = calc_projlmat(self);
	};

	glm::vec3 get_col() const { return self.col; };
	void set_col(glm::vec3 col) {
		self.col = col;
	};

	float get_range() const { return self.range; };
	void set_range(float range) {
		self.range = range;
		auto [attl, attq] = get_attl_attq(range);
		self.attl = attl;
		self.attq = attq;
	};

	float get_attl() const { return self.attl; }
	float get_attq() const { return self.attq; }

	float get_spinn() const { return self.spinn; };
	void set_spinn(float spinn) { 
		self.spinn = glm::radians(spinn); 
	};

	float get_spout() const { return self.spout; };
	void set_spout(float spout) { 
		self.spout = glm::radians(spout);
		self.projlmat = calc_projlmat(self);
	};

	glm::mat4 get_projlmat() const { return self.projlmat; };
	glm::mat4 get_new_projlmat() {
		self.projlmat = calc_projlmat(self);
		return self.projlmat;
	}
};