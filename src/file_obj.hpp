#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hpp"

class FileObj {
private:
	struct Obj {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
	};

	static glm::vec3 s_get_attrib_vec3(
		const std::vector<tinyobj::real_t>& attrib_vector,
		int index,
		const char* attrib_kind_str,
		const glm::vec3& default_value) {
		if (index < 0 || attrib_vector.empty() ||
			(static_cast<size_t>(index) * 3 + 2) >= attrib_vector.size()) {
			return default_value;
		}
		return glm::vec3(
			attrib_vector[3 * static_cast<size_t>(index) + 0],
			attrib_vector[3 * static_cast<size_t>(index) + 1],
			attrib_vector[3 * static_cast<size_t>(index) + 2]);
	}

	static glm::vec2 s_get_attrib_vec2(
		const std::vector<tinyobj::real_t>& attrib_vector,
		int index,
		const char* attrib_kind_str,
		const glm::vec2& default_value) {
		if (index < 0 || attrib_vector.empty() ||
			(static_cast<size_t>(index) * 2 + 1) >= attrib_vector.size()) {
			return default_value;
		}
		return glm::vec2(
			attrib_vector[2 * static_cast<size_t>(index) + 0],
			attrib_vector[2 * static_cast<size_t>(index) + 1]);
	}

	static void into_vert_indices(
		Obj& obj_data,
		Vertices& out_vertices,
		Indices& out_indices
	) {
		out_vertices.clear();
		out_indices.clear();

		std::map<std::tuple<int, int, int>, uint32_t>
			unique_vertex_map;

		for (const auto& shape : obj_data.shapes) {
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
				size_t fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);

				for (size_t v = 0; v < fv; ++v) {
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

					std::tuple<int, int , int> current_vertex_key =
						std::make_tuple(idx.vertex_index, idx.normal_index, idx.texcoord_index);

					uint32_t final_vertex_index;

					auto it = unique_vertex_map.find(current_vertex_key);
					if (it == unique_vertex_map.end()) {
						Vertex new_vertex;

						new_vertex.pos = s_get_attrib_vec3(
							obj_data.attrib.vertices,
							idx.vertex_index,
							"vertex position",
							glm::vec3(0.0f)); 

						new_vertex.nor = s_get_attrib_vec3(
							obj_data.attrib.normals,
							idx.normal_index,
							"normal",
							glm::vec3(0.0f, 0.0f, 1.0f));

						new_vertex.tex = s_get_attrib_vec2(
							obj_data.attrib.texcoords,
							idx.texcoord_index,
							"texture coordinate",
							glm::vec2(0.0f)
						);

						out_vertices.push_back(new_vertex);
						final_vertex_index =
							static_cast<uint32_t>(out_vertices.size() - 1);
						unique_vertex_map[current_vertex_key] =
							final_vertex_index;
					} else {
						final_vertex_index = it->second;
					}
					out_indices.push_back(final_vertex_index);
				}
				index_offset += fv;
			}
		}
	}
public:
	static void Load(std::string basedir, std::string obj_name, Vertices& vertices, Indices& indices) {
		std::string warn, err;

		Obj obj;
		std::string obj_path = basedir + obj_name + ".obj";

		bool ret = tinyobj::LoadObj(
			&obj.attrib,
			&obj.shapes,
			&obj.materials,
			&warn,
			&err,
			obj_path.c_str(),
			basedir.c_str()
		);

		if (!warn.empty()) {
			std::cout << "WARN: " << warn;
			exit(EXIT_FAILURE);
		}

		if (!err.empty()) {
			std::cerr << "ERR: " << err;
			exit(EXIT_FAILURE);
		}

		if (!ret) {
			std::cerr << "Failed to load Obj file.\n";
			exit(EXIT_FAILURE);
		}

		into_vert_indices(obj, vertices, indices);
	}
};


