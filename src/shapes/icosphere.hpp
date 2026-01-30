#pragma once

void create_icosphere(Vertices& vertices, Indices& indices, int quality) {
	auto get_mid_point = [&](
		GLint p1_idx, GLint p2_idx,
		std::map<std::pair<GLint, GLint>, GLint>& cache, float radius) {
		bool first_is_smaller = p1_idx < p2_idx;
		GLint smaller_idx = first_is_smaller ? p1_idx : p2_idx;
		GLint greater_idx = first_is_smaller ? p2_idx : p1_idx;
		std::pair<GLint, GLint> key(smaller_idx, greater_idx);

		auto it = cache.find(key);
		if (it != cache.end()) {
			return it->second;
		}

		glm::vec3 point1 = vertices[p1_idx].pos;
		glm::vec3 point2 = vertices[p2_idx].pos;
		glm::vec3 middle = glm::normalize(glm::vec3(
			(point1.x + point2.x) / 2.0f,
			(point1.y + point2.y) / 2.0f,
			(point1.z + point2.z) / 2.0f
		)) * radius;

		glm::vec3 normal = glm::normalize(middle);
		float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * glm::pi<float>());
		float v = 0.5f - std::asin(normal.y) / glm::pi<float>();

		vertices.push_back({ middle, normal, glm::vec2(u,v) });
		GLint new_idx = static_cast<GLint>(vertices.size() - 1);
		cache[key] = new_idx;
		return new_idx;
	};

	vertices.clear();
	indices.clear();

	float radius = 0.5f;
	float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;

	const glm::vec3 zv3 = glm::vec3(0.f);
	const glm::vec2 zv2 = glm::vec2(0.f);

	vertices.push_back({ glm::normalize(glm::vec3(-1,  phi,  0)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3( 1,  phi,  0)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3(-1, -phi,  0)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3( 1, -phi,  0)) * radius, zv3, zv2 });

	vertices.push_back({ glm::normalize(glm::vec3(0, -1,  phi)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3(0,  1,  phi)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3(0, -1, -phi)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3(0,  1, -phi)) * radius, zv3, zv2 });

	vertices.push_back({ glm::normalize(glm::vec3( phi,  0, -1)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3( phi,  0,  1)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3(-phi,  0, -1)) * radius, zv3, zv2 });
	vertices.push_back({ glm::normalize(glm::vec3(-phi,  0,  1)) * radius, zv3, zv2 });
	
	
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].nor = glm::normalize(vertices[i].pos);
		float u = 0.5f + std::atan2(vertices[i].nor.z, vertices[i].nor.x) / (2.0f * glm::pi<float>());
		float v = 0.5f - std::asin(vertices[i].nor.y) / glm::pi<float>();
		vertices[i].tex = glm::vec2(u, v);
	}
	
	Indices current_faces = {
		0, 11, 5,  0, 5, 1,   0, 1, 7,    0, 7, 10,   0, 10, 11,
		1, 5, 9,   5, 11, 4,  11, 10, 2,  10, 7, 6,   7, 1, 8,
		3, 9, 4,   3, 4, 2,   3, 2, 6,    3, 6, 8,    3, 8, 9,
		4, 9, 5,   2, 4, 11,  6, 2, 10,   8, 6, 7,    9, 8, 1
	};
	
	std::map<std::pair<GLint, GLint>, GLint> midpoint_cache;
	
	for (int q = 0; q < quality; q++) {
		Indices new_faces;
		midpoint_cache.clear();
		for (size_t i = 0; i < current_faces.size(); i += 3) {
			GLint v1_idx = current_faces[i];
			GLint v2_idx = current_faces[i + 1];
			GLint v3_idx = current_faces[i + 2];

			GLint m12_idx = get_mid_point(v1_idx, v2_idx, midpoint_cache, radius);
			GLint m23_idx = get_mid_point(v2_idx, v3_idx, midpoint_cache, radius);
			GLint m31_idx = get_mid_point(v3_idx, v1_idx, midpoint_cache, radius);

			new_faces.insert(new_faces.end(), { v1_idx, m12_idx, m31_idx });
			new_faces.insert(new_faces.end(), { v2_idx, m23_idx, m12_idx });
			new_faces.insert(new_faces.end(), { v3_idx, m31_idx, m23_idx });
			new_faces.insert(new_faces.end(), { m12_idx, m23_idx, m31_idx });
		}
		current_faces = new_faces;
	}
	indices = current_faces;
}
