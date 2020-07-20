#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 world_position;
out vec3 world_normal;
out vec2 texture_coords;

void main()
{
    world_position = (Model * vec4(v_position, 1.0f)).xyz;
	world_normal   = (Model * vec4(v_normal, 1.0f)).xyz;
	texture_coords = v_texture_coord;
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
