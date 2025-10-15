#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

using namespace glm;

class Background_Renderer
{
private:
    //Reference to program, GPU buffers and textures for rendering
    GLuint quadVAO, quadVBO;
    GLuint texture_identifier;
    GLuint program;

    std::vector<float>* background_texture;
    vec2 dimension;                             //dimension of background texture as matrix (width, height)
    float background_clamp;
    float opacity;
    vec2 bounds;
    GLint location_background_texture;
    GLint location_background_clamp;
    GLint location_opacity;
    GLint location_bounds;

    void build_buffers();

public:
    Background_Renderer(std::vector<float>* background_texture, vec2 dimension, std::string vertex_filepath, std::string fragment_filepath, float background_clamp = 1.0, float opacity = 1.0, vec2 bounds = vec2(1.0,1.0));
    ~Background_Renderer();

    void draw();
};

