#pragma once

template<typename VertexType>
class Mesh {
private:
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLsizei m_vertex_count = 0;
private:
	void cleanup() {
		if (m_vao != 0) {
			glDeleteVertexArrays(1, &m_vao);
			m_vao = 0;
		}
		if (m_vbo != 0) {
			glDeleteBuffers(1, &m_vbo);
			m_vbo = 0;
		}
		m_vertex_count = 0;
	}

	inline void setup_attribute(GLuint index, GLint size, GLsizei stride, const void* offset) {
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(index);
	}

	void setup_vertex_attributes();
public:
	Mesh() {};

	Mesh(const std::vector<VertexType>& vertices) {
		if (vertices.empty()) { return; }

		m_vertex_count = (GLsizei) (vertices.size());

		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(
			m_vbo, vertices.size() * sizeof(VertexType), vertices.data(), 0
		);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		setup_vertex_attributes();

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	~Mesh() { cleanup(); }

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept :
		m_vao(other.m_vao),
		m_vbo(other.m_vbo),
		m_vertex_count(other.m_vertex_count) {
		other.m_vao = 0;
		other.m_vbo = 0;
		other.m_vertex_count = 0;
	}

	Mesh& operator=(Mesh&& other) noexcept {
		if (this != &other) {
			cleanup();

			m_vao = other.m_vao;
			m_vbo = other.m_vbo;
			m_vertex_count = other.m_vertex_count;

			other.m_vao = 0;
			other.m_vbo = 0;
			other.m_vertex_count = 0;
		}
		return *this;
	}

	void draw() const {
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
		glBindVertexArray(0);
	}
};


template<>
inline void Mesh<vert_col>::setup_vertex_attributes() {
	GLsizei stride = sizeof(vert_col);
	setup_attribute(0, 3, stride, (void*) offsetof(vert_col, pos));
	setup_attribute(1, 3, stride, (void*) offsetof(vert_col, nor));
	setup_attribute(2, 3, stride, (void*) offsetof(vert_col, col));
}

template<>
inline void Mesh<vert_tex>::setup_vertex_attributes() {
	GLsizei stride = sizeof(vert_tex);
	setup_attribute(0, 3, stride, (void*) offsetof(vert_tex, pos));
	setup_attribute(1, 3, stride, (void*) offsetof(vert_tex, nor));
	setup_attribute(2, 2, stride, (void*) offsetof(vert_tex, tex));
}

