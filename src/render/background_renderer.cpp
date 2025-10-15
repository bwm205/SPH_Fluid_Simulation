#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "background_renderer.hpp"
#include "utils.hpp"

using namespace glm;


Background_Renderer::Background_Renderer(std::vector<float>* background_texture, vec2 dimension, std::string vertex_filepath, std::string fragment_filepath, float background_clamp, float opacity, vec2 bounds)
:   background_texture(background_texture),
    dimension(dimension),
    background_clamp(background_clamp),
    opacity(opacity),
    bounds(bounds)

{
    //Generate texture reference
    glGenTextures(1, &texture_identifier);
    glBindTexture(GL_TEXTURE_2D, texture_identifier);       //Make current context
    //Create and resize texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimension[0], dimension[1], 0, GL_RED, GL_FLOAT, nullptr);
    //Choose how to interpolate between texture values (if texture different dimension to screen)
    //MIN_FILTER and MAG_FILTER specify interpolation methods for zooming out of (texture too detailed) and zooming into (texture insufficient detail) respectively
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Compile shader program +reserve GPU buffers
    program = build_shaders(vertex_filepath, fragment_filepath);
    build_buffers();
}

Background_Renderer::~Background_Renderer()
{
    //Explicitly delete buffer data
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(program);

    //No need to free up pointer since memory not owned, just a reference to other memory
}


void Background_Renderer::build_buffers()
{
    //Vertex coords for our background quad
    const float quad[] = {-1,-1,  1,-1,  1,1, 
                          -1,-1,  1,1,  -1,1}; 

    //Generate vertex array (describes how vertex information stored in buffer) and buffer (GPU buffer to store vertex data)
    //For detailed comments, see particle_renderer
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
 
    //Information for parsing buffer
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    glEnableVertexAttribArray(0); 


    //Bind before setting uniforms 
    glUseProgram(program); 

    //Reference to uniform buffer data
    GLint location_background_texture = glGetUniformLocation(program, "background_texture");
    GLint location_background_clamp = glGetUniformLocation(program, "background_clamp");
    GLint location_opacity = glGetUniformLocation(program, "opacity");
    GLint location_bounds = glGetUniformLocation(program, "bounds");

    //Upload uniform variables to shader (background uploaded at render time)
    glUniform1f(location_background_clamp, background_clamp);
    glUniform1f(location_opacity, opacity);
    glUniform2f(location_bounds, bounds[0], bounds[1]);
}


void Background_Renderer::draw()
{
    //Activate given shader program
    glUseProgram(program);

    //Bind vertex array
    glBindVertexArray(quadVAO);

    //Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_identifier);
    //Upload texture data
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dimension[0], dimension[1], GL_RED, GL_FLOAT, background_texture->data());  //->data() points to 1st element itself, not to wrapping std::vector
    //Tell shader to use texture unit 0 (unit in set of textures)
    glUniform1i(location_background_texture, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}


