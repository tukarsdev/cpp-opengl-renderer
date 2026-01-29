#pragma once

#include "error.hpp"

class WindowManager {
public:
	using Key = int;
	using KeyInputFunction = std::function<void(Key, int)>;
	using MouseInputFunction = std::function<void(double, double)>;
private:
	struct Self {
		GLFWwindow* window = nullptr;

		KeyInputFunction keyboard_input_callback;
		KeyInputFunction mouse_button_callback;
		MouseInputFunction mouse_move_callback;
		MouseInputFunction mouse_scroll_callback;

		int width = 1280;
		int height = 720;

		bool is_mouse_locked = false;
	} self;

	WindowManager() = default;

private:
	static WindowManager* get_wm(GLFWwindow* win) {
		return static_cast<WindowManager*>(
			glfwGetWindowUserPointer(win)
		);
	}

	static void size_callback(GLFWwindow* win, int w, int h) {
		glViewport(0, 0, w, h);
		auto wm = get_wm(win);
		if (wm) {
			wm->self.width = w;
			wm->self.height = h;
		}
	}

public:
	static std::unique_ptr<WindowManager> New(std::string name) {
		auto wm_ptr = std::unique_ptr<WindowManager>(new WindowManager());
		auto& self = wm_ptr->self;
		
		if (!glfwInit()) {
			std::cerr << "GLFW init failure\n";
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		self.window = glfwCreateWindow(
			self.width, self.height, name.c_str(), NULL, NULL
		);

		if (!self.window) {
			std::cerr << "Failed to create GLFW window\n";
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(self.window);
		glfwSetWindowUserPointer(wm_ptr->self.window, wm_ptr.get());
		glfwSetWindowSizeCallback(self.window, size_callback);
		
		if (gl3wInit() != 0) {
		     std::cerr << "ERROR: Could not initialize GL loader\n";
		     exit(EXIT_FAILURE);
		}

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugCallback, 0);

		wm_ptr->set_mouse_locked(self.is_mouse_locked); 

		glfwGetFramebufferSize(self.window, &self.width, &self.height);
		glViewport(0, 0, self.width, self.height);

		return wm_ptr;
	};

	~WindowManager() {
		if (self.window) {
			glfwDestroyWindow(self.window);
			self.window = nullptr;
		}
		glfwTerminate();
	}

	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;
	WindowManager(WindowManager&& other) = delete;
	WindowManager& operator=(WindowManager&& other) = delete;

	// ONLY TO BE USED BY InputManager
	void set_keyboard_input_callback(KeyInputFunction keyboard_input_callback) {
		self.keyboard_input_callback = keyboard_input_callback;
		glfwSetKeyCallback(
			self.window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
			auto wm = get_wm(win);
			wm->self.keyboard_input_callback(key, action);
		});
	}

	// ONLY TO BE USED BY InputManager
	void set_mouse_button_callback(KeyInputFunction mouse_button_callback) {
		self.mouse_button_callback = mouse_button_callback;
		glfwSetMouseButtonCallback(
			self.window, [](GLFWwindow* win, int button, int action, int mods) {
			auto wm = get_wm(win);
			wm->self.mouse_button_callback(button, action);
		});
	}

	// ONLY TO BE USED BY InputManager
	void set_mouse_move_callback(MouseInputFunction mouse_move_callback) {
		self.mouse_move_callback = mouse_move_callback;
		glfwSetCursorPosCallback(
			self.window, [](GLFWwindow* win, double x, double y) {
			auto wm = get_wm(win);
			wm->self.mouse_move_callback(x, y);
		});
	}

	

	// ONLY TO BE USED BY InputManager
	void set_mouse_scroll_callback(MouseInputFunction mouse_scroll_callback) {
		self.mouse_scroll_callback = mouse_scroll_callback;
		glfwSetScrollCallback(
			self.window, [](GLFWwindow* win, double x, double y) {
			auto wm = get_wm(win);
			wm->self.mouse_scroll_callback(x, y);
		});
	}

	glm::dvec2 get_mouse_pos() const {
		double x, y;
		glfwGetCursorPos(self.window, &x, &y);
		return glm::dvec2(x, y);
	}

	// ONLY TO BE USED BY InputManager
	void set_mouse_locked(bool locked) {
		glfwSetInputMode(
			self.window, GLFW_CURSOR,
			locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
		);
		self.is_mouse_locked = locked;
	}

	// ONLY TO BE USED BY InputManager
	bool get_mouse_locked() const {
		return self.is_mouse_locked;
	}

	void poll() const {
		glfwPollEvents();
	}

	float get_aspect_ratio() const {
		if (self.height == 0) return 0.0f;
		return static_cast<float>(self.width) / self.height;
	}

	glm::ivec2 get_resolution() const {
		return glm::ivec2(self.width, self.height);
	}

	bool should_close() const {
		if (!self.window) return true;
		return glfwWindowShouldClose(self.window);
	}

	void swap_buffers() const {
		if (self.window) {
			glfwSwapBuffers(self.window);
		}
	}

	void set_close() {
		glfwSetWindowShouldClose(self.window, true);
	}
};

