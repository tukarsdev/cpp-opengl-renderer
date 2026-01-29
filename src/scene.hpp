#pragma once

#include "camera.hpp"
#include "shader_program.hpp"
#include "movement.hpp"
#include "light_manager.hpp"
#include "game_map.hpp"

class Scene {
public:
	using RenderFunction = std::function<void(ShaderProgram*)>;
private:
	struct Self {
		WindowManager* wm;
		InputManager* input;
		std::unique_ptr<Camera> camera;
		std::unique_ptr<Movement> movement;
		std::unique_ptr<GameMap> game_map;

		std::unique_ptr<ShaderProgram> program;
		std::unique_ptr<ShaderProgram> shadow_program;
		std::unique_ptr<LightManager> light_manager;
	} self;

	Scene() = default;

public:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(Scene&& other) = delete;

	static std::optional<std::unique_ptr<Scene>>
	New(WindowManager* wm, InputManager* input) {
		auto program_opt = ShaderProgram::New(
			"shaders/phong.vert", 
			"shaders/phong.frag"
		);
		
		if (!program_opt.has_value()) { return std::nullopt; }
		auto& program = program_opt.value();
		
		auto shadow_program_opt = ShaderProgram::New(
			"shaders/shadow.vert", 
			"shaders/shadow.frag"
		);
		if (!shadow_program_opt.has_value()) { return std::nullopt; }
		auto& shadow_program = shadow_program_opt.value();
		
		auto camera = Camera::New();
		auto movement = Movement::New(input);
		

		auto light_manager_opt = LightManager::New(program.get(), shadow_program.get());
		if (!light_manager_opt.has_value()) { return std::nullopt; }
		auto& light_manager = light_manager_opt.value();

		auto game_map = GameMap::New(camera.get(), light_manager.get(), movement.get());
		
		auto scene = std::unique_ptr<Scene>(new Scene());
		auto& self = scene->self;
		
		self.wm = wm;
		self.input = input;

		self.camera = std::move(camera);
		self.movement = std::move(movement);
		self.game_map = std::move(game_map);

		self.program = std::move(program);
		self.shadow_program = std::move(shadow_program);
		self.light_manager = std::move(light_manager);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		self.camera->set_position(glm::vec3(20.f, 25.f, 40.f));
		self.camera->look_at(glm::vec3(0.f, 10.f, 0.f));
		self.camera->update(0);

		return scene;
	}

	void render(double dt) {
		auto& wm = self.wm;
		auto& input = self.input;
		auto& camera = self.camera;
		auto& movement = self.movement;
		auto& game_map = self.game_map;
		auto& program = self.program;
		auto& shadow_program = self.shadow_program;
		auto& light_manager = self.light_manager;

		glm::ivec2 res = wm->get_resolution();

		if (movement->get_change_locked()) {
			movement->set_change_locked(false);
			input->set_mouse_locked(!input->get_mouse_locked());
		}

		glm::dvec2 delta = input->get_mouse_delta();
		camera->process_mouse_movement(delta);
		camera->process_movement_input(movement->get_movement_vec());
		camera->process_mouse_scroll(movement->get_zoom_vec().x);
		camera->process_vertical_input(movement->get_y_axis_vec().x);
		camera->update(dt);

		program->use();
		program->uniform(program->location("cam_pos"), camera->get_position());
		program->uniform(program->location("view"), camera->get_view());

		const float aspect = wm->get_aspect_ratio();
		glm::mat4 projection = glm::perspective(glm::radians(90.f), aspect, 0.01f, 5000.f);
		program->uniform(program->location("projection"), projection);

		self.game_map->update(dt);

		auto render_function = [this]() {
			self.game_map->draw();
		};

		light_manager->generate_depth_maps(render_function);
		static const GLfloat bgd[] = { .6745f, .9098f, .9804f, 1.f };
		light_manager->render_with_shadows(render_function, res.x, res.y, bgd);
	}
};