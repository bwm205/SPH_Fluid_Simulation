#pragma once

#include <glm/glm.hpp>

using namespace glm;


//Base kernel type
class Kernel
{
    public:
        virtual float function(float dist, float smoothing_radius) = 0;         //virtual keyword since no default implimentation. Only ever overwritten
        virtual float derivative(float dist, float smoothing_radius) = 0;
};


//Specific kernels. Polymorphism overrides functions
class Cubic_Spline_Kernel : public Kernel
{
    public:
        float function(float dist, float smoothing_radius) override;
        float derivative(float dist, float smoothing_radius) override;
};


class Cubic_Kernel : public Kernel
{
    public:
        float function(float dist, float smoothing_radius) override;
        float derivative(float dist, float smoothing_radius) override;
};


class Square_Kernel : public Kernel
{
    public:
        float function(float dist, float smoothing_radius) override;
        float derivative(float dist, float smoothing_radius) override;
};



class Particle
{
public:
    vec2 position, velocity, acceleration;
    float mass;
    float density = 0.0f;
    float pressure = 0.0f;
    const float smoothing_radius;
    Kernel* kernel;

    Particle(vec2 position, vec2 velocity, Kernel* kernel, float mass = 1.0, const float smoothing_radius = 0.1); 
    float smoothing_kernel(vec2 sample_position);
    float smoothing_kernel_derivative(vec2 sample_position);
};