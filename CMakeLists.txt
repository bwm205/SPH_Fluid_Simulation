#Define project
cmake_minimum_required(VERSION 3.14)  #CMake version
project(FluidSimulation)                  #Project name

set(CMAKE_CXX_STANDARD 17)  #Set C++ standard

# Find libraries installed via Homebrew
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

#Build program
#add_executable(FluidSim old_main.cpp)
add_executable(FluidSim main.cpp src/render/renderer.tpp src/render/particle_renderer.tpp src/render/background_renderer.cpp src/render/border_renderer.cpp src/render/utils.cpp src/simulation/simulate.cpp src/simulation/smoothed_particle.cpp src/simulation/initialisation.cpp)

#Link libraries
target_link_libraries(FluidSim PRIVATE
    GLEW::GLEW
    glfw
    "-framework OpenGL"
)

# Include directories for GLM (header-only)
target_include_directories(FluidSim PRIVATE ${GLM_INCLUDE_DIR})

