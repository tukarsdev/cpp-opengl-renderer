#version 450 core

#define MAX_LIGHTS 10

struct Light {
	int type;
	vec3 dir;
	vec3 pos;
	vec3 col;
	float range;
	float attl;
	float attq;
	float spinn;
	float spout;
	mat4 projlmat;
};

layout(location = 0) in vec4 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_tex;

layout(location = 3) in mat4 model;
layout(location = 7) in vec4 v_col;


uniform mat4 view;
uniform mat4 projection;

uniform Light lights[MAX_LIGHTS];
uniform int num_lights = 0;


out vec4 frag_col;
out vec3 frag_nor;
out vec3 frag_pos;
out vec4 fragpos_projls_2d[MAX_LIGHTS];

void main() {
	vec4 mvpos = model * v_pos;
	gl_Position = projection * view * mvpos;
	frag_col = v_col;
	frag_nor = mat3(transpose(inverse(model))) * v_nor;
	frag_pos = vec3(mvpos);
	for (int i = 0; i < num_lights; i++) {
		fragpos_projls_2d[i] = lights[i].projlmat * mvpos;
	}
}
