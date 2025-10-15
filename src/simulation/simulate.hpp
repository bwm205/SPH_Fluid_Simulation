#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "smoothed_particle.hpp"

using namespace glm;


class System
{
private:
    //Simulation Parameters
    const float gravity;
    float rest_density;
    float stiffness;

    //Collision Parameters
    const float collision_damping;
    float x_lim;
    float y_lim;        //Given as distance

    void collision_detection(Particle& particle, float x_lim = 1.0, float y_lim = 1.0);
    float calc_density(vec2 point, std::vector<Particle>& particles);
    float calc_pressure(float density);
    vec2 calc_pressure_force(int target_particle_ind, std::vector<Particle>& particles);


public:
    std::vector<Particle> particles;

    System(const float gravity, const float rest_density, const float stiffness, const float collision_damping, float x_lim = 1.0, float y_lim = 1.0);
    void simulation_step(float time_step);
};

