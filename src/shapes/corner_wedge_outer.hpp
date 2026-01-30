#pragma once

void create_corner_wedge_outer(Vertices& vertices, Indices& indices) {
	vertices.clear();
	indices.clear();

	glm::vec3 p0 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 p1 = glm::vec3(0.5f, -0.5f, -0.5f);
	glm::vec3 p2 = glm::vec3(-0.5f, 0.5f, -0.5f);
	glm::vec3 p3 = glm::vec3(-0.5f, -0.5f, 0.5f);

	GLint current_v_idx = 0;

	glm::vec3 n_slant = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back({ p1, n_slant, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ p2, n_slant, glm::vec2(1.0f, 0.0f) }); // UVs for a triangle
	vertices.push_back({ p3, n_slant, glm::vec2(0.0f, 1.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
	current_v_idx += 3;

	glm::vec3 n_bottom = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices.push_back({ p0, n_bottom, glm::vec2(0.5f, 1.0f) }); // Apex UV
	vertices.push_back({ p1, n_bottom, glm::vec2(1.0f, 0.0f) });
	vertices.push_back({ p3, n_bottom, glm::vec2(0.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
	current_v_idx += 3;

	glm::vec3 n_back = glm::vec3(0.0f, 0.0f, -1.0f);
	vertices.push_back({ p0, n_back, glm::vec2(0.5f, 1.0f) });
	vertices.push_back({ p2, n_back, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ p1, n_back, glm::vec2(1.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
	current_v_idx += 3;

	glm::vec3 n_left = glm::vec3(-1.0f, 0.0f, 0.0f);
	vertices.push_back({ p0, n_left, glm::vec2(0.5f, 1.0f) });
	vertices.push_back({ p3, n_left, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ p2, n_left, glm::vec2(1.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
}
