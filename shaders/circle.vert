#version 330 core
layout(location = 0) in vec2 quad_position;   // Quad corner  (quad is two triangles making a square)
layout(location = 1) in vec2 particle_position;   // Particle center
layout(location = 2) in vec2 particle_velocity;   // Particle velocity

uniform float particle_radius;    // Particle radius
uniform float velocity_clamp;     //Max velocity at which to clamp frag colouring

out vec2 local_position;
out float particle_speed;

void main()
{
    // Calculate vertex position
    vec2 world_position = particle_position + quad_position * particle_radius;

    //Vertex position output
    gl_Position = vec4(world_position, 0, 1);      // gl_Position variable outputs position in screen space. Last 2 components z position and number used for projection matrix (keep as 1)?
    
    //Output to fragment shader
    local_position = quad_position;      // map quad [-1,1]^2 to circle local space
    particle_speed = length(particle_velocity);
}
