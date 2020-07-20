#version 430


layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

uniform float total_time;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec3 raza_incidenta;

vec3 myReflect()
{
    // TODO - compute the reflection color value
	vec3 r = reflect(-raza_incidenta, world_normal);
	return texture(texture_cubemap,r).xyz;
}

vec3 myRefract(float refractive_index)
{
    // TODO - compute the refraction color value
	vec3 r = refract(-raza_incidenta, world_normal,refractive_index);
	return texture(texture_cubemap,r).xyz;
}

void main()
{
	raza_incidenta = normalize(camera_position - world_position);
    out_color = vec4((myRefract(1.33f) + 0 * myReflect()), 0);
}
