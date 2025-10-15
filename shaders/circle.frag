#version 330 core
in vec2 local_position;     //Interpolates between quad vertexes (in local space)
in float particle_speed;

uniform float velocity_clamp;       //Define in fragment so passed through from vertex

out vec4 fragment_colour;

void main()
{
    //Circle mask
    if(length(local_position) > 1.0) discard;       //length() gives vector norm. If longer than circle distance, throw away

    //Velocity color mapping (white to red)
    float blend_factor = clamp(particle_speed/velocity_clamp, 0.0, 1.0);                      //Gives how much red to keep. Clamps between 0 and 1
    vec3 col = mix(vec3(1.0,1.0,1.0), vec3(1.0,0.0,0.0), blend_factor);    //Blends white and red by given factor

    fragment_colour = vec4(col,1.0);
}
