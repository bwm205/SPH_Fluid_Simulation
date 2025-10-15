#include <cmath>
#include <iostream>

#include <glm/glm.hpp>

#include "smoothed_particle.hpp"

using namespace glm;;


Particle::Particle(vec2 position, vec2 velocity , Kernel* kernel, float mass, const float smoothing_radius)
:   position(position), 
    velocity(velocity),
    mass(mass),
    kernel(kernel),
    smoothing_radius(smoothing_radius),
    acceleration(0.0f,0.0f)
{}


float Particle::smoothing_kernel(vec2 sample_position)
{
    float dist = distance(position, sample_position); //Calculate distance from particle to point we are sampling

    if (dist < smoothing_radius)
    {
        return kernel->function(dist, smoothing_radius);
    }
    else
    {
        return 0.0;
    }
}


float Particle::smoothing_kernel_derivative(vec2 sample_position)
{
    float dist = distance(position, sample_position); //Calculate distance from particle to point we are sampling

    if (dist < smoothing_radius)
    {
        return kernel->derivative(dist, smoothing_radius);
    }
    else
    {
        return 0.0;
    }
}



float Cubic_Spline_Kernel::function(float dist, float smoothing_radius)
{
    float normed_dist = dist / smoothing_radius;
    float volume = 7. * M_PI * pow(smoothing_radius,2.) / 40.;

    if (normed_dist > 1/2)
    {
        return 2. * pow((1-normed_dist),3.) / volume;
    }
    else
    {
        return (6. * (pow(normed_dist,3.) - pow(normed_dist,2.)) + 1.) / volume;
    }
}


float Cubic_Spline_Kernel::derivative(float dist, float smoothing_radius)
{
    float normed_dist = dist / smoothing_radius;
    float volume = 7. * M_PI * pow(smoothing_radius,2.) / 40.;

    if (normed_dist > 1/2)
    {
        return -6. * pow((1.-normed_dist),2.) / (volume * smoothing_radius);
    }
    else
    {
        return 6. * (3. * pow(normed_dist,2.) - 2. * normed_dist) / (volume * smoothing_radius);
    }
}



float Cubic_Kernel::function(float dist, float smoothing_radius)
{
    float volume = M_PI * pow(smoothing_radius,5.) / 10.;

    return pow(smoothing_radius - dist, 3.) / volume;
}


float Cubic_Kernel::derivative(float dist, float smoothing_radius)
{
    float volume = M_PI * pow(smoothing_radius,5.) / 10.;

    return -3. * pow(smoothing_radius - dist, 2.) / volume;
}


float Square_Kernel::function(float dist, float smoothing_radius)
{
    float volume = M_PI * pow(smoothing_radius,4.) / 6.;

    return pow(smoothing_radius - dist, 2.) / volume;
}


float Square_Kernel::derivative(float dist, float smoothing_radius)
{
    float volume = M_PI * pow(smoothing_radius,4.) / 6.;

    return -2. * (smoothing_radius - dist) / volume;
}