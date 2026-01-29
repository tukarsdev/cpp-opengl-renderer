#pragma once

void APIENTRY DebugCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam
) {
	if (type == GL_DEBUG_TYPE_ERROR) {
		std::cerr << "DebugCallback: ERROR:\n"
			<< "Message: " << message << "\n";
		throw std::runtime_error("");
	}
}

