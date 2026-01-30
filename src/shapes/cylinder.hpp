#pragma once

void create_cylinder(Vertices& vertices, Indices& indices, int quality) {
	vertices.clear();
	indices.clear();
	
	const float pi = glm::pi<float>();

	if (quality < 3) { quality = 3; }
	
	float radius = 0.5f;
	float height = 1.0f;
	float half_height = height / 2.0f;
	
	glm::vec3 top_center_pos(0.0f, half_height, 0.0f);
	glm::vec3 cap_normal_top(0.0f, 1.0f, 0.0f);
	vertices.push_back({ top_center_pos, cap_normal_top, glm::vec2(0.5f, 0.5f) });
	GLint top_center_idx = 0;
	
	glm::vec3 bottom_center_pos(0.0f, -half_height, 0.0f);
	glm::vec3 cap_normal_bottom(0.0f, -1.0f, 0.0f);
	vertices.push_back({ bottom_center_pos, cap_normal_bottom, glm::vec2(0.5f, 0.5f) });
	GLint bottom_center_idx = 1;

	GLint current_idx_offset = 2;

	for (int i = 0; i <= quality; ++i) {
		float angle = 2.0f * pi * static_cast<float>(i) / static_cast<float>(quality);
		float x = radius * std::cos(angle);
		float z = radius * std::sin(angle);

		glm::vec3 top_v_pos(x, half_height, z);
		glm::vec2 cap_uv(0.5f + x / (2.f * radius), 0.5f + z / (2.f * radius));
		vertices.push_back({ top_v_pos, cap_normal_top, cap_uv });

		glm::vec3 bottom_v_pos(x, -half_height, z);
		vertices.push_back({ bottom_v_pos, cap_normal_bottom, cap_uv });

		glm::vec3 side_normal = glm::normalize(glm::vec3(x, 0.0f, z));
		float u_side = static_cast<float>(i) / static_cast<float>(quality);
		vertices.push_back({ top_v_pos, side_normal, glm::vec2(u_side, 1.0f) });
		vertices.push_back({ bottom_v_pos, side_normal, glm::vec2(u_side, 0.0f) });
	}

	for (int i = 0; i < quality; ++i) {
		GLint top_cap_v1 = current_idx_offset + i * 4;
		GLint top_cap_v2 = current_idx_offset + (i + 1) * 4;
		GLint bottom_cap_v1 = current_idx_offset + i * 4 + 1;
		GLint bottom_cap_v2 = current_idx_offset + (i + 1) * 4 + 1;

		GLint side_top1 = current_idx_offset + i * 4 + 2;
		GLint side_bottom1 = current_idx_offset + i * 4 + 3;
		GLint side_top2 = current_idx_offset + (i + 1) * 4 + 2;
		GLint side_bottom2 = current_idx_offset + (i + 1) * 4 + 3;

		indices.insert(indices.end(), { top_center_idx, top_cap_v2, top_cap_v1 });
		indices.insert(indices.end(), { bottom_center_idx, bottom_cap_v1, bottom_cap_v2 });

		indices.insert(indices.end(), { side_bottom2, side_bottom1, side_top2 });
		indices.insert(indices.end(), { side_bottom1, side_top1, side_top2 });
	}
}
