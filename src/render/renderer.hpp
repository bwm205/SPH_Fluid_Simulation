#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "particle_renderer.hpp"
#include "background_renderer.hpp"
#include "border_renderer.hpp"


//Add subrenderer class for polymorphism. Use virtual arguments


template<typename Particle>
class Renderer
{
private:
    //Sub-renderers
    std::unique_ptr<Particle_Renderer<Particle>> particle_renderer;
    std::unique_ptr<Background_Renderer> background_renderer;
    std::unique_ptr<Border_Renderer> border_renderer;

    //Gives whether or not to render given scene component and the functions to do so (extend when more added)
    std::array<bool,3> render_instructions = {false};
    std::array<std::function<void()>,3> draw_functions;


public:
    GLFWwindow* window;

    Renderer(const int window_width = 800, const int window_height = 600, const std::string window_name = "");

    //Sub renderer initialisers
    void add_particles(std::vector<Particle>& particles, std::string vertex_filepath, std::string fragment_filepath, float rendering_radius_init, float velocity_clamp_init = 1.0);
    void add_background(std::vector<float>& background_texture, vec2 dimension, std::string vertex_filepath, std::string fragment_filepath, float background_clamp = 1.0, float opacity = 1.0, vec2 bounds = vec2(1.0,1.0));
    void add_border(std::string vertex_filepath, std::string fragment_filepath, int line_width = 5, vec2 bounds = vec2(1.0,1.0));

    //Rendering
    void draw();
    void next_frame();
};

#include "renderer.tpp"
