#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "src/render/renderer.hpp"
#include "src/simulation/simulate.hpp"
#include "src/simulation/smoothed_particle.hpp"
#include "src/simulation/initialisation.hpp"

using namespace glm;


//*** SIMULATION ***
float time_step = 0.01;
float particle_mass = 1.0;
const float smoothing_radius = .1;
Cubic_Spline_Kernel kernel;
const float gravity = -0.0f;
float rest_density = 10.f;
float stiffness = 0.005f;
const float collision_damping = 0.5;

//*** INITIALISATION ***
const int num_particles = 500;
float x_lim = 0.75, y_lim = 0.75;
float vx_lim = 0.0, vy_lim = 0.0;   //Not needed for uniform init
float range = 1.0;

//*** RENDERING ***
//Window
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const std::string WINDOW_NAME = "Fluid Simulation";
//Particles
std::string particle_vertex_filepath = "../shaders/circle.vert";
std::string particle_fragment_filepath = "../shaders/circle.frag";        //.. goes up one level from build where shader is compiled
const float velocity_clamp = 5.0;           //Clamp for rendering colours
const float rendering_radius =  0.005;
//Background
bool render_background = false;
std::string background_vertex_filepath = "../shaders/background.vert";
std::string background_fragment_filepath = "../shaders/background.frag";
vec2 dimension(250,250);
const float background_clamp = kernel.function(0.0, smoothing_radius);
const float opacity = 1.0;  //Don't think this does anyhthing...
//Border
bool render_border = true;
std::string border_vertex_filepath = "../shaders/border.vert";
std::string border_fragment_filepath = "../shaders/border.frag";
int line_width = 100;


void calc_density(std::vector<Particle>& particles, std::vector<float>& background_texture, vec2 dimension, float x_lim, float y_lim)
{
    //Set background density elements to 0
    for (auto& val : background_texture) 
    {
        val = 0.0;
    }

    for (int i = 0; i < dimension[0]; i++)
    {
        for (int j = 0; j < dimension[1]; j++)
        {
            float x = 2.0 * x_lim * i/dimension[0] - x_lim;
            float y = 2.0 * y_lim * j/dimension[1] - y_lim;
            vec2 sample_pos(x,y);

            float val = 0.0;
            for (auto& particle : particles)
            {
                val += particle.mass * particle.smoothing_kernel(sample_pos);
            }
            background_texture[dimension[0] * j + i] = val;

        }
    }
}


int main()
{
    //Initialise system
    System system(gravity, rest_density, stiffness, collision_damping, x_lim, y_lim);
    //std::vector<Particle> particles = uniform_init(num_particles, particle_mass, smoothing_radius, kernel, range, x_lim, y_lim);
    system.particles = uniform_random_init(num_particles, particle_mass, smoothing_radius, &kernel, vx_lim, vy_lim, x_lim, y_lim);

    std::vector<float> background_texture(dimension[0] * dimension[1], 0.0);

    //Initialise renderer
    Renderer<Particle> renderer(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);

    //Build particle vertex and fragment shaders
    renderer.add_particles(system.particles, particle_vertex_filepath, particle_fragment_filepath, rendering_radius, velocity_clamp);
    if (render_background) renderer.add_background(background_texture, dimension, background_vertex_filepath, background_fragment_filepath, background_clamp, opacity, vec2(x_lim, y_lim));
    if (render_border) renderer.add_border(border_vertex_filepath, border_fragment_filepath, line_width, vec2(x_lim, y_lim));

    while (!glfwWindowShouldClose(renderer.window))
    {
        system.simulation_step(time_step);

        if (render_background) calc_density(system.particles, background_texture, dimension, x_lim, y_lim);
        
        //Draw particles and move to next frame
        renderer.draw();
        renderer.next_frame();
    }
}