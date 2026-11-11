#version 430 core

in vec3 position;
in vec3 normal;
in vec2 tcoord;

uniform int u_style;
uniform int u_color_space;

out vec4 frag_colour;

void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);
    switch (u_style)
    {
        case 0:
            color = position;
            break;

        case 1:
            color = normal;
            break;

        case 2:
            color = vec3(tcoord, 0.0);
            break;
    }
    
    if (u_color_space == 1)
    {
        // [-1..1] --> [0..1]
        color = color * 0.5 + 0.5;
    }

    frag_colour = vec4(color, 1.0);
}
