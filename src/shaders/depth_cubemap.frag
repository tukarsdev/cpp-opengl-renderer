#version 450 core

in vec3 v_frag_pos_world;

uniform vec3 light_pos_world;
uniform float light_far_plane;

void main() {
	float light_dist = length(v_frag_pos_world - light_pos_world);
	gl_FragDepth = light_dist / light_far_plane;
}
