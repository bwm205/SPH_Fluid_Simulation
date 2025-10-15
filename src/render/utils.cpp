#include <GL/glew.h>

#include <iostream>
#include <fstream>

#include "utils.hpp"


GLuint build_shaders(std::string vertex_filepath, std::string fragment_filepath)
{
    GLuint program = glCreateProgram();

    //Load in shaders at strings
    std::string vertex_string = load_shader(vertex_filepath);
    std::string fragment_string = load_shader(fragment_filepath);

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_string);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_string);
    glAttachShader(program, vertex_shader); glAttachShader(program, fragment_shader);  //Attach shaders to program
    glLinkProgram(program);     //Link program the same as standard C++ linking during compilation

    return program;
}


//Load shader as string from external file
std::string load_shader(const std::string& filepath)
{
    std::string result = "";        //Initialise single string for loaded shader
    std::string line = "";          //Space to load in file line by line
    std::ifstream file(filepath);   //Filepath

    //Load shader line by line
    if (file.is_open())
    {
        while(std::getline(file, line))
        {
            result += line + "\n";          //std::getline strips \n, hence add
        }
        file.close();
    }
    //Flag as error if cannot be loaded
    else
    {
        std::cerr << "Failed to open shader file : " + filepath << std::endl;
    }

    return result;
}


GLuint compile_shader(GLenum type, const std::string& source_code)
{
    /*GLuint believe is a handle which  points towards the shader (in memory?)
    GLenum type gives the shader type (e.g. vertex, fragment ...)
    source_code points to our shader. Stored as a string (char* is classic c, maybe upgrade to c++ string)*/

    //Compile shader
    GLuint shader = glCreateShader(type);
    const char* src = source_code.c_str();  //Convert C++ to c type string
    glShaderSource(shader,1, &src ,NULL);     //Sets shader code to that contained in string source_code. Can combine multiple strings into shader (input as array), we have only 1 hence 1 argument
    glCompileShader(shader);

    //Check shader compilation okay
    GLint ok; glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
    if(!ok){                    //Notify is shader compilation failed
        char log[512];          //Holds compilation info
        glGetShaderInfoLog(shader,512,NULL,log);
        std::cerr<<"Shader error: "<<log<<"\n"; 
        }

    return shader;
}