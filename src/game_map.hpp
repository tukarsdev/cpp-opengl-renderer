#pragma once

#include "object.hpp"

using ShapeCreator = InstantiableMesh::Shape_Creator;
using ShapeSource = std::variant<ShapeCreator, std::string>;
using ShapeMap = std::unordered_map<std::string, ShapeSource>;


static const ShapeMap shape_map = {
		{"Quad", create_quad},
		{"Box", create_box},
		{"CylinderLQ", [](Vertices& v, Indices& i) { create_cylinder(v, i, 10); }},
		{"Cylinder", [](Vertices& v, Indices& i) { create_cylinder(v, i, 24); }},
		{"CylinderHQ", [](Vertices& v, Indices& i) { create_cylinder(v, i, 64); }},
		{"IcosphereULQ", [](Vertices& v, Indices& i) { create_icosphere(v, i, 0); }},
		{"IcosphereLQ", [](Vertices& v, Indices& i) { create_icosphere(v, i, 1); }},
		{"Icosphere", [](Vertices& v, Indices& i) { create_icosphere(v, i, 3); }},
		{"IcosphereHQ", [](Vertices& v, Indices& i) { create_icosphere(v, i, 5); }},
		{"Wedge", create_wedge },
		{"CornerWedgeInner", create_corner_wedge_inner },
		{"CornerWedgeOuter", create_corner_wedge_outer },
		{"quad", "quad"},
		{"cornell-box", "cornell-box"},
		{"heart", "heart"}
};

class GameMap {
private:
	struct Self {
		std::unordered_map<std::string, std::unique_ptr<InstantiableMesh>> meshes;
		std::vector<std::unique_ptr<Instance>> instances;

		Camera* camera;
		LightManager* light_manager;
		Movement* movement;
		Light* sun;
		Light* spot_1;
		Light* spot_2;
		Light* spot_3;
		Light* point_1;
		Light* point_2;

		Instance* animate_heart;
		double time = 0;

		const float tilt = glm::radians(45.0f);
		const float rotspeed = glm::radians(10.f);
		float orbit_angle = 0.0f;
	} self;

	GameMap() = default;

private:
	void create_meshes() {
		for (const auto& [k, v] : shape_map) {
			if (std::holds_alternative<std::string>(v)) {
				auto& file_obj = std::get<std::string>(v);
				self.meshes.insert({ k, InstantiableMesh::FromFile(file_obj) });
			} else if (std::holds_alternative<ShapeCreator>(v)) {
				auto& shape_func = std::get<ShapeCreator>(v);
				self.meshes.insert({ k, InstantiableMesh::FromShape(shape_func) });
			}
		}
	}

	InstantiableMesh* get_mesh(const std::string& name) {
		auto it = self.meshes.find(name);
		if (it == self.meshes.end()) {
			std::cerr << "Invalid name \"" << name << "\" specified: not in map.\n";
			exit(EXIT_FAILURE);
		}
		return it->second.get();
	}

	Instance* create_instance(InstantiableMesh* object) {
		auto instance = object->instance();
		Instance* inst = instance.get();
		self.instances.push_back(std::move(instance));
		return inst;
	}

	Instance* create_instance_with(
		InstantiableMesh* object, 
		glm::vec3 pos, glm::vec3 size, glm::vec3 col
	) {
		auto inst = create_instance(object);
		inst->set_frame(glm::translate(glm::mat4(1.f), pos));
		inst->set_size(size);
		inst->set_color(glm::vec4(col, 1.f));
		return inst;
	}

	Instance* create_instance_with_rot(
		InstantiableMesh* object, 
		glm::vec3 pos, glm::vec3 axis, float angle, glm::vec3 size, glm::vec3 col
	) {
		auto inst = create_instance(object);
		inst->set_frame(glm::rotate(glm::translate(glm::mat4(1.f), pos), glm::radians(angle), axis));
		inst->set_size(size);
		inst->set_color(glm::vec4(col, 1.f));
		return inst;
	}

	void setup_map() {
		const auto quad = get_mesh("Quad");
		const auto box = get_mesh("Box");
		const auto cylinder_lq = get_mesh("CylinderLQ");
		const auto cylinder = get_mesh("Cylinder");
		const auto cylinder_hq = get_mesh("CylinderHQ");
		const auto icosphere_ulq = get_mesh("IcosphereULQ");
		const auto icosphere_lq = get_mesh("IcosphereLQ");
		const auto icosphere = get_mesh("Icosphere");
		const auto icosphere_hq = get_mesh("IcosphereHQ");
		const auto wedge = get_mesh("Wedge");
		const auto corner_wedge_inner = get_mesh("CornerWedgeInner");
		const auto corner_wedge_outer = get_mesh("CornerWedgeOuter");
		const auto quad_2 = get_mesh("quad");
		const auto cornell_box = get_mesh("cornell-box");
		const auto heart = get_mesh("heart");

		create_instance_with(
			box,
			glm::vec3(0.f, -2.5f, 0.f),
			glm::vec3(512.f, 5.f, 512.f),
			glm::vec3(0.640f, 0.636f, 0.648f)
		);

		create_instance_with(
			box,
			glm::vec3(0.f, 15.0f, 0.f),
			glm::vec3(15.f, 30.f, 15.f),
			glm::vec3(0.804f, 0.330f, 0.295f)
		);

		create_instance_with_rot(
			wedge,
			glm::vec3(30.f, 15.0f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			0.f,
			glm::vec3(45.f, 30.f, 15.f),
			glm::vec3(0.483f, 0.714f, 0.910f)
		);

		create_instance_with_rot(
			wedge,
			glm::vec3(0.f, 15.0f, -30.f),
			glm::vec3(0.f, 1.f, 0.f),
			90.f,
			glm::vec3(45.f, 30.f, 15.f),
			glm::vec3(0.969f, 0.946f, 0.553f)
		);

		create_instance_with_rot(
			wedge,
			glm::vec3(-30.f, 15.0f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			180.f,
			glm::vec3(45.f, 30.f, 15.f),
			glm::vec3(0.518f, 0.714f, 0.553f)
		);

		create_instance_with_rot(
			wedge,
			glm::vec3(0.f, 15.0f, 30.f),
			glm::vec3(0.f, 1.f, 0.f),
			270.f,
			glm::vec3(45.f, 30.f, 15.f),
			glm::vec3(0.420f, 0.197f, 0.487f)
		);

		create_instance_with(
			cylinder_lq,
			glm::vec3(-50.f, 70.f, 50.f),
			glm::vec3(15.f, 25.f, 15.f),
			glm::vec3(0.977f, 0.914f, 0.600f)
		);

		create_instance_with(
			cylinder,
			glm::vec3(50.f, 70.f, 50.f),
			glm::vec3(15.f, 25.f, 15.f),
			glm::vec3(0.761f, 0.855f, 0.722f)
		);

		create_instance_with(
			cylinder_hq,
			glm::vec3(50.f, 70.f, -50.f),
			glm::vec3(15.f, 25.f, 15.f),
			glm::vec3(0.502f, 0.734f, 0.859f)
		);

		create_instance_with(
			icosphere_ulq, 
			glm::vec3(100.f, 70.f, 0.f), 
			glm::vec3(50.f, 50.f, 50.f), 
			glm::vec3(0.769f, 0.157f, 0.110f)
		);
		create_instance_with(
			icosphere_lq,
			glm::vec3(0.f, 70.f, 100.f),
			glm::vec3(50.f, 50.f, 50.f),
			glm::vec3(0.769f, 0.157f, 0.110f)
		);

		create_instance_with(
			icosphere,
			glm::vec3(-100.f, 70.f, 0.f),
			glm::vec3(50.f, 50.f, 50.f),
			glm::vec3(0.769f, 0.157f, 0.110f)
		);

		create_instance_with(
			icosphere_hq,
			glm::vec3(0.f, 70.f, -100.f),
			glm::vec3(50.f, 50.f, 50.f),
			glm::vec3(0.769f, 0.157f, 0.110f)
		);

		create_instance_with(
			corner_wedge_inner,
			glm::vec3(-50.f, 70.f, -50.f),
			glm::vec3(25.f, 15.f, 50.f),
			glm::vec3(0.800f, 0.557f, 0.412f)
		);

		create_instance_with(
			corner_wedge_outer,
			glm::vec3(0.f, 70.f, 0.f),
			glm::vec3(25.f, 15.f, 50.f),
			glm::vec3(0.632f, 0.769f, 0.550f)
		);

		create_instance_with_rot(
			cornell_box,
			glm::vec3(120.f, 30.f, 60.f),
			glm::vec3(0.f, 1.f, 0.f),
			-90.f,
			glm::vec3(10.f, 10.f, 10.f),
			glm::vec3(0.772f)
		);

		self.animate_heart = create_instance_with_rot(
			heart,
			glm::vec3(-20.f, 80.f, -80.f),
			glm::vec3(-1.f, 0.f, 0.f),
			90.f,
			glm::vec3(5.0f),
			glm::vec3(0.9f, 0.f, 0.f)
		);

		glm::vec3 base = glm::vec3(120.f, 30.f, -80.f);

		//top
		create_instance_with(
			box,
			base + glm::vec3(0.f, -25.f, 0.f),
			glm::vec3(102.f, 5.f, 102.f),
			glm::vec3(0.772f)
		);
		// sides
		create_instance_with(
			box,
			base + glm::vec3(0.f, 0.f, 50.f),
			glm::vec3(100.f, 50.f, 5.f),
			glm::vec3(0.772f)
		);

		create_instance_with(
			box,
			base + glm::vec3(0.f, 0.f, -50.f),
			glm::vec3(100.f, 50.f, 5.f),
			glm::vec3(0.772f)
		);

		create_instance_with(
			box,
			base + glm::vec3(50.f, 0.f, 0.f),
			glm::vec3(5.f, 50.f, 100.f),
			glm::vec3(0.772f)
		);

		create_instance_with(
			box,
			base + glm::vec3(-50.f, 0.f, 0.f),
			glm::vec3(5.f, 50.f, 100.f),
			glm::vec3(0.772f)
		);
		//bottom
		create_instance_with(
			box,
			base + glm::vec3(0.f, 25.f, 0.f),
			glm::vec3(102.f, 5.f, 102.f),
			glm::vec3(0.772f)
		);

		create_instance_with(
			cylinder,
			glm::vec3(-125.f, 75.f, -150.f),
			glm::vec3(1.f),
			glm::vec3(1.0f, 0.5f, 0.25f)
		);


		// lights

		
		self.sun = self.light_manager->add_light(Light::New(
			LightType::Directional,
			glm::vec3(-0.5f, -1.0f, -0.5f),
			glm::vec3(0.f),
			glm::vec3(1.0f, 0.941f, 0.914f),
			8000.f,
			10.f,
			12.f
		));

		
		self.spot_1 = self.light_manager->add_light(Light::New(
			LightType::Spot,
			glm::vec3(-0.5f, -1.0f, -0.5f),
			glm::vec3(0.f),
			glm::vec3(1.0f, 0.96f, 0.61f),
			1500.f,
			15.f,
			18.f
		));
		self.spot_2 = self.light_manager->add_light(Light::New(
			LightType::Spot,
			glm::vec3(0.f),
			glm::vec3(2.f, 3.f, 2.f),
			glm::vec3(0.61f, 0.96f, 1.0f),
			1500.f,
			15.f,
			18.f
		));

		self.spot_3 = self.light_manager->add_light(Light::New(
			LightType::Spot,
			glm::vec3(0.f),
			glm::vec3(2.f, 3.f, 2.f),
			glm::vec3(1.f, 1.f, 1.0f),
			800.f,
			25.f,
			35.f
		));

		self.point_1 = self.light_manager->add_light(Light::New(
			LightType::Positional,
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(1.f, 0.847f, 0.725f),
			1500.f
		));

		self.point_2 = self.light_manager->add_light(Light::New(
			LightType::Positional,
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(0.725f, 0.847f, 1.f),
			1500.f
		));
	}

	Light* get_light_from_index(int i) const {
		switch (i) {
			case 0: return self.spot_1;
			case 1: return self.spot_2;
			case 2: return self.spot_3;
			case 3: return self.point_1;
			case 4: return self.point_2;
			default: return nullptr;
		}
	}

	Instance* get_instance_from_index(int i) const {
		if (i < 0) { return nullptr; }
		if (i >= self.instances.size()) { return nullptr; }
		return self.instances[i].get();
	}

	void apply_instance_transform(Instance* inst, glm::vec2 arrow_vec) {
		if (!inst) { return; }
		glm::mat4 model = inst->get_frame();
		model[3] = model[3] + glm::vec4(arrow_vec.x, 0.f, arrow_vec.y, 0.f);
		inst->set_frame(model);
	}

	void apply_light_transform(Light* light) {
		if (!light) { return; }
		light->set_pos_dir(self.camera->get_position(), self.camera->get_front());
	}
	
public:
	static std::unique_ptr<GameMap> New(
		Camera* camera, LightManager* light_manager, Movement* movement
	) {
		auto map = std::unique_ptr<GameMap>(new GameMap());
		auto& self = map->self;

		self.camera = camera;
		self.light_manager = light_manager;
		self.movement = movement;

		map->create_meshes();
		map->setup_map();

		return map;
	}

	void update(double dt) {
		auto& light_manager = self.light_manager;
		auto& camera = self.camera;
		auto& movement = self.movement;

		self.orbit_angle = self.orbit_angle + self.rotspeed * (float) dt;
		if (self.orbit_angle >= glm::two_pi<float>()) {
			self.orbit_angle -= glm::two_pi<float>();
		} else if (self.orbit_angle < 0.0f) {
			self.orbit_angle += glm::two_pi<float>();
		}

		glm::vec3 rotlightdir = glm::vec3(0.f);
		rotlightdir.y = -std::sin(self.tilt);
		float xzprojmat = std::cos(self.tilt);
		rotlightdir.x = xzprojmat * std::cos(self.orbit_angle);
		rotlightdir.z = xzprojmat * std::sin(self.orbit_angle);

		self.sun->set_dir(rotlightdir);

		int i = movement->get_selected_item();
		glm::vec2 arrow_vec = movement->get_arrow_vec();

		Instance* inst = get_instance_from_index(i);
		apply_instance_transform(inst, arrow_vec);

		if (movement->get_left_mouse_button_down()) {
			Light* light = get_light_from_index(i);
			apply_light_transform(light);
		}

		self.time += dt;

		float size = glm::cos(self.time * 6.0f) * 0.125f + 2.5f;
		self.animate_heart->set_size(glm::vec3(size));

		float offset = glm::sin(self.time * 2.0f - glm::radians(30.f)) * 20.0f;

		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(-70.f, 10.f, -80.f) + glm::vec3(offset, 0.f, 0.f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));

		self.animate_heart->set_frame(model);
		
		/*
		if (self.movement->get_update_light()) {
			auto cam_pos = camera->get_position();
			auto cam_front = camera->get_front();
			auto cam_right = camera->get_right();

			auto sep = 8.f;
			//self.spot_1->set_pos_dir(cam_pos + cam_right * -sep, cam_front);
			//self.spot_2->set_pos_dir(cam_pos + cam_right * sep, cam_front);
			self.point_1->set_pos(cam_pos);
		}*/


	}

	void draw() {
		for (const auto& [k, v] : self.meshes) {
			v->draw();
		}
	}
};