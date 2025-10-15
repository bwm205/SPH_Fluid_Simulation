#version 330 core
layout(location = 0) in vec2 quad_position;   //Position of vertex (in screen corner)

uniform vec2 bounds;        //Bounds of simualtion given as (x_range, y_range). Given in screen space (-1 to 1)

out vec2 texture_coordinate;

void main()
{
    //Map [-1,1] quad positions to [0,1] texture coordinates
    texture_coordinate = (quad_position + 1.0) * 0.5;

    //Convert to world space (vertex position scaled by bounds)
    gl_Position = vec4(quad_position * bounds, 0.0, 1.0);
}