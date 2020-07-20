#version 430
layout(location = 0) in vec3 text_coord;

uniform samplerCube texture_cubemap;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture(texture_cubemap, text_coord);
}
