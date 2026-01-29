#pragma once

class Movement {
public:
	using KeyMap = std::unordered_map<Key, int>;
private:
	struct Self {
		InputManager* input;

		KeyMap movement_keys;
		KeyMap arrow_keys;
		KeyMap zoom_keys;
		KeyMap y_axis_keys;

		glm::vec2 movement_vec = glm::vec2(0.f);
		glm::vec2 arrow_vec = glm::vec2(0.f);
		glm::vec1 zoom_vec = glm::vec1(0.f);
		glm::vec1 y_axis_vec = glm::vec1(0.f);

		int current_selected = -1;

		bool left_mouse_button_down = false;
		bool right_mouse_button_down = false;
		bool change_locked = false;
	} self;

	Movement() = default;

private:
	void movement(const std::string& action, InputState input_state, Key key) {
		auto& movement_keys = self.movement_keys;
		movement_keys[key] = (input_state != InputState::End);
		auto movement_vec_not_norm = glm::vec2(
			movement_keys[GLFW_KEY_D] - movement_keys[GLFW_KEY_A],
			movement_keys[GLFW_KEY_W] - movement_keys[GLFW_KEY_S]
		);

		if (glm::length(movement_vec_not_norm) > 0.f) {
			self.movement_vec = glm::normalize(movement_vec_not_norm);
		} else {
			self.movement_vec = movement_vec_not_norm;
		}
	}

	void arrows(const std::string& action, InputState input_state, Key key) {
		auto& arrow_keys = self.arrow_keys;
		arrow_keys[key] = (input_state != InputState::End);
		auto arrow_vec_not_norm = glm::vec2(
			arrow_keys[GLFW_KEY_RIGHT] - arrow_keys[GLFW_KEY_LEFT],
			arrow_keys[GLFW_KEY_UP] - arrow_keys[GLFW_KEY_DOWN]
		);

		if (glm::length(arrow_vec_not_norm) > 0.f) {
			self.arrow_vec = glm::normalize(arrow_vec_not_norm);
		} else {
			self.arrow_vec = arrow_vec_not_norm;
		}
	}

	void select_item(const std::string& action, InputState input_state, Key key) {
		if (input_state != InputState::Begin) { return; }
		switch (key) {
			case GLFW_KEY_1: self.current_selected = 0; break;
			case GLFW_KEY_2: self.current_selected = 1; break;
			case GLFW_KEY_3: self.current_selected = 2; break;
			case GLFW_KEY_4: self.current_selected = 3; break;
			case GLFW_KEY_5: self.current_selected = 4; break;
			case GLFW_KEY_6: self.current_selected = 5; break;
			case GLFW_KEY_7: self.current_selected = 6; break;
			case GLFW_KEY_8: self.current_selected = 7; break;
			case GLFW_KEY_9: self.current_selected = 8; break;
			case GLFW_KEY_0: self.current_selected = -1; break;
		}
	}

	void zoom(const std::string& action, InputState input_state, Key key) {
		auto& zoom_keys = self.zoom_keys;
		zoom_keys[key] = (input_state != InputState::End);
		self.zoom_vec = glm::vec1((float) (zoom_keys[GLFW_KEY_F] - zoom_keys[GLFW_KEY_R]));
	}

	void update_y_axis(const std::string& action, InputState input_state, Key key) {
		auto& y_axis_keys = self.y_axis_keys;
		if (key == GLFW_KEY_C) { key = GLFW_KEY_LEFT_CONTROL; }
		y_axis_keys[key] = (input_state != InputState::End);

		self.y_axis_vec = glm::vec1((float) (y_axis_keys[GLFW_KEY_SPACE] - y_axis_keys[GLFW_KEY_LEFT_CONTROL]));
	}

	void left_mouse_button(const std::string& action, InputState input_state, Key key) {
		self.left_mouse_button_down = (input_state != InputState::End);
	}

	void right_mouse_button(const std::string& action, InputState input_state, Key key) {
		self.right_mouse_button_down = (input_state != InputState::End);
	}

	void handle_locked(const std::string& action, InputState input_state, Key key) {
		self.change_locked = (input_state == InputState::Begin);
	}

	void bind_actions() {
		self.input->bind_action("movement",
			[this](const std::string& action, InputState input_state, Key key) { 
				movement(action, input_state, key); 
			}, GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D
		);
		self.input->bind_action("arrows",
			[this](const std::string& action, InputState input_state, Key key) {
				arrows(action, input_state, key);
			}, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT
		);
		self.input->bind_action("select item",
			[this](const std::string& action, InputState input_state, Key key) {
				select_item(action, input_state, key);
			}, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, 
			GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9, GLFW_KEY_0
		);

		self.input->bind_action("zoom camera",
			[this](const std::string& action, InputState input_state, Key key) { 
				zoom(action, input_state, key); 
			}, GLFW_KEY_R, GLFW_KEY_F
		);
		self.input->bind_action("update y axis",
			[this](const std::string& action, InputState input_state, Key key) { 
				update_y_axis(action, input_state, key); 
			}, GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_C
		);
		self.input->bind_action("left mouse button",
			[this](const std::string& action, InputState input_state, Key key) { 
				left_mouse_button(action, input_state, key); 
			}, GLFW_MOUSE_BUTTON_1
		);
		self.input->bind_action("right mouse button",
			[this](const std::string& action, InputState input_state, Key key) {
				right_mouse_button(action, input_state, key);
			}, GLFW_MOUSE_BUTTON_2
		);
		self.input->bind_action("handle locked",
			[this](const std::string& action, InputState input_state, Key key) { 
				handle_locked(action, input_state, key); 
			}, GLFW_KEY_ESCAPE
		);

		self.input->set_mouse_locked(true);
	}

public:
	static std::unique_ptr<Movement> New(InputManager* input) {
		auto movement = std::unique_ptr<Movement>(new Movement());
		auto& self = movement->self;

		self.input = input;

		movement->bind_actions();

		return movement;
	}

	glm::vec2 get_movement_vec() const {
		return self.movement_vec;
	}

	glm::vec2 get_arrow_vec() const {
		return self.arrow_vec;
	}

	int get_selected_item() const {
		return self.current_selected;
	}

	glm::vec1 get_zoom_vec() const {
		return self.zoom_vec;
	}

	glm::vec1 get_y_axis_vec() const {
		return self.y_axis_vec;
	}

	bool get_left_mouse_button_down() const {
		return self.left_mouse_button_down;
	}

	bool get_right_mouse_button_down() const {
		return self.right_mouse_button_down;
	}

	bool get_change_locked() const {
		return self.change_locked;
	}

	void set_change_locked(bool x) {
		self.change_locked = x;
	}
};