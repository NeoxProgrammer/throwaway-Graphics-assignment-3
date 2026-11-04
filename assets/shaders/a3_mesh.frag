#version 430 core

in vec3 ndc;
out vec4 frag_colour;
in vec2 frag_tcoords; 
in vec3 frag_normal; 

void main()
{
    vec3 color_space = ndc * 0.5 + 0.5; // [-1..1] --> [0..1]; 
    frag_colour = vec4(frag_tcoords.x,frag_tcoords.y,0.0, 1.0);

}
