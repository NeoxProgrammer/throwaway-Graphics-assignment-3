#version 430 core

in vec3 ndc;
out vec4 frag_colour;

void main()
{
    vec3 color_space = ndc * 0.5 + 0.5; // [-1..1] --> [0..1]
    frag_colour = vec4(color_space, 1.0);
}
