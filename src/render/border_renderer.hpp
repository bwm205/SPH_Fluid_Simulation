#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Border_Renderer
{
private:
    //Reference to program, GPU buffers and textures for rendering
    GLuint quadVAO, quadVBO;
    GLuint program;

    int line_width;

    vec2 bounds;
    GLint location_bounds;

    void build_buffers();

public:
    Border_Renderer(std::string vertex_filepath, std::string fragment_filepath, int line_width = 5, vec2 bounds = vec2(1.0,1.0));
    ~Border_Renderer();

    void draw();
};