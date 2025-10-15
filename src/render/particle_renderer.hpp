#pragma once

#include <GL/glew.h>

#include <vector>
#include <string>


template<typename Particle>
class Particle_Renderer
{
private:
    //References to program + GPU buffers
    GLuint program;
    GLuint quadVAO,quadVBO;
    GLuint instVBO;

    //Uniform buffer data
    float rendering_radius;
    float velocity_clamp;
    GLint location_rendering_radius;
    GLint location_velocity_clamp;

    //Pointer to particles to render
    // MAYBE RECONSIDER THIS!!! If the position of this vector changes in memory (eg:by resizing vector), this pointer will not change and we will loose the particles vector
    //Initialise as pointer in first place? then pass through function
    std::vector<Particle>* particles;

    void build_buffers();

public:
    Particle_Renderer(std::vector<Particle>* particles, std::string vertex_filepath, std::string fragment_filepath, float rendering_radius_init, float velocity_clamp_init = 1.0);
    ~Particle_Renderer();

    void draw();
};

#include "particle_renderer.tpp"
