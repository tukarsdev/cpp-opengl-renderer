#pragma once

void create_wedge(Vertices& vertices, Indices& indices) {
	vertices.clear();
	indices.clear();

	glm::vec3 p0 = glm::vec3(-0.5f, 0.5f, 0.5f);
	glm::vec3 p1 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 p2 = glm::vec3(0.5f, -0.5f, 0.5f);

	glm::vec3 p3 = glm::vec3(-0.5f, 0.5f, -0.5f);
	glm::vec3 p4 = glm::vec3(0.5f, -0.5f, -0.5f);
	glm::vec3 p5 = glm::vec3(-0.5f, -0.5f, -0.5f);

	GLint current_idx = 0;
	glm::vec2 uv00 = glm::vec2(0.0f, 0.0f);
	glm::vec2 uv10 = glm::vec2(1.0f, 0.0f);
	glm::vec2 uv01 = glm::vec2(0.0f, 1.0f);
	glm::vec2 uv11 = glm::vec2(1.0f, 1.0f);

	auto create_face_tri = [&](
		glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 normal, 
		glm::vec2 t0, glm::vec2 t1, glm::vec2 t2
	) {
		vertices.push_back({ p0, normal, t0 });
		vertices.push_back({ p1, normal, t1 });
		vertices.push_back({ p2, normal, t2 });
		indices.insert(indices.end(), { current_idx, current_idx + 1, current_idx + 2 });
		current_idx += 3;
	};

	auto create_face_square = [&](
		glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal,
		glm::vec2 t0, glm::vec2 t1, glm::vec2 t2, glm::vec2 t3
	) {
		vertices.push_back({ p0, normal, t0 });
		vertices.push_back({ p1, normal, t1 });
		vertices.push_back({ p2, normal, t2 });
		vertices.push_back({ p3, normal, t3 });
		indices.insert(indices.end(), { current_idx, current_idx + 1, current_idx + 2 });
		indices.insert(indices.end(), { current_idx, current_idx + 2, current_idx + 3 });
		current_idx += 4;
	};

	glm::vec3 n_left =   glm::vec3( 0.0f,  0.0f,  1.0f);
	glm::vec3 n_right =  glm::vec3( 0.0f,  0.0f,  -1.0f);
	glm::vec3 n_back =   glm::vec3( -1.0f,  0.0f, 0.0f);
	glm::vec3 n_bottom = glm::vec3( 0.0f, -1.0f,  0.0f);
	glm::vec3 n_slant =  glm::normalize(glm::cross(p4 - p2, p0 - p2));

	create_face_tri(p0, p1, p2, n_left, uv01, uv10, uv00);
	create_face_tri(p3, p4, p5, n_right, uv01, uv00, uv10);

	create_face_square(p1, p5, p4, p2, n_bottom, uv00, uv10, uv11, uv01);
	create_face_square(p0, p3, p5, p1, n_back, uv10, uv00, uv01, uv11);
	create_face_square(p0, p2, p4, p3, n_slant, uv00, uv10, uv11, uv01);
}
