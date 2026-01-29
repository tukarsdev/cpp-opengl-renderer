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

layout (location = 0) out vec4 out_colour;

in vec4 frag_col;
in vec3 frag_nor;
in vec3 frag_pos;
in vec4 fragpos_projls_2d[MAX_LIGHTS];

uniform Light lights[MAX_LIGHTS];
uniform int num_lights = 0;
uniform sampler2DShadow shadow_maps_2d[MAX_LIGHTS];
uniform samplerCube shadow_maps_cube[MAX_LIGHTS];
uniform vec3 cam_pos;

const float ambient = 0.1f;
const float diff_strength = 1.0f;
const float spec_strength = 0.25f;
const float shininess = 132.f;
const float attc = 1.f;
const float bias_s = 0.007f;
const float bias_m = 0.007f;
const float point_bias = 0.005f;

vec3 calc_diff_spec(float diff, float spec, int i) {
	vec3 diff_l = diff * diff_strength * lights[i].col * frag_col.xyz;
	vec3 spec_l = spec * spec_strength * lights[i].col;
	return diff_l + spec_l;
}

float shadow_frag(int i, vec3 L_direction_to_light) {
	vec3 ndc = fragpos_projls_2d[i].xyz / fragpos_projls_2d[i].w;
	vec3 ss = (ndc + 1) * 0.5f;
	
	float frag_depth = ss.z;
	if (frag_depth > 1.0f || frag_depth < 0.0f) {
		return 1.0f;
	}

	vec3 N_nor = normalize(frag_nor);
	float bias = max(bias_s * (1.0f - dot(N_nor, L_direction_to_light)), bias_m);

	return texture(shadow_maps_2d[i], vec3(ss.xy, frag_depth - bias));
}


float shadow_frag_positional(int i) {
	vec3 light_to_frag_vec = frag_pos - lights[i].pos;
	float current_linear_depth = length(light_to_frag_vec);
	float depth_map_far_plane = 400.0f; // ideally not hardcoded...
	float current_normalized_depth = current_linear_depth / depth_map_far_plane;
	if (current_normalized_depth > 1.0f) {
		return 1.0f;
	}
	float sampled_normalized_depth = texture(shadow_maps_cube[i], normalize(light_to_frag_vec)).r;
	float shadow_factor = 1.0f;
	if (sampled_normalized_depth < current_normalized_depth - point_bias) {
		shadow_factor = 0.0f;
	}

	return shadow_factor;
}

vec3 calculate_directional_contribution(int i) {
	vec3 norm = normalize(frag_nor);
	vec3 light_dir = normalize(-lights[i].dir);
	float diff = max(dot(norm, light_dir), 0.0f);

	vec3 cam_dir = normalize(cam_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(cam_dir, reflect_dir), 0.0f), shininess);

	vec3 N_to_light = normalize(-lights[i].dir);
	float shadow = shadow_frag(i, N_to_light);
	vec3 diff_spec = calc_diff_spec(diff, spec, i);
	vec3 phong = shadow * diff_spec;

	return phong;
}

vec3 calculate_spot_contribution(int i) {
	vec3 dfrag = lights[i].pos - frag_pos;
	float d = length(dfrag);
	vec3 to_light = normalize(dfrag);
	
	vec3 norm = normalize(frag_nor);

	float diff = max(dot(norm, to_light), 0.0f);
	
	vec3 ref_light = reflect(-to_light, norm);
	vec3 cam_direction = normalize(cam_pos - frag_pos);
	float spec = pow(max(dot(cam_direction, ref_light), 0.0f), shininess);
	
	float att = 1.0f / (attc + (lights[i].attl * d) + (lights[i].attq * d * d));
	
	vec3 spot_dir = normalize(lights[i].dir);
	float theta = dot(to_light, -spot_dir);

	float cos_inner = cos(lights[i].spinn);
	float cos_outer = cos(lights[i].spout);
	float intensity = smoothstep(cos_outer, cos_inner, theta);

	float shadow = shadow_frag(i, to_light);
	vec3 diff_spec = calc_diff_spec(diff, spec, i);
	vec3 phong = shadow * diff_spec * att * intensity;

	return phong;
}

vec3 calculate_positional_contribution(int i) {
	vec3 norm = normalize(frag_nor);
	vec3 dfrag = lights[i].pos - frag_pos;
	float d = length(dfrag);
	vec3 light_dir = normalize(dfrag);
	
	float diff = max(dot(norm, light_dir), 0.0f);
	
	vec3 view_dir = normalize(cam_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), shininess);
	
	float att = 1.0 / (attc + (lights[i].attl * d) + (lights[i].attq * d * d));
	
	float shadow = shadow_frag_positional(i);
	vec3 diff_spec = calc_diff_spec(diff, spec, i);
	vec3 phong = shadow * diff_spec * att;

	return phong;
}

void main() {
	vec3 final_col = ambient * frag_col.xyz;
	for (int i = 0; i < num_lights; i++) {
		if (lights[i].type == 0) {
			final_col += calculate_directional_contribution(i);
		} else if (lights[i].type == 1) {
			final_col += calculate_positional_contribution(i);
		} else if (lights[i].type == 2) {
			final_col += calculate_spot_contribution(i);
		}
	}
	out_colour = vec4(final_col, frag_col.w);
}