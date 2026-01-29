#pragma once

struct Vertex {
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

using Vertices = std::vector<Vertex>;
using Indices = std::vector<GLint>;

#include "file_obj.hpp" // load objs from file
#include "quad.hpp"
#include "box.hpp"
#include "icosphere.hpp"
#include "cylinder.hpp"
#include "wedge.hpp"
#include "corner_wedge_outer.hpp"
#include "corner_wedge_inner.hpp"


class InstantiableMesh;
class Instance;

struct InstanceData {
	glm::mat4 model = glm::mat4(1.f);
	glm::vec4 color = glm::vec4(1.f);
};
	
class Instance {
private:
	struct Self {
		InstantiableMesh* object = nullptr;
		int64_t index = -1;

		std::string name = "";

		glm::mat4 frame = glm::mat4(1.f);
		glm::vec3 size = glm::vec3(1.f);
		glm::vec4 rgba = glm::vec4(1.f);
	} self;
		
	Instance() = default;

	void initialize(InstantiableMesh* object, int64_t index) {
		self.object = object;
		self.index = index;
		update_object();
	}

public:
	void update_object();
	~Instance();

	static std::unique_ptr<Instance> New(InstantiableMesh* object, int64_t index) {
		auto instance = std::unique_ptr<Instance>(new Instance());
		instance->initialize(object, index);
		return instance;
	}

	const std::string& get_name_ref() const {
		return self.name;
	}

	std::string get_name() const {
		return self.name;
	}

	void set_name(std::string&& new_name) {
		self.name = new_name;
	}

	bool name_is(const std::string& comp_name) const {
		return self.name == comp_name;
	}

	glm::vec3 get_size() const {
		return self.size;
	}

	void set_size(glm::vec3 size) {
		self.size = glm::max(size, 0.f);
		update_object();
	}

	glm::mat4 get_frame() const {
		return self.frame;
	}

	void set_frame(glm::mat4 frame) {
		self.frame = frame;
		update_object();
	}

	glm::vec4 get_color() const {
		return self.rgba;
	}

	void set_color(glm::vec4 rgba) {
		self.rgba = glm::clamp(rgba, 0.f, 1.f);
		update_object();
	}

	bool is_transparent() const {
		return self.rgba.w < 1.f;
	}
};


class InstantiableMesh {
public:
	using Shape_Creator = std::function<void(Vertices&, Indices&)>;
private:
	struct Mesh {
		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ebo = 0;

		Vertices vertices;
		Indices indices;

		Mesh() = default;
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
	private:
		void move(Mesh& other) {
			vao = other.vao;
			vbo = other.vbo;
			ebo = other.ebo;
			vertices = std::move(other.vertices);
			indices = std::move(other.indices);
			other.vao = 0;
			other.vbo = 0;
			other.ebo = 0;
		}

		void cleanup() {
			if (vao) glDeleteVertexArrays(1, &vao);
			if (vbo) glDeleteBuffers(1, &vbo);
			if (ebo) glDeleteBuffers(1, &ebo);
			vao = 0;
			vbo = 0;
			ebo = 0;
		}
	public:
		Mesh(Mesh&& other) noexcept { move(other); };
		Mesh& operator=(Mesh&& other) noexcept {
			if (this != &other) {
				cleanup();
				move(other);
			}
			return *this;
		};

		~Mesh() { cleanup(); }
	};

	struct Self {
		Mesh mesh;

		GLsizei indices = 0;
		GLenum index_type = GL_UNSIGNED_INT;
		GLenum draw_mode = GL_TRIANGLES;

		std::vector<InstanceData> instances;
		GLuint vbo_instances = 0;
		size_t vbo_capacity = 0;

		std::set<int64_t> free_indices;
		std::set<int64_t> dirty_instances;

		Self() = default;
		Self(const Self&) = delete;
		Self& operator=(const Self&) = delete;
	private:
		void move(Self& other) {
			mesh = std::move(other.mesh);
			indices = other.indices;
			index_type = other.index_type;
			draw_mode = other.draw_mode;
			vbo_instances = other.vbo_instances;
			vbo_capacity = other.vbo_capacity;
			instances = std::move(other.instances);
			free_indices = std::move(other.free_indices);
			dirty_instances = std::move(other.dirty_instances);
		}

		void cleanup() {
			if (vbo_instances) { glDeleteBuffers(1, &vbo_instances); }
			vbo_instances = 0;
		}
	public:
		Self(Self&& other) noexcept { move(other); };
		Self& operator=(Self&& other) noexcept {
			if (this != &other) {
				cleanup();
				move(other);
			}
			return *this;
		};
	} self;

	InstantiableMesh() = default;

	static inline void setup_attribute(
		GLuint index, GLint size, GLsizei stride, const void* offset
	) {
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(index);
	}

	static inline void setup_divattr(
		GLuint index, GLint size, GLsizei stride, const void* offset
	) {
		setup_attribute(index, size, stride, offset);
		glVertexAttribDivisor(index, 1);
	}

	void setup_buffers() {
		auto& mesh = self.mesh;
		glGenVertexArrays(1, &(mesh.vao));
		glGenBuffers(1, &(mesh.vbo));
		glGenBuffers(1, &(mesh.ebo));
		glGenBuffers(1, &self.vbo_instances);
			
		glBindVertexArray(self.mesh.vao);
			
		auto& verts = self.mesh.vertices;
		glBindBuffer(GL_ARRAY_BUFFER, self.mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

		auto& indis = self.mesh.indices;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,indis.size() * sizeof(GLint), indis.data(), GL_STATIC_DRAW);

		GLsizei stride = sizeof(Vertex);
		
		setup_attribute(0, 3, stride, (void*) offsetof(Vertex, pos));
		setup_attribute(1, 3, stride, (void*) offsetof(Vertex, nor));
		setup_attribute(2, 2, stride, (void*) offsetof(Vertex, tex));

		glBindBuffer(GL_ARRAY_BUFFER, self.vbo_instances);
		glBufferData(
			GL_ARRAY_BUFFER,
			self.vbo_capacity * sizeof(InstanceData),
			self.instances.data(),
			GL_DYNAMIC_DRAW
		);
		GLsizei inst_s = sizeof(InstanceData);
		GLsizei vec4_s = sizeof(glm::vec4);

		setup_divattr(3, 4, inst_s, (void*) (offsetof(InstanceData, model) + 0 * vec4_s));
		setup_divattr(4, 4, inst_s, (void*) (offsetof(InstanceData, model) + 1 * vec4_s));
		setup_divattr(5, 4, inst_s, (void*) (offsetof(InstanceData, model) + 2 * vec4_s));
		setup_divattr(6, 4, inst_s, (void*) (offsetof(InstanceData, model) + 3 * vec4_s));
		setup_divattr(7, 4, inst_s, (void*) (offsetof(InstanceData, color)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void prepare_instance_vbo() {
		if (self.instances.empty()) {
			if (self.vbo_capacity > 0) {
				glBindBuffer(GL_ARRAY_BUFFER, self.vbo_instances);
				glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				self.vbo_capacity = 0;
			}
			self.dirty_instances.clear();
			return;
		}

		if (self.dirty_instances.empty() && self.instances.size() == self.vbo_capacity) {
			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, self.vbo_instances);

		if (
			self.instances.size() > self.vbo_capacity
			|| self.vbo_capacity == 0
			|| self.instances.size() < self.vbo_capacity / 2
		) {
			glBufferData(
				GL_ARRAY_BUFFER,
				self.instances.size() * sizeof(InstanceData),
				self.instances.data(), 
				GL_DYNAMIC_DRAW
			);
			self.vbo_capacity = self.instances.size();
			self.dirty_instances.clear();
		} else {
			for (auto i : self.dirty_instances) {
				glBufferSubData(
					GL_ARRAY_BUFFER, i * sizeof(InstanceData),
					sizeof(InstanceData),
					&self.instances[i]
				);
			}
			self.dirty_instances.clear();
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void initialize(GLenum draw_mode) {
		self.indices = static_cast<GLsizei>(self.mesh.indices.size());
		self.index_type = GL_UNSIGNED_INT;
		self.draw_mode = draw_mode;
		setup_buffers();
	}

public:
	~InstantiableMesh() {
		if (self.vbo_instances) { glDeleteBuffers(1, &self.vbo_instances); }
	}
	InstantiableMesh(const InstantiableMesh&) = delete;
	InstantiableMesh& operator=(const InstantiableMesh&) = delete;
	InstantiableMesh(InstantiableMesh&& other) = default;
	InstantiableMesh& operator=(InstantiableMesh&& other) = default;

	static std::unique_ptr<InstantiableMesh> FromShape(
		Shape_Creator shape_func, GLenum draw_mode = GL_TRIANGLES
	) {
		auto obj = std::unique_ptr<InstantiableMesh>(new InstantiableMesh());
		auto& self = obj->self;

		shape_func(self.mesh.vertices, self.mesh.indices);
		obj->initialize(draw_mode);

		return obj;
	}

	static std::unique_ptr<InstantiableMesh> FromFile(
		std::string filename, 
		GLenum draw_mode = GL_TRIANGLES
	) {
		auto obj = std::unique_ptr<InstantiableMesh>(new InstantiableMesh());
		auto& self = obj->self;

		std::string base_dir = "objs/" + filename + "/";
		FileObj::Load(base_dir, filename, self.mesh.vertices, self.mesh.indices);
		obj->initialize(draw_mode);

		return obj;
	}

	std::unique_ptr<Instance> instance() {
		int64_t index;
		if (!self.free_indices.empty()) {
			auto it = self.free_indices.begin();
			index = *it;
			self.free_indices.erase(it);
			self.instances[index] = InstanceData {};
		} else {
			self.instances.push_back(InstanceData {});
			index = self.instances.size() - 1;
		}

		self.dirty_instances.insert(index);
		auto instance = Instance::New(this, index);

		return instance;
	}

	void update_instance(int64_t index, glm::mat4& frame, glm::vec3& size, glm::vec4& color) {
		if (index < 0 || index >= static_cast<int64_t>(self.instances.size())) {
			return;
		}

		glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), size);
		self.instances[index].model = frame * scale_matrix;
		self.instances[index].color = color;

		self.dirty_instances.insert(index);
	}

	void release_instance(int64_t index) {
		if (index < 0 || index >= static_cast<int>(self.instances.size())) {
			return;
		}

		self.instances[index].model = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f));
		self.instances[index].color.w = 0.0f;

		self.dirty_instances.insert(index);
		self.free_indices.insert(index);
	}

	void draw() {
		if (self.mesh.vertices.empty() || self.mesh.indices.empty() || self.instances.empty()) { return; }

		prepare_instance_vbo();
		glBindVertexArray(self.mesh.vao);
		glDrawElementsInstanced(
			self.draw_mode,
			self.indices,
			self.index_type,
			(void*) 0,
			static_cast<GLsizei>(self.instances.size())
		);
		glBindVertexArray(0);
	}
};

void Instance::update_object() {
	self.object->update_instance(self.index, self.frame, self.size, self.rgba);
}

Instance::~Instance() {
	if (self.object && self.index != -1) {
		self.object->release_instance(self.index);
	}
}


