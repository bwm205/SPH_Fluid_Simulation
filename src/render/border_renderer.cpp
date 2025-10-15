#include <string>

#include <glm/glm.hpp>

#include "border_renderer.hpp"
#include "utils.hpp"


Border_Renderer::Border_Renderer(std::string vertex_filepath, std::string fragment_filepath, int line_width, vec2 bounds)
:   line_width(line_width),
    bounds(bounds)
{
    program = build_shaders(vertex_filepath, fragment_filepath);
    build_buffers();
}


Border_Renderer::~Border_Renderer()
{
    //Explicitly delete buffer data
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(program);
}


void Border_Renderer::build_buffers()
{
    //Vertex coords for our border quad. We are drawing via a GL_LINE_lOOP so we need not input two triangles, simply the vertexes we want to draw between
    const float quad[] = {-1,-1,  1,-1,  1,1, -1,1}; 

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
    GLint location_bounds = glGetUniformLocation(program, "bounds");
    glUniform2f(location_bounds, bounds[0], bounds[1]);
}


void Border_Renderer::draw()
{
    glUseProgram(program);

    glBindVertexArray(quadVAO);

    glLineWidth(line_width);
    glDrawArrays(GL_LINE_LOOP, 0, 4);   //4 Vertices since 4 quad points with line loop, not 2 triangles
}