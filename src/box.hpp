#pragma once



void create_box(Vertices& vertices, Indices& indices) {
	GLint current_idx = 0;
	auto create_face = [&](
		glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3 normal
	) {
		static const glm::vec2 uv00(0.0f, 0.0f);
		static const glm::vec2 uv10(1.0f, 0.0f);
		static const glm::vec2 uv11(1.0f, 1.0f);
		static const glm::vec2 uv01(0.0f, 1.0f);

		vertices.push_back({ p0, normal, uv00 });
		vertices.push_back({ p1, normal, uv10 });
		vertices.push_back({ p2, normal, uv11 });
		vertices.push_back({ p3, normal, uv01 });
		indices.insert(indices.end(), { current_idx, current_idx + 1, current_idx + 2 });
		indices.insert(indices.end(), { current_idx, current_idx + 2, current_idx + 3 });
		current_idx += 4;
	};

	vertices.clear();
	indices.clear();

	glm::vec3 p0 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 p1 = glm::vec3(0.5f, -0.5f, 0.5f);
	glm::vec3 p2 = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 p3 = glm::vec3(-0.5f, 0.5f, 0.5f);
	glm::vec3 p4 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 p5 = glm::vec3(0.5f, -0.5f, -0.5f);
	glm::vec3 p6 = glm::vec3(0.5f, 0.5f, -0.5f);
	glm::vec3 p7 = glm::vec3(-0.5f, 0.5f, -0.5f);

	create_face(p0, p1, p2, p3, glm::vec3( 0.0f,  0.0f,  1.0f));
	create_face(p5, p4, p7, p6, glm::vec3( 0.0f,  0.0f, -1.0f));
	create_face(p1, p5, p6, p2, glm::vec3( 1.0f,  0.0f,  0.0f));
	create_face(p4, p0, p3, p7, glm::vec3(-1.0f,  0.0f,  0.0f));
	create_face(p3, p2, p6, p7, glm::vec3( 0.0f,  1.0f,  0.0f));
	create_face(p4, p5, p1, p0, glm::vec3( 0.0f, -1.0f,  0.0f));
};


	
	
	
	

