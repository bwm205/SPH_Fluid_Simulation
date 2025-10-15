#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>

#include <glm/glm.hpp>

#include "smoothed_particle.hpp"

using namespace glm;

std::vector<Particle> uniform_random_init(int num_particles, float mass, float smoothing_radius, Kernel* kernel, float vx_lim, float vy_lim, float x_lim, float y_lim)
{   //Initialise particles
    std::vector<Particle> particles;

    //Initialise random number generator
    std::srand(time(0));

    for (int i = 0; i < num_particles; i++)
    {
        //Initialsie random position and constant velocity and append to vector
        float x = -x_lim + smoothing_radius + 2*(x_lim - smoothing_radius) * std::rand()/float(RAND_MAX);    //RAND_MAX will scale rand() output between 0 and 1
        float y = -y_lim + smoothing_radius + 2*(y_lim - smoothing_radius) * std::rand()/float(RAND_MAX);
        vec2 position(x, y);

        float vx = -vx_lim + 2*vx_lim * std::rand()/float(RAND_MAX);
        float vy = -vy_lim + 2*vy_lim * std::rand()/float(RAND_MAX);
        vec2 velocity(vx, vy);
        
        particles.emplace_back(position, velocity, kernel, mass, smoothing_radius);     //Emplace back also constructs object
    }

    return particles;
}


std::vector<Particle> uniform_init(int num_particles, float mass, float smoothing_radius, Kernel* kernel, float range, float x_lim, float y_lim)
{
    std::vector<Particle> particles;

    int particles_per_row = std::ceil(std::sqrt(num_particles));
    float grid_width = range / particles_per_row;
    int particles_added = 0;

    for (int i = 0; i < particles_per_row; i++)
    {
        for(int j = 0; j < particles_per_row; j++)
        {
            //Check if all particles added
            if (particles_added == num_particles) {break;} 
            else {particles_added++;}

            //Calculate position on grid
            float x = grid_width * (j + 0.5) - x_lim/2;
            float y = range - grid_width * (i + 0.5) - y_lim/2;
            vec2 position(x, y);
            vec2 velocity(0.0, 0.0);      //Initialise with 0 velocity

            particles.emplace_back(position, velocity, kernel, mass, smoothing_radius);
        }
    }

    return particles;
}