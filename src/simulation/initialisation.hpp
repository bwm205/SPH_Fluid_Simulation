#pragma once

#include <vector>

#include "smoothed_particle.hpp"

std::vector<Particle> uniform_random_init(int num_particles, float mass, float smoothing_radius, Kernel* kernel, float vx_lim = 0.25, float vy_lim = 0.25, float x_lim = 1.0, float y_lim = 1.0);
std::vector<Particle> uniform_init(int num_particles, float mass, float smoothing_radius, Kernel* kernel, float range = 2.0,  float x_lim = 1.0, float y_lim = 1.0);