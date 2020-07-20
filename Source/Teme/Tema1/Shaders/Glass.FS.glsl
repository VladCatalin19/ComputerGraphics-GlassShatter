#version 430
layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec2 text_coord;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;
uniform vec3 camera_position;
uniform int mode;

layout(location = 0) out vec4 out_color;


void main()
{
    switch (mode) {
    case 0:
        float ratio = 1.00 / 1.05;
        vec3 incident_ray = normalize(world_position - camera_position);
        vec3 reflect_ray = reflect(incident_ray, normalize(world_normal));
        //vec3 refract_ray = incident_ray - 0.1 * normalize(world_normal);
        vec3 refract_ray = refract(incident_ray, world_normal, ratio);

        vec4 reflect_color = vec4(texture(texture_cubemap, reflect_ray).rgb, 1.0);
        vec4 refract_color = vec4(texture(texture_cubemap, refract_ray).rgb, 1.0);
        vec4 texture_color = vec4(texture2D(texture_1, text_coord));

        out_color = mix(mix(refract_color, vec4(0.0f, 0.02f, 0.01f, 1.0f), 0.2f), reflect_color, 0.1);
        break;
    case 1:
        
        break;
    case 2:

        break;
    default:

    }
}
