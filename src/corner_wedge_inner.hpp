#pragma once

void create_corner_wedge_inner(Vertices& vertices, Indices& indices) {
	vertices.clear();
	indices.clear();

	glm::vec3 s1 = glm::vec3(-0.5f, 0.5f, 0.5f);
	glm::vec3 s2 = glm::vec3(0.5f, -0.5f, 0.5f);
	glm::vec3 s3 = glm::vec3(0.5f, 0.5f, -0.5f);

	glm::vec3 p_bbb = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 p_bbr = glm::vec3(0.5f, -0.5f, -0.5f);
	glm::vec3 p_btl = glm::vec3(-0.5f, 0.5f, -0.5f);
	glm::vec3 p_fbl = glm::vec3(-0.5f, -0.5f, 0.5f);

	GLint current_v_idx = 0;

	glm::vec3 n_slant = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back({ s1, n_slant, glm::vec2(0.0f, 1.0f) });
	vertices.push_back({ s2, n_slant, glm::vec2(1.0f, 0.0f) });
	vertices.push_back({ s3, n_slant, glm::vec2(0.0f, 0.0f) }); // Example UVs
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
	current_v_idx += 3;

	glm::vec3 n_bottom = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices.push_back({ p_bbb, n_bottom, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ p_fbl, n_bottom, glm::vec2(0.0f, 1.0f) });
	vertices.push_back({ s2,	n_bottom, glm::vec2(1.0f, 1.0f) });
	vertices.push_back({ p_bbr, n_bottom, glm::vec2(1.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx + 0, current_v_idx + 3, current_v_idx + 2 });
	indices.insert(indices.end(), { current_v_idx + 0, current_v_idx + 2, current_v_idx + 1 });
	current_v_idx += 4;

	glm::vec3 n_back = glm::vec3(0.0f, 0.0f, -1.0f);
	vertices.push_back({ p_bbb, n_back, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ p_bbr, n_back, glm::vec2(1.0f, 0.0f) });
	vertices.push_back({ s3,	n_back, glm::vec2(1.0f, 1.0f) });
	vertices.push_back({ p_btl, n_back, glm::vec2(0.0f, 1.0f) });
	indices.insert(indices.end(), { current_v_idx + 0, current_v_idx + 3, current_v_idx + 2 });
	indices.insert(indices.end(), { current_v_idx + 0, current_v_idx + 2, current_v_idx + 1 });
	current_v_idx += 4;

	glm::vec3 n_left = glm::vec3(-1.0f, 0.0f, 0.0f);
	vertices.push_back({ p_bbb, n_left, glm::vec2(1.0f, 0.0f) });
	vertices.push_back({ p_btl, n_left, glm::vec2(1.0f, 1.0f) });
	vertices.push_back({ s1,	n_left, glm::vec2(0.0f, 1.0f) });
	vertices.push_back({ p_fbl, n_left, glm::vec2(0.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx + 0, current_v_idx + 3, current_v_idx + 2 });
	indices.insert(indices.end(), { current_v_idx + 0, current_v_idx + 2, current_v_idx + 1 });
	current_v_idx += 4;

	glm::vec3 n_top = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back({ p_btl, n_top, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ s1,	n_top, glm::vec2(0.0f, 1.0f) });
	vertices.push_back({ s3,	n_top, glm::vec2(1.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
	current_v_idx += 3;

	glm::vec3 n_front = glm::vec3(0.0f, 0.0f, 1.0f);
	vertices.push_back({ p_fbl, n_front, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ s2,	n_front, glm::vec2(0.0f, 1.0f) });
	vertices.push_back({ s1,	n_front, glm::vec2(1.0f, 0.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
	current_v_idx += 3;

	glm::vec3 n_right = glm::vec3(1.0f, 0.0f, 0.0f);
	vertices.push_back({ p_bbr, n_right, glm::vec2(0.0f, 0.0f) });
	vertices.push_back({ s3,	n_right, glm::vec2(1.0f, 1.0f) });
	vertices.push_back({ s2,	n_right, glm::vec2(0.0f, 1.0f) });
	indices.insert(indices.end(), { current_v_idx, current_v_idx + 1, current_v_idx + 2 });
}
