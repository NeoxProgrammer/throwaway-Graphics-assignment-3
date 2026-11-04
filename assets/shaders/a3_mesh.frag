#version 430 core

in vec3 ndc;
out vec4 frag_colour;
in vec2 frag_tcoords; 
in vec3 frag_normal; 

uniform int u_mode;// change this to a global uniform variable in the future


void main()
{
    if(u_mode == 0)
    {
        vec3 color_space = ndc * 0.5 + 0.5; // [-1..1] --> [0..1]; 
        frag_colour = vec4(frag_tcoords.x,frag_tcoords.y,0.0, 1.0);
    }

    if(u_mode == 1)
    {

      frag_colour = vec4(frag_normal.x,frag_normal.y,frag_normal.z, 1.0);

    }

}
