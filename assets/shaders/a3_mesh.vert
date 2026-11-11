#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_tcoord;

out vec3 position;
out vec3 normal;
out vec2 tcoord;

uniform mat4 u_transform;

void main()
{
    vec4 pos = u_transform * vec4(vertex_position, 1.0);

    position = pos.xyz;
    normal = vec3(1.0, 0.0, 1.0);
    tcoord = vec2(1.0, 1.0);

    // Uncomment to test coloring based on vertex attributes
    tcoord = vertex_tcoord;
    //normal = mat3(u_transform) * vertex_normal;
    normal = normalize(mat3(u_transform) * vertex_normal);

    gl_Position = pos;
}
