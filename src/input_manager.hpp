#pragma once

enum InputState {
	End = GLFW_RELEASE,
	Begin = GLFW_PRESS,
	Change = GLFW_REPEAT
};

using Key = int;

class InputManager {
public:
	using ActionCallback = std::function
		<void(const std::string&, InputState, Key)>;
private:
	struct Action {
		ActionCallback callback;
		std::unordered_set<Key> input_types;
	};

	struct Self {
		WindowManager* wm;
		
		std::unordered_map<std::string, Action> actions;
		std::unordered_map<Key, std::unordered_set<std::string>> input_to_action_map;
		
		bool mouse_locked = false;

		double mouse_dx = 0.0f;
		double mouse_dy = 0.0f;
		double mouse_last_x = 0.0f;
		double mouse_last_y = 0.0f;
		double mouse_scroll_x = 0.f;
		double mouse_scroll_y = 0.f;
	} self;

	InputManager() = default;

private:
	void call_actions(
		const std::unordered_set<std::string>& actions_to_trigger,
		const InputState& state, const Key& key
	) {
		for (const auto& action_name : actions_to_trigger) {
			auto action_it = self.actions.find(action_name);
			auto& callback = action_it->second.callback;
			if (action_it != self.actions.end() && callback) {
				callback(action_name, state, key);
			}
		}
	}

	InputState to_input_state(int state) {
		return static_cast<InputState>(state);
	}

	void process_key_input(Key key, int state) {
		auto it = self.input_to_action_map.find(key);
		if (it == self.input_to_action_map.end()) { return; }

		InputState input_state = to_input_state(state);

		std::unordered_set<std::string> actions_to_trigger = it->second;
		call_actions(actions_to_trigger, input_state, key);
	}

	void process_mouse_move(double x, double y) {

		if (self.mouse_locked) {
			self.mouse_dx += x - self.mouse_last_x;
			self.mouse_dy += y - self.mouse_last_y;
		}
		self.mouse_last_x = x;
		self.mouse_last_y = y;
	}

	void process_mouse_scroll(double x, double y) {
		self.mouse_scroll_x = x;
		self.mouse_scroll_y = y;
	}

public:
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&& other) = delete;
	InputManager& operator=(InputManager&& other) = delete;

	static std::unique_ptr<InputManager> New(WindowManager* wm) {
		auto input = std::unique_ptr<InputManager>(new InputManager());
		auto& self = input->self;

		self.wm = wm;

		wm->set_keyboard_input_callback([input_ptr = input.get()](Key key, int state) {
			input_ptr->process_key_input(key, state);
		});

		wm->set_mouse_button_callback([input_ptr = input.get()](Key key, int state) {
			input_ptr->process_key_input(key, state);
		});

		wm->set_mouse_move_callback([input_ptr = input.get()](double x, double y) {
			input_ptr->process_mouse_move(x, y);
		});

		wm->set_mouse_scroll_callback([input_ptr = input.get()](double x, double y) {
			input_ptr->process_mouse_scroll(x, y);
		});

		self.mouse_locked = wm->get_mouse_locked();

		auto mouse_pos = wm->get_mouse_pos();
		self.mouse_last_x = mouse_pos.x;
		self.mouse_last_y = mouse_pos.y;

		return input;
	};

	void unbind_action(const std::string& action_name) {
		auto action_it = self.actions.find(action_name);
		if (action_it == self.actions.end()) { return; }

		const auto& input_set = action_it->second.input_types;
		for (Key key : input_set) {
			auto input_map_it = self.input_to_action_map.find(key);
			if (input_map_it == self.input_to_action_map.end()) { continue; }

			auto& action_set = input_map_it->second;
			action_set.erase(action_name);

			if (!action_set.empty()) { continue; }
			self.input_to_action_map.erase(input_map_it);
		}

		self.actions.erase(action_it);
	}

	template<typename... Keys>
	std::enable_if_t<(std::is_same_v<Keys, Key> && ...), void> bind_action(
		const std::string& action_name, ActionCallback action_callback, Keys... keys
	) {
		unbind_action(action_name);

		Action& action = self.actions[action_name];
		action.callback = std::move(action_callback);

		auto process_input = [&](Key key) {
			action.input_types.insert(key);
			self.input_to_action_map[key].insert(action_name);
		};

		(process_input(static_cast<Key>(keys)), ...);
	};

	void set_mouse_locked(bool locked) {
		self.wm->set_mouse_locked(locked);
		self.mouse_locked = locked;
	}

	bool get_mouse_locked() const {
		return self.mouse_locked;
	}

	glm::dvec2 get_mouse_position() {
		return glm::dvec2(self.mouse_last_x, self.mouse_last_y);
	}

	glm::dvec2 get_mouse_delta() {
		return glm::dvec2(self.mouse_dx, self.mouse_dy);
	}

	glm::vec2 get_mouse_scroll() {
		return glm::vec2(self.mouse_scroll_x, self.mouse_scroll_y);
	}

	void poll() {
		self.mouse_dx = 0.0;
		self.mouse_dy = 0.0;
		self.mouse_scroll_x = 0.0;
		self.mouse_scroll_y = 0.0;
		self.wm->poll();
	}
};

