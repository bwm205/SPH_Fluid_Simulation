#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "renderer.hpp"
#include "particle_renderer.hpp"
#include "background_renderer.hpp"
#include "border_renderer.hpp"


template<typename Particle>
Renderer<Particle>::Renderer(const int window_width, const int window_height, std::string window_name)
{
    //Boot up GLFW (I think window hints give boot specifics)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    //Create window
    window = glfwCreateWindow(window_width, window_height, window_name.c_str(), 0,0);
    if(!window)
    {        //Check window compilation
        throw std::runtime_error("Failed to build window");    
    }
    glfwMakeContextCurrent(window);     //Make window current context (OpenGL knows to go there)

    //Initialise glew (pointers to OpenGL functions)
    glewExperimental = GL_TRUE;         //Purpose of this variable ???
    if(glewInit()!=GLEW_OK)
    {            
        throw std::runtime_error("Glew init failed");
    }
}


template<typename Particle>
void Renderer<Particle>::add_particles(std::vector<Particle>& particles, std::string vertex_filepath, std::string fragment_filepath, float rendering_radius_init, float velocity_clamp_init)
{
    //Initialise sub_renderer
    particle_renderer = std::make_unique<Particle_Renderer<Particle>> (&particles, vertex_filepath, fragment_filepath, rendering_radius_init, velocity_clamp_init);

    //Add to render list
    render_instructions[2] = true;
    draw_functions[2] = [&]() {particle_renderer->draw(); };
}


template<typename Particle>
void Renderer<Particle>::add_background(std::vector<float>& background_texture, vec2 dimension, std::string vertex_filepath, std::string fragment_filepath, float background_clamp, float opacity, vec2 bounds)
{
    //Initialise sub_renderer
    background_renderer = std::make_unique<Background_Renderer> (&background_texture, dimension, vertex_filepath, fragment_filepath, background_clamp, opacity, bounds);

    //Add to render list
    render_instructions[0] = true;
    draw_functions[0] = [&]() {background_renderer->draw(); };      //Draw background first
}


template<typename Particle>
void Renderer<Particle>::add_border(std::string vertex_filepath, std::string fragment_filepath, int line_width, vec2 bounds)
{
    //Initialise sub_renderer
    border_renderer = std::make_unique<Border_Renderer> (vertex_filepath, fragment_filepath, line_width, bounds);

    //Add to render list
    render_instructions[1] = true;
    draw_functions[1] = [&]() {border_renderer->draw(); };      //Draw background first
}


template<typename Particle>
void Renderer<Particle>::draw()
{
    //Render requested objects
    for (int i = 0; i < render_instructions.size(); i++)
    {
        if (render_instructions[i])
        {
            draw_functions[i]();
        }
    }
}


template<typename Particle>
void Renderer<Particle>::next_frame()
{
    //Swap buffers
    glfwSwapBuffers(window);     //We use a front and back buffer. At each frame, we draw into one and show the other so we dont see the screen drawing. This swaps which buffer we draw to and show
    glfwPollEvents();            //Process window messages (resizing, close, input ...)

    //Clear and draw background
    glClearColor(0,0,0,1);          //Background colour to use when clearing screen (sets in openGL context)
    glClear(GL_COLOR_BUFFER_BIT);   //Clear colour buffer
}


