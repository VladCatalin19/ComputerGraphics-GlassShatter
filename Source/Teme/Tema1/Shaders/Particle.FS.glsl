#version 430

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

uniform samplerCube texture_cubemap;
uniform vec3 camera_position;
uniform int mode;

layout(location = 0) out vec4 out_color;

void main()
{
    switch (mode) {
    case 0:
        // Cube map
        float ratio = 1.00 / 1.05;
        vec3 incident_ray = normalize(world_position - camera_position);
        vec3 reflect_ray = reflect(incident_ray, normalize(world_normal));
        vec3 refract_ray = refract(incident_ray, world_normal, ratio);

        vec4 reflect_color = vec4(texture(texture_cubemap, reflect_ray).rgb, 1.0);
        vec4 refract_color = vec4(texture(texture_cubemap, refract_ray).rgb, 1.0);

        out_color = mix(mix(refract_color, vec4(0.0f, 0.02f, 0.01f, 1.0f), 0.2f),
                        reflect_color, 0.1f);
        break;

    case 1:
        // Face normal
        out_color = vec4((world_normal + 1.0f) / 2.0f, 1.0f);
        break;

    case 2:
        // Wireframe
        // We don't do that here
        out_color = vec4(0.5f);
        break;

    default:
        out_color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }
}