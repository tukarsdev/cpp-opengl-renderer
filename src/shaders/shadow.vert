#version 450 core

layout(location = 0) in vec4 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_tex;

layout(location = 3) in mat4 model;
layout(location = 7) in vec4 v_col;

uniform mat4 projlmat;

void main()
{
	gl_Position = projlmat * model * v_pos;
}