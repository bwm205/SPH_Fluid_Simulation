#include <vector>
#include <cmath>
#include <iostream>

#include "simulate.hpp"
#include "smoothed_particle.hpp"

using namespace glm;


System::System(const float gravity, float rest_density, float stiffness, const float collision_damping, float x_lim, float y_lim)
:   gravity(gravity),
    rest_density(rest_density),
    stiffness(stiffness),
    collision_damping(collision_damping),
    x_lim(x_lim),
    y_lim(y_lim)
{}


void System::simulation_step(float time_step)
{
    for (auto& particle : particles)
    {
        particle.density = calc_density(particle.position, particles);
        particle.pressure = calc_pressure(particle.density);
    }

    //Update particles
    for (int i = 0; i < particles.size(); i++) 
    {
        //Calculate pressure force
        vec2 pressure = calc_pressure_force(i, particles);

        //Update particle state
        particles[i].velocity.y += gravity * time_step;
        particles[i].velocity -= pressure;
        particles[i].position += particles[i].velocity * time_step;

        //Detect collisions with wall
        collision_detection(particles[i], x_lim, y_lim);
    } 
}


void System::collision_detection(Particle& particle, float x_lim, float y_lim)
{
    //Simple bounce at screen edges
    if(std::abs(particle.position.x) + particle.smoothing_radius > x_lim)
    {
        particle.position.x = (x_lim - particle.smoothing_radius) * sign(particle.position.x);
        particle.velocity.x *= -1 * collision_damping;
    }
    if(std::abs(particle.position.y) + particle.smoothing_radius > y_lim)
    {
        particle.position.y = (y_lim - particle.smoothing_radius) * sign(particle.position.y);
        particle.velocity.y *= -1 * collision_damping;
    }
}


float System::calc_density(vec2 point, std::vector<Particle>& particles)
{
    float density = 0.0;

    for (auto& particle : particles)
    {
        density += particle.mass * particle.smoothing_kernel(point);
    }

    return density;
}


float System::calc_pressure(float density)
{
    return stiffness * (density - rest_density);
}



vec2 System::calc_pressure_force(int target_particle_ind, std::vector<Particle>& particles)
{
    vec2 pressure_force(0.0);

    for (int i = 0; i < particles.size(); i++)
    {
        if (i == target_particle_ind) continue;
        
        //Normalised vector between particles
        vec2 direction = particles[target_particle_ind].position - particles[i].position;
        if (direction.x == 0 && direction.y == 0 )          //Catch scenario with particles in same positino
            {direction = -particles[target_particle_ind].velocity;}     
        direction /= length(direction);

        float pressure = particles[target_particle_ind].pressure/pow(particles[target_particle_ind].density,2) + particles[i].pressure/pow(particles[i].density,2);

        float influence = particles[i].smoothing_kernel_derivative(particles[target_particle_ind].position);

        pressure_force += particles[i].mass * influence * pressure * direction;

    }

    return pressure_force;
}