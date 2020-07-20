#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform float delta_time_seconds;
uniform vec3 gravitational_acceleration;
uniform int update_physics;

flat out vec4 points_num;
flat out vec4 rotation;
flat out vec4 points_pos[16];

struct Particle {
    vec4 position;
    vec4 speed;
    vec4 rotationAngle;
    vec4 initilalAngle;
    vec4 pointsNum;
    vec4 pointsPos[16];
};

layout(std430, binding = 0) buffer particles {
	Particle data[];
};

void main()
{
	vec3 pos = data[gl_VertexID].position.xyz;
	vec3 spd = data[gl_VertexID].speed.xyz;
    vec3 rot = data[gl_VertexID].rotationAngle.xyz;
    vec3 rot_init = data[gl_VertexID].initilalAngle.xyz;

    if (update_physics == 1) {
        pos += spd * delta_time_seconds;
        spd += gravitational_acceleration * delta_time_seconds;
        rot += rot_init * delta_time_seconds;
    }

    // Update stats in buffer
	data[gl_VertexID].position.xyz = pos;
	data[gl_VertexID].speed.xyz = spd;
    data[gl_VertexID].rotationAngle.xyz = rot;

    // Send data to geometry shader
    points_num = data[gl_VertexID].pointsNum;
    points_pos = data[gl_VertexID].pointsPos;
    rotation = data[gl_VertexID].rotationAngle;

	gl_Position = Model * vec4(pos, 1);
}
