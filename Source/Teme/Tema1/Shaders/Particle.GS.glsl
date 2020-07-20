#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 256) out;
flat in vec4 points_num[];
flat in vec4 rotation[];
flat in vec4 points_pos[][16];

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform int fragment_num;
uniform float thickness;

layout(location = 0) out vec3 world_position;
layout(location = 1) out vec3 world_normal;


mat4 rotation_x(float angle)
{
    return mat4(1.0f,       0.0f,        0.0f, 0.0f,
                0.0f, cos(angle), -sin(angle), 0.0f,
                0.0f, sin(angle),  cos(angle), 0.0f,
                0.0f,       0.0f,        0.0f, 1.0f);
}

mat4 rotation_y(float angle)
{
    return mat4( cos(angle), 0.0f, sin(angle), 0.0f,
                       0.0f, 1.0f,       0.0f, 0.0f,
                -sin(angle), 0.0f, cos(angle), 0.0f,
                       0.0f, 0.0f,       0.0f, 1.0f);
}

mat4 rotation_z(float angle)
{
    return mat4(cos(angle), -sin(angle), 0.0f, 0.0f,
                sin(angle),  cos(angle), 0.0f, 0.0f,
                      0.0f,        0.0f, 1.0f, 0.0f,
                      0.0f,        0.0f, 0.0f, 1.0f);
}

void EmitPoint(vec3 point, vec3 norm)
{
    gl_Position = Projection * View * vec4(point, 1.0f);
    world_position = point;
    world_normal = norm;
    EmitVertex();
}

void main()
{
    vec3 vpos = gl_in[0].gl_Position.xyz;
    vec3 norm_top, norm_side;
    mat4 rot = mat4(1);
    rot *= rotation_y(rotation[0].y);
    rot *= rotation_x(rotation[0].x);
    rot *= rotation_z(rotation[0].z);

    for (int i = 1; i < points_num[0].x - 1; ++i)
    {
        vec3 rot_pos0 = (points_pos[0][0] * rot).xyz;
        vec3 rot_posi = (points_pos[0][i] * rot).xyz;
        vec3 rot_posi1 = (points_pos[0][i + 1] * rot).xyz;
        norm_top = normalize(cross(rot_posi - rot_pos0, rot_posi1 - rot_pos0));
        vec3 rot_arr[3] = {rot_pos0, rot_posi, rot_posi1};
        // Front face
        for (int j = 0; j < 3; ++j)
        {
            EmitPoint(vpos + rot_arr[j], norm_top);
        }
        EndPrimitive();
        // Back face
        vec3 center = (rot_pos0 + rot_posi + rot_posi1) / 3.0f;
        for (int j = 2; j >= 0; --j)
        {
            rot_arr[j] -= norm_top * thickness;
            EmitPoint(vpos + rot_arr[j], -norm_top);
        }
        EndPrimitive();
    }

    // Side faces
    for (int i = 0; i < points_num[0].x; ++i)
    {
        vec3 posit = (points_pos[0][i] * rot).xyz;
        vec3 posi1t = (points_pos[0][(i + 1) % int(points_num[0].x)] * rot).xyz;
        vec3 posib = posit - norm_top * thickness;
        vec3 posi1b = posi1t - norm_top * thickness;
        norm_side = normalize(cross(posi1b - posib, posit - posib));
        EmitPoint(vpos + posib, norm_side);
        EmitPoint(vpos + posi1b, norm_side);
        EmitPoint(vpos + posit, norm_side);
        EmitPoint(vpos + posi1t, norm_side);
        EndPrimitive();
    }
}
