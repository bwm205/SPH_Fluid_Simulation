#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "particle_renderer.hpp"
#include "utils.hpp"    


template<typename Particle>
Particle_Renderer<Particle>::Particle_Renderer(std::vector<Particle>* particles, std::string vertex_filepath, std::string fragment_filepath, float rendering_radius, float velocity_clamp)
:   particles(particles),
    rendering_radius(rendering_radius),
    velocity_clamp(velocity_clamp)
{
    //Compile shader program +reserve GPU buffers
    program = build_shaders(vertex_filepath, fragment_filepath);
    build_buffers();
}


template<typename Particle>
Particle_Renderer<Particle>::~Particle_Renderer()
{
    //Explicitly delete buffer data
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &instVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(program);

    //No need to free up pointer since memory not owned, just a reference to other memory
}


template<typename Particle>
void Particle_Renderer<Particle>::build_buffers()
{
    //Vertex coords for our circle quad
    const float quad[] = {-1,-1,  1,-1,  1,1, 
                          -1,-1,  1,1,  -1,1}; 

    //Generate vertex array (describes how vertex information stored in buffer) and buffer (GPU buffer to store vertex data)
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    //Make this vertex array + buffer the current context
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    //Transfer vertex info (our quad vertex points) into GPU memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);         //STATIC_DRAW specifies this data will be kept constant and drawn regularly
    /*Provide metadata to use uploaded data
    Arguments given: position of attribute (ie:layout=0), number of vertex components (vec2 = x,y here),
    data type, normalise values?,
    distance (in bytes) between, offset to 1st vertex in buffer (used for if buffer object for many attributes)*/
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    glEnableVertexAttribArray(0);       //Enable attribute use during rendering


    //Instance buffer for instance data (e.g. position velocity). Since objects identical, vertex data stored once and used for each instance - more efficient
    glGenBuffers(1,&instVBO);                           //Gen buffer
    glBindBuffer(GL_ARRAY_BUFFER,instVBO);              //make current context
    glBufferData(GL_ARRAY_BUFFER,0,nullptr,GL_DYNAMIC_DRAW);    //No data transfered since changes at each render

    //Provide metadata for reading buffer data
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0); //Notice stride of 4*sizeof(float). Since multiple data stored in one buffer, we need to know how far to jump to next instance (not just size of input 2*sizeof(float))
    glVertexAttribDivisor(1,1);     //1 in second argument ensures we only advance per instance (per new circle) not per vertex. 0 (per vertex) is default and = N loops every N instances
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));  //Notice buffer starts at 2*sizeof(float), half way through data
    glVertexAttribDivisor(2,1);


    //Bind before setting uniforms
    glUseProgram(program);      

    //Reference to uniform buffer data
    GLint location_particle_radius = glGetUniformLocation(program, "particle_radius");
    GLint location_velocity_clamp = glGetUniformLocation(program, "velocity_clamp");

    //Upload uniform variables to shader   
    glUniform1f(location_particle_radius, rendering_radius); 
    glUniform1f(location_velocity_clamp, velocity_clamp);
}


template<typename Particle>
void Particle_Renderer<Particle>::draw()
{
    //Activate shader program
    glUseProgram(program);

    //Upload instance data
    std::vector<float> instance_data;
    instance_data.reserve(particles->size() * 4);     //Increases instance data array to number of particles in sim (number of attributes = 4)
    for (auto& particle: *particles)
    {
        instance_data.push_back(particle.position.x);
        instance_data.push_back(particle.position.y);
        instance_data.push_back(particle.velocity.x);
        instance_data.push_back(particle.velocity.y);
    }

    //Bind + upload uniform buffer data
    glBindBuffer(GL_ARRAY_BUFFER, instVBO);      //Not sure we formally need this line if we dont bind to any other buffer data?
    glBufferData(GL_ARRAY_BUFFER,instance_data.size()*sizeof(float),instance_data.data(),GL_DYNAMIC_DRAW);  //Upload new buffer data. inst.data() gives pointer to data. GL_DYNAMIC_DRAW means data will be continuously modified

    //Bind vertex object
    glBindVertexArray(quadVAO);         //How does the vertex

    //Draw data
    glDrawArraysInstanced(GL_TRIANGLES,0,6,particles->size());  //Arguments : Primitives to render; index to start at; number of indices to be rendered; number of instances to render. Behaves as glDrawArrays but for multiple instances
}