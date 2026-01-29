#pragma once

#include "light.hpp"

class LightManager {
public:
	static constexpr int MAX_SHADER_LIGHTS = 10;
	static constexpr int SHADOW_MAP_RES = 4096;

	using RenderFunction = std::function<void()>;
	template<typename T, size_t Size>
	using Array = std::array<T, Size>;
	template<typename T>
	using LArray = Array<T, MAX_SHADER_LIGHTS>;
	using GLiArray = LArray<GLint>;
	using GLuArray = LArray<GLuint>;

private:
	struct Self {
		ShaderProgram* program = nullptr;
		ShaderProgram* shadow_program = nullptr;
		std::unique_ptr<ShaderProgram> depth_cubemap_program;

		int num_lights = 0;
		LArray<std::unique_ptr<Light>> lights;
		GLint l_num_lights = -1;

		GLint l_dcm_light_space_matrix = -1;
		GLint l_light_pos_world = -1;
		GLint l_light_far_plane = -1;

		GLiArray ls_type = GLiArray();
		GLiArray ls_dir = GLiArray();
		GLiArray ls_pos = GLiArray();
		GLiArray ls_col = GLiArray();
		GLiArray ls_range = GLiArray();
		GLiArray ls_attl = GLiArray();
		GLiArray ls_attq = GLiArray();
		GLiArray ls_spinn = GLiArray();
		GLiArray ls_spout = GLiArray();
		GLiArray ls_projlmat = GLiArray();
		GLint projlmat_shadow = -1;

		GLiArray ls_shadow_maps_2d = GLiArray();
		GLiArray ls_shadow_maps_cube = GLiArray();

		GLuArray ls_shadow_fbos = GLuArray();
		GLuArray ls_shadow_textures_2d = GLuArray();
		GLuArray ls_shadow_textures_cube = GLuArray();
	} self;

	LightManager() = default;

	void setup_uniforms() {
		auto& program = self.program;
		auto& shadow_program = self.shadow_program;
		auto& depth_cubemap_program = self.depth_cubemap_program;

		program->use();
		self.l_num_lights = program->location("num_lights");

		for (size_t i = 0; i < MAX_SHADER_LIGHTS; i++) {
			std::string is = std::to_string(i);
			std::string base_name = "lights[" + is + "].";

			self.ls_type[i] = program->location(base_name + "type");
			self.ls_dir[i] = program->location(base_name + "dir");
			self.ls_pos[i] = program->location(base_name + "pos");
			self.ls_col[i] = program->location(base_name + "col");
			self.ls_range[i] = program->location(base_name + "range");
			self.ls_attl[i] = program->location(base_name + "attl");
			self.ls_attq[i] = program->location(base_name + "attq");
			self.ls_spinn[i] = program->location(base_name + "spinn");
			self.ls_spout[i] = program->location(base_name + "spout");
			self.ls_projlmat[i] = program->location(base_name + "projlmat");
			self.ls_shadow_maps_2d[i] = program->location("shadow_maps_2d[" + is + "]");
			self.ls_shadow_maps_cube[i] = program->location("shadow_maps_cube[" + is + "]");
		}

		shadow_program->use();
		self.projlmat_shadow = shadow_program->location("projlmat");

		depth_cubemap_program->use();
		self.l_dcm_light_space_matrix = depth_cubemap_program->location("light_space_matrix");
		self.l_light_pos_world = depth_cubemap_program->location("light_pos_world");
		self.l_light_far_plane = depth_cubemap_program->location("light_far_plane");
	}

	void setup_params(int target) {
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		if (target == GL_TEXTURE_2D) {
			glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}
	}

	void setup_shadow_maps() {
		auto& program = self.program;
		GLuint* shadow_fbos = self.ls_shadow_fbos.data();
		GLuint* shadow_textures_2d = self.ls_shadow_textures_2d.data();
		GLuint* shadow_textures_cube = self.ls_shadow_textures_cube.data();

		glGenFramebuffers(MAX_SHADER_LIGHTS, shadow_fbos);

		glGenTextures(MAX_SHADER_LIGHTS, shadow_textures_2d);
		for (size_t i = 0; i < MAX_SHADER_LIGHTS; i++) {
			glBindTexture(GL_TEXTURE_2D, shadow_textures_2d[i]);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				SHADOW_MAP_RES, SHADOW_MAP_RES, 0,
				GL_DEPTH_COMPONENT, GL_FLOAT, NULL
			);
			setup_params(GL_TEXTURE_2D);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		glGenTextures(MAX_SHADER_LIGHTS, shadow_textures_cube);
		for (size_t i = 0; i < MAX_SHADER_LIGHTS; i++) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, self.ls_shadow_textures_cube[i]);
			for (size_t face = 0; face < 6; face++) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
					SHADOW_MAP_RES, SHADOW_MAP_RES, 0, GL_DEPTH_COMPONENT, 
					GL_FLOAT, NULL
				);
			}
			setup_params(GL_TEXTURE_CUBE_MAP);
		}

		for (size_t i = 0; i < MAX_SHADER_LIGHTS; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, self.ls_shadow_fbos[i]);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, self.ls_shadow_textures_2d[i], 0
			);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr
					<< "ERROR::FRAMEBUFFER:: Initial FBO is not complete! Index: "
					<< i << std::endl;
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		program->use();
		for (size_t i = 0; i < MAX_SHADER_LIGHTS; i++) {
			if (self.ls_shadow_maps_2d[i] != -1) {
				program->uniform(
					self.ls_shadow_maps_2d[i], 
					static_cast<GLint>(i)
				);
			}
			if (self.ls_shadow_maps_cube[i] != -1) {
				program->uniform(
					self.ls_shadow_maps_cube[i], 
					static_cast<GLint>(MAX_SHADER_LIGHTS + i)
				);
			}
		}
	}

public:
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;
	LightManager(LightManager&& other) = delete;
	LightManager& operator=(LightManager&& other) = delete;

	static std::optional<std::unique_ptr<LightManager>>
	New(ShaderProgram* program, ShaderProgram* shadow_program) {
		if (!program->is_valid() || !shadow_program->is_valid()) {
			std::cerr << "Program must be initialized before light manager!\n";
			return std::nullopt;
		}

		auto light_manager = std::unique_ptr<LightManager>(new LightManager());
		auto& self = light_manager->self;

		auto depth_cubemap_program_opt = ShaderProgram::New(
			"shaders/depth_cubemap.vert", "shaders/depth_cubemap.frag"
		);
		if (!depth_cubemap_program_opt.has_value()) {
			std::cerr << "Could not load depth cubemap shader program.\n";
			return std::nullopt;
		}
		auto& depth_cubemap_program = depth_cubemap_program_opt.value();

		self.program = program;
		self.shadow_program = shadow_program;
		self.depth_cubemap_program = std::move(depth_cubemap_program);

		light_manager->setup_uniforms();
		light_manager->setup_shadow_maps();

		return light_manager;
	}

	Light* add_light(std::unique_ptr<Light> light) {
		if (self.num_lights >= MAX_SHADER_LIGHTS) {
			std::cerr << "Maximum lights exceeded.\n";
			return nullptr;
		}

		self.lights[self.num_lights] = std::move(light);
		Light* u_light = self.lights[self.num_lights].get();

		self.num_lights += 1;

		return u_light;
	}

	bool remove_light(const Light* light) {
		if (!light) {
			std::cerr << "Cannot remove null light pointer\n";
			return false;
		}

		for (int i = 0; i < self.num_lights; i++) {
			if (self.lights[i].get() == light) {
				self.num_lights -= 1;

				if (i != self.num_lights) {
					self.lights[i] = std::move(self.lights[self.num_lights]);
				}
				
				self.lights[self.num_lights].reset();

				return true;
			}
		}
		return false;
	}

	void clear_background() {
		static const GLfloat bgd[] = { .6745f, .9098f, .9804f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	void generate_depth_maps(RenderFunction render) {
		glViewport(0, 0, SHADOW_MAP_RES, SHADOW_MAP_RES);
		//
		
		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < self.num_lights; i++) {
			Light* light = self.lights[i].get();
			LightType type = light->get_type();

			glBindFramebuffer(GL_FRAMEBUFFER, self.ls_shadow_fbos[i]);

			if (type == LightType::Positional) {
				//self.shadow_program->use();
				self.depth_cubemap_program->use();
				self.depth_cubemap_program->uniform(
					self.l_light_far_plane, 400.f
				);
				self.depth_cubemap_program->uniform(
					self.l_light_pos_world, light->get_pos()
				);
				std::array<glm::mat4, 6> light_space_matrices =
					light->get_cubemap_face_matrices();
				for (int face = 0; face < 6; face++) {
					glFramebufferTexture2D(
						GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
						self.ls_shadow_textures_cube[i], 0);
					glClear(GL_DEPTH_BUFFER_BIT);
					//self.shadow_program->uniform(self.projlmat_shadow, light_space_matrices[face]);
					self.depth_cubemap_program->uniform(self.l_dcm_light_space_matrix, light_space_matrices[face]);
					render();
				}
			} else {
				self.shadow_program->use();
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
					self.ls_shadow_textures_2d[i], 0
				);
				glClear(GL_DEPTH_BUFFER_BIT);
				self.shadow_program->uniform(
					self.projlmat_shadow, light->get_projlmat()
				);
				render();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void render_with_shadows(
		RenderFunction render, int screen_w, int screen_h, const GLfloat* bgd
	) {
		glViewport(0, 0, (GLsizei) screen_w, (GLsizei) screen_h);
		
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		self.program->use();
		update_uniforms();

		for (int i = 0; i < self.num_lights; i++) {
			Light* light = self.lights[i].get();
			LightType type = light->get_type();

			if (light->get_type() == LightType::Positional) { 
				glActiveTexture(GL_TEXTURE0 + MAX_SHADER_LIGHTS + i);
				glBindTexture(GL_TEXTURE_CUBE_MAP, self.ls_shadow_textures_cube[i]);
			} else {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, self.ls_shadow_textures_2d[i]);
			}
		}
		glActiveTexture(GL_TEXTURE0);

		render();
	}

	void update_uniforms() {
		auto program = self.program;
		self.program->use();
		program->uniform(self.l_num_lights, self.num_lights);
		for (int i = 0; i < self.num_lights; i++) {
			Light* light = self.lights[i].get();
			program->uniform(self.ls_type[i], (int) light->get_type());
			program->uniform(self.ls_dir[i], light->get_dir());
			program->uniform(self.ls_pos[i], light->get_pos());
			program->uniform(self.ls_col[i], light->get_col());
			program->uniform(self.ls_range[i], light->get_range());
			program->uniform(self.ls_attl[i], light->get_attl());
			program->uniform(self.ls_attq[i], light->get_attq());
			program->uniform(self.ls_spinn[i], light->get_spinn());
			program->uniform(self.ls_spout[i], light->get_spout());
			program->uniform(self.ls_projlmat[i], light->get_projlmat());
		}
	}

	int get_num_lights() const {
		return self.num_lights;
	}

	Light* get_light(int index) {
		if (index >= 0 && index < self.num_lights) {
			return self.lights[index].get();
		}
		return nullptr;
	}

	const Light* get_light(int index) const {
		if (index >= 0 && index < self.num_lights) {
			return self.lights[index].get();
		}
		return nullptr;
	}
};


