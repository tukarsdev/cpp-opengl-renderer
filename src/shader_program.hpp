#pragma once

#include "file.hpp"

class ShaderProgram {
private:
	struct Self {
		GLuint pid = 0;
		GLuint vertex = 0;
		GLuint fragment = 0;
	} self;
	ShaderProgram() = default;

private:
	static GLint gl_log(GLuint id, bool is_shader) {
		GLint success;
		is_shader ? glGetShaderiv(id, GL_COMPILE_STATUS, &success)
			: glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (success) { return success; }
		
		GLint logLength = 0;
		is_shader ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength)
			: glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		std::string infoLogStr;
		if (logLength > 0) {
			std::vector<char> infoLog(logLength);
			is_shader ? glGetShaderInfoLog(id, logLength, NULL, infoLog.data())
				: glGetProgramInfoLog(id, logLength, NULL, infoLog.data());
			infoLogStr = infoLog.data();
		} else {
			infoLogStr = "No details available.";
		}
		std::cout << infoLogStr;
		if (is_shader) { glDeleteShader(id); }
		
		return success;
	}

	static GLuint compile(const std::string& filename, GLenum shader_type, GLuint pid) {
		auto shader_source_opt = read_file(filename);
		if (!shader_source_opt.has_value()) { return 0; }
		const char* shader_source = shader_source_opt.value().c_str();

		GLuint sid = glCreateShader(shader_type);
		if (!sid) { return 0; }

		glShaderSource(sid, 1, &shader_source, NULL);
		glCompileShader(sid);

		GLint success = gl_log(sid, true);
		if (!success) { 
			glDeleteShader(sid);
			return 0; 
		}

		glAttachShader(pid, sid);

		return sid;
	}

	static void detach_and_delete_shaders(Self& self) {
		if (self.pid == 0) { return; }
		if (self.vertex) {
			glDetachShader(self.pid, self.vertex);
			glDeleteShader(self.vertex);
			self.vertex = 0;
		}
		if (self.fragment) {
			glDetachShader(self.pid, self.fragment);
			glDeleteShader(self.fragment);
			self.fragment = 0;
		}
	}



public:
	~ShaderProgram() {
		detach_and_delete_shaders(self);
		if (self.pid != 0) { glDeleteProgram(self.pid); }
	}

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& other) = delete;
	ShaderProgram& operator=(ShaderProgram&& other) = delete;

	static std::optional<std::unique_ptr<ShaderProgram>> 
	New(std::string vertex_file, std::string fragment_file) 
	{
		auto shader = std::unique_ptr<ShaderProgram>(new ShaderProgram());
		auto& self = shader->self;

		self.pid = glCreateProgram();
		if (!self.pid) {
			std::cerr << "Failed to create program!\n";
			return std::nullopt;
		}

		self.vertex = compile(vertex_file, GL_VERTEX_SHADER, self.pid);
		self.fragment = compile(fragment_file, GL_FRAGMENT_SHADER, self.pid);

		if (!(self.vertex && self.fragment)) {
			detach_and_delete_shaders(self);
			return std::nullopt;
		}

		glLinkProgram(self.pid);
		GLint success = gl_log(self.pid, false);
		if (!success) {
			std::cout << "Linking error occurred. (pid = " << self.pid <<")\n";
			std::cout << "Error happened with input shaders:"
				<< "\n\t  Vertex: " << vertex_file
				<< "\n\tFragment: " << fragment_file
				<< "\n";
			return std::nullopt;
		}

		detach_and_delete_shaders(self);

		return shader;
	}

	bool is_valid() const {
		return self.pid != 0;
	}

	inline void use() const {
		glUseProgram(self.pid);
	}

	inline GLint location(std::string name) const {
		return glGetUniformLocation(self.pid, name.c_str());
	}

	inline void uniform(GLint location, const glm::vec3& vec) const {
		glUniform3f(location, vec.x, vec.y, vec.z);
	};

	inline void uniform(GLint location, const glm::mat4 matrix) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	inline void uniform(GLint location, const GLint x) const {
		glUniform1i(location, x);
	}

	inline void uniform(GLint location, const float x) const {
		glUniform1f(location, x);
	}
};

