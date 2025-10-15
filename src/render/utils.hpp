#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>

GLuint build_shaders(std::string vertex_filepath, std::string fragment_filepath);
std::string load_shader(const std::string& filepath);
GLuint compile_shader(GLenum type, const std::string&source_code);