#version 330

in vec3 world_position;
in vec3 world_normal;
in vec2 texture_coords;

uniform vec3 camera_position;
uniform sampler2D u_texture_0;

// Light and material properties
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform vec3 light_color;

// Struct containing information about spotligths illuminating the fragment
#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct SpotLight {
   vec3 position;
   float cone_angle;
   vec3 cone_direction;
} spot_lights[MAX_LIGHTS];

layout(location = 0) out vec4 out_color;


// Calculate light from a spotlight
float calculate_light(SpotLight light, vec3 N, vec3 L, vec3 V, vec3 H)
{
    float ambient_light = 0.0f;
    float diffuse_light;
    float specular_light;
    float result_light;
    
    float cut_off = light.cone_angle;
    float spot_light = dot(- L, light.cone_direction);
    float spot_light_limit = cos(cut_off);

    if (spot_light > spot_light_limit) {
        // Fragment is illuminated by the spotlight
        diffuse_light  = material_kd * max(dot(L, N), 0) * 8;
        specular_light = 0.0f;
        float attenuation = pow((spot_light - spot_light_limit) / (1.0f - spot_light_limit), 2);
        result_light = ambient_light + attenuation * (diffuse_light + specular_light);
    } else {
        // Fragment is not illuminated by the spotlight
        result_light = ambient_light;
    }

    return result_light;
}


void main()
{
	vec3 N = normalize(world_normal);
	vec3 V = normalize(camera_position - world_position);
	vec3 L, H;

	float ambient_light = 3.0f;
	float diffuse_light = material_kd * max(dot(L, N), 0);
	float specular_light = 0.5f;
	float light = ambient_light + diffuse_light + specular_light;

    // Add spotlights light
    for (int i = 0; i < num_lights; ++i) {
        L = normalize(spot_lights[i].position - world_position);
        H = normalize(L + V);
        light += calculate_light(spot_lights[i], N, L, V, H);
    }

	out_color = mix(texture(u_texture_0, texture_coords), vec4(0.0f), 0.87f) * light * light_color;
}