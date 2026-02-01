#ifdef __linux__
#include <unistd.h>
#include <linux/limits.h>
#endif

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>

#include "window_manager.hpp"
#include "input_manager.hpp"
#include "scene.hpp"
#include "texture.hpp"

std::filesystem::path get_executable_path() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#else
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    return std::filesystem::path(std::string(buffer, (count > 0) ? count : 0)).parent_path();
#endif
}

int set_working_path() {
	try {
		std::filesystem::path exe_dir = get_executable_path();
		std::filesystem::current_path(exe_dir); 
		std::cout << "Working directory set to: " << std::filesystem::current_path() << std::endl;
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}

int main(int argc, char** argv) {
	set_working_path();
	
	auto wm = WindowManager::New("renderer");
	auto input = InputManager::New(wm.get());
	auto scene_opt = Scene::New(wm.get(), input.get());
	
	if (!scene_opt.has_value()) {
		std::cerr << "Failed to create scene\n";
		exit(EXIT_FAILURE);
	}
	auto& scene = scene_opt.value();

	double last_frame_time = glfwGetTime();
	double dt = 0.0f;

	while (!wm->should_close()) {
		double current_frame_time = glfwGetTime();
		dt = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;

		input->poll();
		scene->render(dt);
		wm->swap_buffers();
	}

	return EXIT_SUCCESS;
}

