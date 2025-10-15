#version 330 core
in vec2 texture_coordinate;

uniform sampler2D background_texture;       //sampler2D gives index for texture to sample from
uniform float background_clamp;
uniform float opacity;

out vec4 fragment_colour;

void main()
{
    //Extract background value from r channel
    float background_value = texture(background_texture, texture_coordinate).r;      

    if(background_value == 0) discard;      //If density = 0, don't render
    
    //Calculate blend factor
    float blend_factor = clamp(background_value/background_clamp, 0.0, 1.0);                     
    vec3 col = mix(vec3(0.0,0.0,0.0), vec3(0.0,0.5,0.0), blend_factor); 
    fragment_colour = vec4(col, opacity);                       
}