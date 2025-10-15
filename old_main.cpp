#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <iostream>
using namespace glm;

const int WINDOW_WIDTH = 3024;
const int WINDOW_HEIGHT = 1964;

// ===== circle shaders =====
//  Needs to be char* ?? Can I use a c++ string??
const std::string circleVtx = R"( 
#version 330 core
layout(location = 0) in vec2 quad_position;   // Quad corner  (quad is two triangles making a square)
layout(location = 1) in vec2 particle_position;   // Particle center
layout(location = 2) in vec2 particle_velocity;   // Particle velocity

uniform float uRadius;
uniform vec2 uAspect; // (1, aspectRatio)      //Aspect ratio given as 1:ratio

out vec2 vLocal;
out float vSpeed;

void main(){
    // Calculate vertex position
    vec2 pos = particle_position + quad_position * uRadius * uAspect;       // This is essentially only multiplying the y coordinate by appropriate aspect ratio

    //Vertex position output
    gl_Position = vec4(pos, 0, 1);      // gl_Position variable gives output. Last 2 components z position and number used for projection matrix (keep as 1)?
    
    //Output to fragment shader
    vLocal = quad_position;      // map quad [-1,1]^2 to circle local space
    vSpeed = length(particle_velocity);
}
)";

const std::string circleFrag = R"(
#version 330 core
in vec2 vLocal;
in float vSpeed;
out vec4 FragColor;

void main(){
    // circle mask
    if(length(vLocal) > 1.0) discard;       //length() gives vector norm. If longer than circle distance, throw away

    // velocity color mapping (white to red)
    float blend_factor = clamp(vSpeed*5.0, 0.0, 1.0);                      //Gives how much red to keep. Clamps between 0 and 1
    vec3 col = mix(vec3(1.0,1.0,1.0), vec3(1.0,0.0,0.0), blend_factor);    //Blends white and red by given factor

    FragColor = vec4(col,1.0);
}
)";


// ===== compile helper =====
GLuint build_shader(GLenum type,const std::string& source_code){
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


struct Particle {
    vec2 pos, vel;
    Particle(vec2 p, vec2 v):pos(p),vel(v){}
    void update(float dt){
        pos += vel*dt;
        // simple bounce at screen edges
        if(pos.x>0.95f||pos.x<-0.95f) vel.x*=-1;
        if(pos.y>0.95f||pos.y<-0.95f) vel.y*=-1;
    }
};


int main(){
    //Boot up GLFW (I think window hints give boot specifics)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    //Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"",0,0);
    if(!window){        //Check window compilation
        std::cerr<<"Window fail\n";
        return -1; 
        }
    glfwMakeContextCurrent(window);     //Make window current context (OpenGL knows to go there)

    glewExperimental = GL_TRUE;         //Purpose of this variable ???
    if(glewInit()!=GLEW_OK){            //Initialise glew (pointers to OpenGL functions)
        std::cerr<<"GLEW fail\n";
        return -1;
        }

    // Compile shaders
    GLuint program = glCreateProgram();
    GLuint vertex_shader = build_shader(GL_VERTEX_SHADER, circleVtx);
    GLuint fragment_shader = build_shader(GL_FRAGMENT_SHADER, circleFrag);
    glAttachShader(program, vertex_shader); glAttachShader(program, fragment_shader);  //Attach shaders to program
    glLinkProgram(program);     //Link program the same as standard C++ linking during compilation

    //Vertex coords for our circle quad
    float quad[] = {
        -1,-1,  1,-1,  1,1,
        -1,-1,  1,1,  -1,1
    };

    //Generate vertex array (describes how vertex information stored in buffer) and buffer (GPU buffer to store vertex data)
    GLuint quadVAO,quadVBO;
    glGenVertexArrays(1,&quadVAO);
    glGenBuffers(1,&quadVBO);
    //Make this vertex array + buffer the current context
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
    //Transfer vertex info (our quad vertex points) into GPU memory
    glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);         //STATIC_DRAW specifies this data will be kept constant and drawn regularly
    /*Provide metadata to use uploaded data
    Arguments given: position of attribute (ie:layout=0), number of vertex components (vec2 = x,y here),
    data type, normalise values?,
    distance (in bytes) between, offset to 1st vertex in buffer (used for if buffer object for many attributes)*/
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    glEnableVertexAttribArray(0);

    //Instance buffer for instance data (e.g. position velocity). Since objects identical, vertex data stored once and used for each instance - more efficient
    //Why no vertex array?
    GLuint instVBO;
    glGenBuffers(1,&instVBO);                           //Gen buffer
    glBindBuffer(GL_ARRAY_BUFFER,instVBO);              //make current context
    glBufferData(GL_ARRAY_BUFFER,0,nullptr,GL_DYNAMIC_DRAW);    //Transfer to GPU

    //Provide metadata for reading buffer data. D
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0); //Notice stride of 4*sizeof(float). Since multiple data stored in one buffer, we need to know how far to jump to next instance (not just size of input 2*sizeof(float))
    glVertexAttribDivisor(1,1);     //1 in second argument ensures we only advance per instance (per new circle) not per vertex. 0 (per vertex) is default and = N loops every N instances
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));  //Notice buffer starts at 2*sizeof(float), half way through data
    glVertexAttribDivisor(2,1);

    //Initialise particles
    std::vector<Particle> ps;
    for(int i=0;i<200;i++){
        float x = -0.9f + 1.8f*rand()/float(RAND_MAX);
        float y = -0.9f + 1.8f*rand()/float(RAND_MAX);
        vec2 v = 0.5f*vec2(rand()/float(RAND_MAX)-0.5f,rand()/float(RAND_MAX)-0.5f);
        ps.emplace_back(vec2(x,y),v);
    }

    while(!glfwWindowShouldClose(window)){

        //Update particles
        for(auto& ptl:ps) ptl.update(0.01f);

        //Upload instance data
        std::vector<float> inst;
        inst.reserve(ps.size()*4);     //Increases instance data array to number of particles in sim (number of attributes = 4)
        for(auto& ptl:ps){
            inst.push_back(ptl.pos.x);
            inst.push_back(ptl.pos.y);
            inst.push_back(ptl.vel.x);
            inst.push_back(ptl.vel.y);
        }

        glBindBuffer(GL_ARRAY_BUFFER,instVBO);      //Not sure we formally need this line if we dont bind to any other buffer data?
        glBufferData(GL_ARRAY_BUFFER,inst.size()*sizeof(float),inst.data(),GL_DYNAMIC_DRAW);  //Upload new buffer data. inst.data() gives pointer to data. GL_DYNAMIC_DRAW means data will be continuously modified

        //Dynamically get aspect ratio (if we scale window?)
        int ww,hh; glfwGetFramebufferSize(window,&ww,&hh);
        float aspect = float(ww)/float(hh);

        //Activate given shader program
        glUseProgram(program);
        //Upload constant variables to shader
        glUniform1f(glGetUniformLocation(program,"uRadius"),0.03f);     
        glUniform2f(glGetUniformLocation(program,"uAspect"),1.0f,aspect);

        glBindVertexArray(quadVAO);         //How does the vertex
        glDrawArraysInstanced(GL_TRIANGLES,0,6,ps.size());  //Arguments : Primitives to render; index to start at; number of indices to be rendered; number of instances to render

        glfwSwapBuffers(window);     //We use a front and back buffer. At each frame, we draw into one and show the other so we dont see the screen drawing. This swaps which buffer we draw to and show
        glfwPollEvents();            //Process window messages (resizing, close, input ...)

        glClearColor(0,0,0,1);          //Background colour to use when clearing screen (sets in openGL context)
        glClear(GL_COLOR_BUFFER_BIT);   //Clear colour buffer
    }
}